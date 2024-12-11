#include "Header.hpp"

#define TIMEOUT 5000
#define BUFER_SIZE 4096
#define FD_NUMBER 100


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
    data->requestAllow = itbeg->getAllowedMethods();
	data->buffer = "";
	data->header = "";
	data->body = "";
	data->cgi = NULL;
    data->isDownload = false;

	event.events = EPOLLIN; // Monitor for input events
	//I stock the info server on the event ptr data
	event.data.ptr = static_cast<void*>(data);

	return (event);
}

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
    data->requestAllow = info->requestAllow;
	data->buffer = "";
	data->header = "";
	data->body = "";
	data->cgi = NULL;
    data->isDownload = false;

    struct epoll_event client_event;
	client_event.events = EPOLLIN;
	client_event.data.ptr = static_cast<void*>(data);

	return(client_event);
}

void Server::setupSocket(int &sockfd, struct sockaddr_in &addr, std::vector<Server>::iterator itbeg)
{
	struct addrinfo hints, *result;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	std::string ip = itbeg->getIP();
	std::string port = itbeg->getPort();
	std::cout << "ip: " << ip << "; Port: " << port << std::endl;
	int status = getaddrinfo(ip.c_str(), port.c_str(), &hints, &result);
	if (status != 0) {
		closeAllFileDescriptors();
		std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
		throw Response::Error();
	}
	
	struct sockaddr_in *resolved_addr = reinterpret_cast<struct sockaddr_in*>(result->ai_addr);
	memcpy(&addr, resolved_addr, sizeof(struct sockaddr_in));

	if (bind(sockfd, result->ai_addr, result->ai_addrlen) < 0) 
	{
		closeAllFileDescriptors();
		freeaddrinfo(result);
		std::cout << "\nBIND: ";
		throw Response::ErrorCreatingSocket(strerror(errno));
	}

	if (listen(sockfd, 10) < 0)
	{
		closeAllFileDescriptors();
		freeaddrinfo(result);
		std::cout <<"LISTEN: " << strerror(errno);
		throw Response::Error();
	}
	freeaddrinfo(result);
}

void Server::configuringNetwork(std::vector<Server>::iterator &itbeg, ConfigParser &config, int &epoll_fd)
{
	while(itbeg != config.getServers().end())
	{
		struct sockaddr_in addr;
		
		int sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd == -1)
		{
            std::cout << "error creating socket" << std::endl;
            errorCloseEpollFd(epoll_fd, 2);
		}

		//add properties to allow the socket to be reusable even if it is in time wait
		int opt = 1;
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
			errorCloseEpollFd(epoll_fd, 3);

		addr.sin_family = AF_INET;
		addr.sin_port = htons(atoi(itbeg->getPort().c_str()));

		setupSocket(sockfd, addr, itbeg);

		struct epoll_event event = this->fillEpoolDataIterator(sockfd, itbeg, config);
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &event) == -1)
			errorCloseEpollFd(epoll_fd, 4);
		this->setSocketFd(sockfd);
		itbeg++;
	}
	std::cout << std::endl;
}

int acceptConnection(int &fd, int &epoll_fd, struct sockaddr_in &client_addr)
{
	socklen_t client_addr_len = sizeof(client_addr);
	int client_fd = accept(fd, (struct sockaddr*)&client_addr, &client_addr_len);
	if (client_fd == -1)
    {
        errorCloseEpollFd(epoll_fd, 6);
    }

	return(client_fd);
}

bool handleRequest(std::string buffer, t_serverData *data, Cookie &cookie, std::map<int, t_serverData*> &fdEpollLink) 
{
	std::string firstLine = data->header.substr(0, data->header.find("\n"));
	std::string typeRequest = firstLine.substr(0, data->header.find(" "));

    request_allowed(typeRequest, data);

	if(typeRequest == "GET")
	{
		if(data->redir.size())
		{
			std::cout << "REDIRECTION GET" << std::endl;
			redirRequest(data->redir.begin()->second, data->sockfd, data);
		}
		else
			parseAndGetRequest(buffer, data, cookie, fdEpollLink);
	}
	else if(typeRequest == "POST" && request_allowed("POST", data))
		postRequest(data, cookie);
	else if(typeRequest == "DELETE" && request_allowed("DELETE", data))
		parseAndDeleteRequest(buffer, data);
	else
		errorPage("405", data);
	return(false);
}

bool read_one_chunk(t_serverData *data, struct epoll_event ev, int epoll_fd) 
{
	char buffer[BUFER_SIZE];
	int bytes_read = recv(data->sockfd, buffer, BUFER_SIZE, 0);
	if (bytes_read < 0) 
	{
        std::cout << "Error " << errno << " reading from socket " << data->sockfd << ": " << strerror(errno) << std::endl;
        errorPage("400", data);
	} 
	else if (bytes_read == 0) 
	{
		std::cout << RED "Connection closed by the client. (recv = 0) " << data->sockfd << RESET << std::endl;
		if(epoll_ctl(epoll_fd, EPOLL_CTL_DEL, data->sockfd, &ev) < 0)
        {
            std::cout << RED "Error epoll ctl catch: "<< errno << " " << strerror(errno) << RESET << std::endl;
            errorPage("500", data);
        }
		close(data->sockfd);
		return (false); 
	}
	data->buffer.append(buffer, bytes_read);
	size_t pos = data->buffer.find("\r\n\r\n");
	if(pos != std::string::npos)
	{
		data->header = data->buffer.substr(0, pos + 4);
		std::cout << GREEN << data->header << RESET << std::endl;
	}
	if(static_cast<int>(data->buffer.size() - data->header.size()) == getContentLength(data->buffer, data))
		return (true);
	if(static_cast<int>(data->buffer.size()) == getContentLength(data->buffer, data))
		return true;
	return (false);
}

void proceed_response(t_serverData *data, Cookie &cookie, std::map<int, t_serverData*> &fdEpollLink)
{
	if(!data->buffer.empty())
	{
		size_t pos = data->buffer.find("\r\n\r\n");
		data->header = data->buffer.substr(0, pos);
		data->body = data->buffer.substr(pos + 4, data->buffer.size() - pos + 4);
		handleRequest(data->buffer, data, cookie, fdEpollLink);
	}
}

void manage_tserver(t_serverData *&data, struct epoll_event *events, int i, int epoll_fd)
{
    std::cout << BLUE "switching to epoolin" << RESET << std::endl;
    events[i].events = EPOLLIN;
    if(epoll_ctl(epoll_fd, EPOLL_CTL_MOD, data->sockfd, events) < 0)
    {
        std::cout << RED "Error epoll ctl catch: "<< errno << " " << strerror(errno) << RESET << std::endl;
        errorPage("500", data);
    }
    close(data->sockfd);
    GlobalLinkedList::update_data(data);
    data->body.erase();
    data->buffer.erase();
    data->header.erase();
    delete data;
    data = NULL;
}

void Server::createListenAddr(ConfigParser &config)
{
	std::vector<Server>::iterator itbeg = config.getServers().begin();
	std::map<int, t_serverData*> fdEpollLink;

	int epoll_fd = epoll_create(50);
	if (epoll_fd == -1)
		errorCloseEpollFd(epoll_fd, 7);

	Cookie cookie;
	this->configuringNetwork(itbeg, config, epoll_fd);

	struct epoll_event events[MAX_EVENTS];

	std::cout << "\nWaiting for connection...\n";

	while (true) {
		int num_fds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (num_fds == -1) 
			errorCloseEpollFd(epoll_fd, 1);
        if(num_fds == 0)
        {
            std::cout << BCYAN " TIMEOUT DETECTED" << RESET << std::endl; 
        }
		for (int i = 0; i < num_fds; ++i)
		{
			t_serverData *info = static_cast<t_serverData*>(events[i].data.ptr);
			int fd = info->sockfd;
			// std::cout << YELLOW "i " << i << " num " << num_fds << " poll fd " << events[i].data.fd << " fd " << fd << " status: " << events[i].events << RESET << std::endl; 
            if((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN)))
            {
                if(epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, &events[i]) == -1)
                {
                    std::cout << RED "Error epoll ctl catch: "<< errno << " " << strerror(errno) << RESET << std::endl;
					errorCloseEpollFd(epoll_fd, 4);
                }
                close(fd);
                std::cout << RED "client disconnection" << RESET << std::endl;
            }
            // check if my fd is equal to a socket for handcheck
			else if(this->socketfd.find(fd) != this->socketfd.end())
			{
				struct sockaddr_in client_addr;
				int client_fd = acceptConnection(fd, epoll_fd, client_addr);
				
				struct epoll_event client_event = this->fillEpoolDataInfo(client_fd, info);
				if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event) == -1)
                {
                    std::cout << RED "Error epoll ctl catch: "<< errno << " " << strerror(errno) << RESET << std::endl;
					errorCloseEpollFd(epoll_fd, 4);
                }
			}
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
                    else if(read_one_chunk(info, events[i], epoll_fd))
                    {
                        //if i finish read the request info i change the status of the socket
                        std::cout << BLUE "switching to epoolout" RESET << std::endl;
                        events[i].events = EPOLLOUT;
                        if(epoll_ctl(epoll_fd, EPOLL_CTL_MOD, info->sockfd, events) < 0)
                        {
                            std::cout << RED "Error epoll ctl catch: "<< errno << " " << strerror(errno) << RESET << std::endl;
                            errorCloseEpollFd(epoll_fd, 4);
                        }
                    }
                }
                // if i can write to my socket
                if(events[i].events & EPOLLOUT)
                {
                    try
                    {
                        // std::cout << BMAGENTA "Inside epollout" RESET << std::endl;
                        //if I have already a cgi and ready to return information
                        if(info->cgi)
                        {
                            std::string response = httpGetResponse("200 Ok", "text/html", info->body, info, "");
                            if(send(info->sockfd, response.c_str(), response.size(), 0) < 0)
                            {
                                std::cout << "error sending CGI response\n";
                                errorPage("500", info);
                            }
                            fdEpollLink.erase(info->sockfd);
                            close(info->cgi->cgifd);
                            delete info->cgi;
                            info->cgi = NULL;
                        }
                        // parse the data
                        else
                            proceed_response(info, cookie, fdEpollLink);
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
                std::cout << "-----------------------------------" << std::endl;
            }
		}
	}
}
