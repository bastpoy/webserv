#include "Header.hpp"

#define TIMEOUT 5000
#define BUFER_SIZE 4096
#define FD_NUMBER 100

int check_fd_valid(int fd) 
{
    return fcntl(fd, F_GETFL) != -1;
}

//Fill data epoll with server iterator
struct epoll_event Server::fillEpoolDataIterator(int sockfd, std::vector<Server>::iterator itbeg, ConfigParser &config)
{
	t_serverData *data = new t_serverData;

    GlobalLinkedList::insert(data);
	struct epoll_event event;

    this->data = data;
    config.setListData(data);

	data->sockfd = sockfd;
	data->port = itbeg->getPort();
	data->server_name = itbeg->getServerName();
	data->path = itbeg->getRoot();
	data->maxBody = itbeg->getMaxBody();
	data->autoindex = itbeg->getAutoIndex();
	data->index = itbeg->getIndex();
	data->errorPage = itbeg->getErrorPage();
	data->cgiPath = itbeg->getCgiPath();
	data->redir = itbeg->getRedir();
	data->location = itbeg->getLocation();
	data->buffer = "";
	data->header = "";
	data->body = "";
	data->cgi = NULL;

	event.events = EPOLLIN; // Monitor for input events
	//I stock the info server on the event ptr data
	event.data.fd = sockfd;
	event.data.ptr = static_cast<void*>(data);

	return (event);
}

//Fill data epoll with info from the new client
struct epoll_event Server::fillEpoolDataInfo(int &client_fd, t_serverData *info)
{
	t_serverData *data = new t_serverData();

    GlobalLinkedList::insert(data);
	data->sockfd = client_fd;
	data->port = info->port;
	data->server_name = info->server_name;
	data->path = info->path;
	data->maxBody = info->maxBody;
	data->autoindex = info->autoindex;
	data->index = info->index;
	data->errorPage = info->errorPage;
	data->cgiPath = info->cgiPath;
	data->redir = info->redir;
	data->location = info->location;
	data->buffer = "";
	data->header = "";
	data->body = "";
	data->cgi = NULL;
	// data->requestAllow.push_back("GET");

	struct epoll_event client_event;

	client_event.events = EPOLLIN;
	client_event.data.fd = client_fd;
	client_event.data.ptr = static_cast<void*>(data);

	// std::pair<int, t_serverData> pair(client_fd, *data);
	// data->fdEpollLink->insert(pair);

	return(client_event);
}

//IP managing, Binding, Listening
void Server::setupSocket(int &sockfd, struct sockaddr_in &addr, std::vector<Server>::iterator itbeg)
{
    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;  // Use local IP

    std::string ip = itbeg->getIP();
    std::string port = itbeg->getPort();  // Assuming you have a getPort() method
	std::cout << "ip: " << ip << "; Port: " << port << std::endl;
    int status = getaddrinfo(ip.c_str(), port.c_str(), &hints, &result);
    if (status != 0) {
        closeAllFileDescriptors();
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        throw Response::Error();
    }
    
    // Convert the resolved address to sockaddr_in
    struct sockaddr_in *resolved_addr = reinterpret_cast<struct sockaddr_in*>(result->ai_addr);
    memcpy(&addr, resolved_addr, sizeof(struct sockaddr_in));

	if (bind(sockfd, result->ai_addr, result->ai_addrlen) < 0) 
    {
        closeAllFileDescriptors();
        freeaddrinfo(result);
		std::cout << "\nBIND: ";
		throw Response::ErrorCreatingSocket(strerror(errno));
	}

	//listen on the current socket created
	if (listen(sockfd, 10) < 0)
	{
        closeAllFileDescriptors();
        freeaddrinfo(result);
		std::cout <<"LISTEN: " << strerror(errno);
		throw Response::Error();
	}
    freeaddrinfo(result);
}

// Configuration network
void Server::configuringNetwork(std::vector<Server>::iterator &itbeg, ConfigParser &config, int &epoll_fd)
{
	std::cout << "The socket server fd are:" << std::endl;

	while(itbeg != config.getServers().end())
	{
		//creation addrinfo struc to stock my addrinfo informations
		struct sockaddr_in addr;
		
		//I create my socket
		int sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd == -1)
		{
			std::cout << "error creating socket" << std::endl;
			errorCloseEpollFd(epoll_fd, 2);
		}

		std::cout << sockfd << " and ";
		//add properties to allow the socket to be reusable even if it is in time wait
		int opt = 1;
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
			errorCloseEpollFd(epoll_fd, 3);
		
		// std::cout << itbeg->getPort() << "-" << itbeg->getServerName() << "-" << std::endl;

		//fill my sockaddr_in addr with the result of getaddrinfo
		addr.sin_family = AF_INET;
		addr.sin_port = htons(atoi(itbeg->getPort().c_str()));

		setupSocket(sockfd, addr, itbeg);

		//create a new instance of epool_event to stock info        
		struct epoll_event event = this->fillEpoolDataIterator(sockfd, itbeg, config);

		//add the epoll event to my epoll_fd instance
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &event) == -1)
			errorCloseEpollFd(epoll_fd, 4);
		
		//add every socket to my set container
		this->setSocketFd(sockfd);
		itbeg++;
	}
	std::cout << std::endl;
}

int acceptConnection(int &fd, int &epoll_fd, struct sockaddr_in &client_addr)
{
	socklen_t client_addr_len = sizeof(client_addr);
	//accept the connection
	int client_fd = accept(fd, (struct sockaddr*)&client_addr, &client_addr_len);
	if (client_fd == -1)
		errorCloseEpollFd(epoll_fd, 6);
	return(client_fd);
}

bool handleRequest(std::string buffer, t_serverData *data, Cookie &cookie, std::map<int, t_serverData*> &fdEpollLink) 
{
	std::string firstLine = data->header.substr(0, data->header.find("\n"));
	std::string typeRequest = firstLine.substr(0, data->header.find(" "));

	if(typeRequest == "GET" && request_allowed("GET", data))
	{
		//if i have a redirection on the server or the location
		if(data->redir.size())
		{
			std::cout << "REDIRECTION GET" << std::endl;
			redirRequest(data->redir.begin()->second, data->sockfd, data);
		}
		// else I respond 
		else
			parseAndGetRequest(buffer, data, cookie, fdEpollLink);
	}
	//if it is a post request
	else if(typeRequest == "POST" && request_allowed("POST", data))
		postRequest(data, cookie);
	//if its a delete request
	else if(typeRequest == "DELETE" && request_allowed("DELETE", data))
		parseAndDeleteRequest(buffer, data);
	//if its not get, post or delete request
	else
		errorPage("405", data);
	return(false);
}

bool read_one_chunk(t_serverData *data) 
{
	char buffer[BUFER_SIZE];
	// Read data into the buffer
	// std::cout << BLUE "waiting before the buffer" << data->sockfd << RESET << std::endl;
	int bytes_read = recv(data->sockfd, buffer, BUFER_SIZE, 0);
	if (bytes_read < 0) 
	{
		std::cout << "Error " << errno << " reading from socket " << data->sockfd << ": " << strerror(errno) << std::endl;
		errorPage("400", data);
	} 
	// if there is a deconnection
	else if (bytes_read == 0) 
	{
		std::cout << RED "Connection closed by the client. (recv = 0) " << data->sockfd << RESET << std::endl;
		// epoll_ctl(epoll_fd, EPOLL_CTL_DEL, data->sockfd, events);
		close(data->sockfd);
		// close(data->sockfd);
		return (false); 
	}
	data->buffer.append(buffer, bytes_read);
	// std::cout << YELLOW << "size " << data->buffer.size() << " " << data->buffer << std::endl;
	//retieve the header
	size_t pos = data->buffer.find("\r\n\r\n");
	if(pos != std::string::npos)
	{
		data->header = data->buffer.substr(0, pos + 4);
		std::cout << GREEN << data->header << RESET << std::endl;
	}
	//check if buffer size - header size = contentlength
	if(static_cast<int>(data->buffer.size() - data->header.size()) == getContentLength(data->buffer, data))
	{
		// std::cout << BLUE "finish reading data first" << RESET << std::endl;
		return (true);
	}
	//if i finish reading everything
	if(static_cast<int>(data->buffer.size()) == getContentLength(data->buffer, data))
	{
		// std::cout << BLUE "finish reading data second" << RESET << std::endl;
		return true;
	}
	// std::cout << BLUE "bytes read " << bytes_read << " with sockfd "<< data->sockfd << " and sizebuffer" << data->buffer.size() << RESET <<std::endl;
	return (false);
}

void parsing_buffer(t_serverData *data, Cookie &cookie, std::map<int, t_serverData*> &fdEpollLink)
{
	//if i have a buffer so a request 
	if(!data->buffer.empty())
	{
		//i fill my header and my body of my request
		size_t pos = data->buffer.find("\r\n\r\n");
		data->header = data->buffer.substr(0, pos);
		data->body = data->buffer.substr(pos + 4, data->buffer.size() - pos + 4);
		// std::cout << MAGENTA "handling request\n" << data->header << RESET << std::endl;
		handleRequest(data->buffer, data, cookie, fdEpollLink);
	}
	else
	{
		std::cout << "no data provide" << std::endl;
	}
}

void check_timeout_cgi(t_serverData *info, std::map<int, t_serverData*> &fdEpollLink)
{
    if(info)
    {
        if(info->cgi == NULL)
        {
            std::map<int, t_serverData*>::iterator it = fdEpollLink.begin();
            //iterate through my corresponse map between fd and data struct
            while (it != fdEpollLink.end()) 
            {
                //if i have a cgi inside my struct
                if(it->second->cgi)
                {
                    //if my cgi is timeout
                    if(it->second->cgi->cgiTimeout < time(NULL))
                    {
                        std::cout << "a cgi is TIMEOUT" << std::endl;
                        std::string response = httpGetResponse("200 Ok", "text/html", readFile("./www/error/error408.html", it->second), it->second);
                        if(send(it->second->sockfd, response.c_str(), response.size(), 0) < 0)
                        {
                            std::cout << RED "error send main "<< errno << " " << strerror(errno) << RESET << std::endl;
                        }
                        close(it->second->cgi->cgifd);
                        close(it->second->sockfd);
                        delete it->second->cgi;
                        it->second->cgi = NULL;
                        std::map<int, t_serverData*>::iterator toErase = it;
                        it++;
                        fdEpollLink.erase(toErase);
                        continue;
                    }
                }
                it++;
            }
        }
    }
}

void read_cgi(t_serverData *data, struct epoll_event *events, int i, int epoll_fd)
{
	//i read my cgi which is finish
	char buffer[4096];
	int bytes_read;

	std::cout << YELLOW "before reading cgi" << RESET << std::endl;
	bytes_read = read(data->cgi->cgifd, buffer, 4096);
	if(bytes_read < 1)
	{
		std::cerr << RED "error reading the cgi: " << strerror(errno) << RESET << std::endl; 
	}
	std::cout << GREEN "reading from the cgi fd" << RESET << std::endl;
	//i put the content of the cgi response in the body
	data->body.append(buffer, bytes_read);
	//switching to epollout
	events[i].events = EPOLLOUT;
	epoll_ctl(epoll_fd, EPOLL_CTL_MOD, data->sockfd, events);
}

void manage_tserver(t_serverData *&data, struct epoll_event *events, int i, int epoll_fd)
{
    std::cout << BLUE "switching to epoolin" << RESET << std::endl;
    events[i].events = EPOLLIN;
    if(epoll_ctl(epoll_fd, EPOLL_CTL_MOD, data->sockfd, events) < 0)
    {
        std::cout << RED "Error epoll ctl catch: "<< errno << " " << strerror(errno) << RESET << std::endl;
    }
    close(data->sockfd);
    delete data;
    GlobalLinkedList::update_data(data);
    data = NULL;
}

// Fill from ServerAddr
void Server::createListenAddr(ConfigParser &config)
{
	std::vector<Server>::iterator itbeg = config.getServers().begin();
    std::map<int, t_serverData*> fdEpollLink;

	//creating the poll
	int epoll_fd = epoll_create(50);
	if (epoll_fd == -1)
		errorCloseEpollFd(epoll_fd, 7);

	Cookie cookie;
	//iterate through every server and retrieve information
	this->configuringNetwork(itbeg, config, epoll_fd);

	// EPOLL STARTING
	struct epoll_event events[MAX_EVENTS];

	std::cout << "\nWaiting for connection...\n";

	while (true) {
		//epollwait return a number corresponding to all the files descriptor
		int num_fds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (num_fds == -1) 
			errorCloseEpollFd(epoll_fd, 1);
		for (int i = 0; i < num_fds; ++i)
		{
			// std::cout << "i is equal to " << i << std::endl; 
			t_serverData *info = static_cast<t_serverData*>(events[i].data.ptr);
			int fd = info->sockfd;

			// std::cout << BLUE "actual fd: " << info->sockfd << " and number fd " << num_fds <<RESET << std::endl; 
			// check if my fd is equal to a socket for handcheck
			if(this->socketfd.find(fd) != this->socketfd.end())
			{
				// listen to add new fd to my epoll structure
				struct sockaddr_in client_addr;
				//new fd_client for communication
				int client_fd = acceptConnection(fd, epoll_fd, client_addr);
				
				// add new fd to my epoll instance
				struct epoll_event client_event = this->fillEpoolDataInfo(client_fd, info);
				// add the new fd to be control by my epoll instance
				if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event) == -1)
					errorCloseEpollFd(epoll_fd, 4);
				std::cout << "New connection established with new fd: " << client_fd << std::endl;
			}
			// Connection already etablish 
			else
			{
                //i listen for some epollin event and possible data read
                if(events[i].events & EPOLLIN)
                {
                    // if i read the content of a cgi
                    if(info->cgi)
                    {
                        read_cgi(info, events, i, epoll_fd);
                    }
                    else if(read_one_chunk(info))
                    {
                        //if i finish read the request info i change the status of the socket
                        // std::cout << BLUE "switching to epoolout" RESET << std::endl;
                        events[i].events = EPOLLOUT;
                        epoll_ctl(epoll_fd, EPOLL_CTL_MOD, info->sockfd, events);
                    }
                }
                // if i can write to my socket
                if(events[i].events & EPOLLOUT)
                {
                    try
                    {
                        std::cout << BMAGENTA "Inside epollout" RESET << std::endl;
                        //if I have already a cgi and ready to return information
                        if(info->cgi)
                        {
                            std::cout << GREEN "sending cgi response\n" RESET;
                            std::string response = httpGetResponse("200 Ok", "text/html", info->body, info);
                            std::cout << MAGENTA "Response: " << response << RESET << std::endl;
                            if(send(info->sockfd, response.c_str(), response.size(), 0) < 0)
                            {
                                std::cout << "error sending CGI response\n";
                                Response::sendResponse("500", "text/html", "<h1>500 Internal Server Error</h1>", info);
                            }
                            fdEpollLink.erase(info->sockfd);
                            close(info->cgi->cgifd);
                            delete info->cgi;
                            info->cgi = NULL;
                        }
                        // parse the data
                        else
                            parsing_buffer(info, cookie, fdEpollLink);
                        // if i finish sending the info I change the status of the socket
                        manage_tserver(info, events, i, epoll_fd);
                    }
                    catch(const std::exception& e)
                    {
                        std::cout << RED << "Error catch" << RESET << std::endl;
                        if(info->cgi)
                        {
                            std::cout << GREEN "CGI EXIST BUT RETURN" RESET << std::endl;
                            continue;
                        }
                        manage_tserver(info, events, i, epoll_fd);
                        std::cerr << e.what() << '\n';
                    }
                }
                //check if i have a cgi running in all my fd open
                check_timeout_cgi(info, fdEpollLink);
			}
		}
	}
}
