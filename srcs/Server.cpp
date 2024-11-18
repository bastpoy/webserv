#include "Header.hpp"

//Fill data epoll with server iterator
struct epoll_event fillEpoolDataIterator(int sockfd, std::vector<Server>::iterator itbeg)
{
	t_serverData *data = new t_serverData;
	struct epoll_event event;
	
	data->sockfd = sockfd;
	data->port = itbeg->getPort();
	data->server_name = itbeg->getServerName();
	data->path = itbeg->getPath();
	data->maxBody = itbeg->getMaxBody();
	data->autoindex = itbeg->getAutoIndex();
	data->index = itbeg->getIndex();
	data->errorPage = itbeg->getErrorPage();
	data->redir = itbeg->getRedir();
	data->location = itbeg->getLocation();
    data->session = NULL;

	event.events = EPOLLIN | EPOLLOUT; // Monitor for input events
	//I stock the info server on the event ptr data
	event.data.ptr = static_cast<void*>(data);

	return (event);
}

//Fill data epoll with info from the new client
struct epoll_event fillEpoolDataInfo(int &client_fd, t_serverData *info)
{
	t_serverData *data = new t_serverData;

	data->sockfd = client_fd;
	data->port = info->port;
	data->server_name = info->server_name;
	data->path = info->path;
	data->maxBody = info->maxBody;
	data->autoindex = info->autoindex;
	data->index = info->index;
	data->errorPage = info->errorPage;
	data->redir = info->redir;
	data->location = info->location;
    data->session = NULL;

	struct epoll_event client_event;

	client_event.events = EPOLLIN | EPOLLOUT;
	client_event.data.ptr = static_cast<void*>(data);

	return(client_event);
}

//IP managing, Binding, Listening
void setupSocket(int &sockfd, struct sockaddr_in &addr, std::vector<Server>::iterator itbeg)
{
	//translate my string ip address into a network adress
	if(inet_pton(AF_INET, itbeg->getServerName().c_str(), &addr.sin_addr.s_addr) < 0)
	{
		std::cout << "wrong IP address: " << strerror(errno) << std::endl;
		throw Response::Error();
	}

	//bind my socket with the current fill sockaddr_in
	if (bind(sockfd, (struct sockaddr*)&addr, sizeof(sockaddr)) < 0) 
	{
		std::cout << "BIND: "<< strerror(errno) << " ";
		throw Response::Error();
	}

	//listen on the current socket created
	if (listen(sockfd, 10) < 0)
	{
		std::cout << strerror(errno) << " ";
		throw Response::Error();
	}
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
			errorCloseEpollFd(epoll_fd, 2);
        
        // int flag = fcntl(sockfd, F_GETFL, 0);
        // fcntl(sockfd, F_SETFL, flag | O_NONBLOCK);

        std::cout << sockfd << " and ";
		//add properties to allow the socket to be reusable even if it is in time wait
		int opt = 1;
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
			errorCloseEpollFd(epoll_fd, 3);
		
		// std::cout << itbeg->getPort() << "-" << itbeg->getServerName() << "-" << std::endl;

		//fill my sockaddr_in addr with the result of getaddrinfo
		addr.sin_family = AF_INET;
		addr.sin_port = htons(atoi(itbeg->getPort().c_str()));

		setupSocket(sockfd, addr, itbeg);

		//create a new instance of epool_event to stock info        
		struct epoll_event event = fillEpoolDataIterator(sockfd, itbeg);

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

//reading the request http data
std::string readingData(int &fd, int &epoll_fd, struct epoll_event *client_event)
{
	char buffer[1024];
	ssize_t bytes_read = recv(fd, buffer, sizeof(buffer) - 1, 0);
	//Error reading data
	if (bytes_read > 0)
    {
        buffer[bytes_read] = '\0';
	    return(buffer);
    }
	else if (bytes_read == 0) 
	{
		// Connection closed by the client
		std::cout << "Client disconnected: " << fd << std::endl;
        if(epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, client_event) == -1)
            errorCloseEpollFd(epoll_fd, 4);
		close(fd);
		return ("");
	}
    else if(bytes_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
    {
        std::cout << "Data not receive totally\n"; 
    }
    else
		errorCloseEpollFd(fd, 5);
    return(buffer);
}

bool handleRequest(std::string buffer, t_serverData *data) 
{
	std::string firstLine = buffer.substr(0, buffer.find("\n"));
	std::string typeRequest =  firstLine.substr(0, buffer.find(" "));

	if(typeRequest == "GET")
	{
		//if i have a redirection on the server or the location
		if(data->redir.size())
		{
            std::cout << "REDIRECTION GET" << std::endl;
			redirRequest(data->redir.begin()->second, data->sockfd);
		}
		// else I respond 
		else
		{
            parseAndGetRequest(buffer, data);
		}
	}
    //if it is a post request
	else if(typeRequest == "POST")
	{
		postRequest(buffer, data);
	}
    //if its a delete request
	else if(typeRequest == "DELETE")
	{
        parseAndDeleteRequest(buffer, data);
    }
	//if its not get, post or delete request
	else
		std::cout << "404 not found" << std::endl;
	return(false);
}

// Fill from ServerAddr
void Server::createListenAddr(ConfigParser &config)
{
	std::vector<Server>::iterator itbeg = config.getServers().begin();

	//creating the poll
	int epoll_fd = epoll_create(50);
	if (epoll_fd == -1)
		errorCloseEpollFd(epoll_fd, 7);

	//iterate through every server and retrieve information
	this->configuringNetwork(itbeg, config, epoll_fd);

	// EPOLL STARTING
	struct epoll_event events[MAX_EVENTS];

	std::cout << "\nWaiting for connection...\n";

	while (true) {
		//epollwait return a number corresponding to all the files descriptor
        std::cout << "waiting...\n";
		int num_fds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (num_fds == -1) 
			errorCloseEpollFd(epoll_fd, 1);
		// I iterate through each fd
		for (int i = 0; i < num_fds; ++i) 
		{
			t_serverData *info = static_cast<t_serverData*>(events[i].data.ptr);
			int fd = info->sockfd;

			// check if my fd is equal to a socket for handcheck
			if(this->socketfd.find(fd) != this->socketfd.end())
			{
				// listen to add new fd to my epoll structure
				struct sockaddr_in client_addr;
				//new fd_client for communication
				int client_fd = acceptConnection(fd, epoll_fd, client_addr);
				
                // add new fd to my epoll instance
				struct epoll_event client_event = fillEpoolDataInfo(client_fd, info);
				// add the new fd to be control by my epoll instance
				if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event) == -1)
					errorCloseEpollFd(epoll_fd, 4);
				std::cout << "New connection established with new fd: " << client_fd << std::endl;
			}
			//Connection already etablish 
			else
			{
				// check for modification inside the socket
				if(events[i].events & (EPOLLIN | EPOLLOUT))
				{
					try
					{
						//reading data
						std::string path = readingData(fd, epoll_fd, events);
						if (path.empty())
							continue;
                        // std::cout << path << std::endl;
						// response request
						if(handleRequest(path, info))
							continue;
					}
					catch(const std::exception& e)
					{
						std::cerr << e.what() << '\n';
					}
                    std::cout << "\n\n";
				}
			}
		}
	}
}
