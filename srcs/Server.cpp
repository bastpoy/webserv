#include "Header.hpp"

/* ================ */
/*	CANONICAL FORMS	*/
/* ================ */

Server::Server()
{
	// std::cout << "creating a server configuration" << std::endl;
}

Server::~Server()
{
	// std::cout << "destroying a server configuration" << std::endl;
}

// Server::Server(const Server &other)
// {
// 	*this = other;
// }

/* ================ */
/*		SETTER		*/
/* ================ */

void Server::setPort(std::string port)
{
	this->_port = port;
}

void Server::setServerName(std::string server_name)
{
	this->_server_name = server_name;
}

void Server::setPath(std::string path)
{
	this->_path = path;
}

void Server::setMaxBody(std::string maxBody)
{
	this->_maxBody = maxBody;
}

void Server::setIndex(std::string index)
{
	this->_index = index;
}

void Server::setLocation(Location &location)
{
	this->_location.push_back(location);
}


void Server::setRedir(int code, std::string domain)
{
	this->_redir.insert(std::make_pair(code, domain));
}

void Server::setErrorPage(int code, std::string errorFile)
{
	this->_errorPage.insert(std::make_pair(code, errorFile));
}

void Server::setSocketFd(int sockfd)
{
	this->socketfd.insert(sockfd);
}

/* ================ */
/*		GETTER		*/
/* ================ */

std::string	Server::getPort() const
{
	return (this->_port);
}

std::string	Server::getServerName() const
{
	return (this->_server_name);
}

std::string	Server::getPath() const
{
	return (this->_path);
}

std::string	Server::getMaxBody() const
{
	return (this->_maxBody);
}

std::string	Server::getIndex() const
{
	return (this->_index);
}

std::map<int,std::string>	&Server::getRedir()
{
	return (this->_redir);
}

std::map<int,std::string>	&Server::getErrorPage()
{
	return (this->_errorPage);
}

std::vector<Location>	&Server::getLocation()
{
	return (this->_location);
}


/* ================ */
/*		FILL		*/
/* ================ */

void	Server::fillPort(std::string line)
{
	size_t pos = line.find("listen ");
	//get the port and convert to int
	this->setPort(line.substr(pos + strlen("listen "), line.length()).c_str());
	std::cout << "the port is: " << this->getPort() << std::endl;
}

void	Server::fillServerName(std::string line)
{
	size_t pos = line.find("server_name ");
	this->setServerName(line.substr(pos + strlen("server_name "), line.length() - (pos + strlen("server_name "))));
	//print
	std::cout << "the server is: " << this->getServerName() << std::endl;
}

void	Server::fillPath(std::string line)
{
	size_t pos = line.find("root ");
	this->setPath(line.substr(pos + strlen("root "), line.length() -(pos + strlen("root "))));
	//print
	std::cout << "the path is: " << this->getPath() << std::endl;
}

void	Server::fillMaxBody(std::string line)
{
	size_t pos = line.find("client_max_body_size ");
	this->setMaxBody(line.substr(pos + strlen("client_max_body_size "), line.length() - (pos + strlen("client_max_body_size "))));
	//print
	std::cout << "the maxBody is: " << this->getMaxBody() << std::endl;
}

void	Server::fillIndex(std::string line)
{
	size_t pos = line.find("index ");
	this->setIndex(line.substr(pos + strlen("index "), line.length() - (pos + strlen("index "))));
	//print
	std::cout << "the index is: " << this->getIndex() << std::endl;
}

void	Server::fillErrorPage(std::string line)
{
	size_t pos = line.find("error_page ");
	int code = atoi(line.substr(pos + strlen("error_page "), 3).c_str());
	std::string domain = line.substr(pos + strlen("error_page ") + 3, line.length());
	this->setErrorPage(code, domain);

	//print
	std::map<int, std::string>::iterator it = this->getErrorPage().begin();
	std::cout << "the errorCode is: " << it->first << "\t the file is: " << it->second <<  std::endl;
}

void	Server::fillRedir(std::string line)
{
	size_t pos = line.find("return ");
	int code = atoi(line.substr(pos + strlen("return "), 3).c_str());
	std::string domain = line.substr(pos + strlen("return ") + 3, line.length() - (pos + strlen("return ")));
	this->setRedir(code, domain);

	//print
	std::map<int, std::string>::iterator it = this->getRedir().begin();
	std::cout << "the code is: " << it->first << "\t the domain is: " << it->second <<  std::endl;
}

void	Server::fillLocation(std::ifstream &file, std::string line)
{
	Location location;
	std::cout << "\nNew location" << std::endl;

	//get the path of the location
	location.fillPath(line);
	//get all the information
	while(getline(file, line))
	{
		//fill the index of the file to serve
		if (line.find("autoindex ") != std::string::npos)
			location.fillAutoIndex(line);
		//fill the maxbody size
		else if (line.find("client_max_body_size ") != std::string::npos)
			location.fillMaxBody(line);
		//fill the autoindex
		else if (line.find("index ") != std::string::npos)
			location.fillIndex(line);
		//fill a redirection 
		else if (line.find("return ") != std::string::npos)
			location.fillRedir(line, this);
		//fill an error code
		else if (line.find("error_page ") != std::string::npos)
			location.fillErrorPage(line, this);
		//get the end of location section
		else if (line.find("}") != std::string::npos)
		{
			this->setLocation(location);
			return;
		}
		//if there is not all the above things return an error
		else
			throw Response::ConfigurationFileLocation();
	}
	//add my location to my server
}

template <typename T>
std::string to_string(T value) {
  std::ostringstream oss;
  oss << value;
  return oss.str();
}

std::string readFile(std::string &path)
{
	std::ifstream file(path.c_str(), std::ios::binary);
	if(!file.is_open())
	{
		std::cout << path << ": ";
		throw Response::ErrorOpeningFile();
	}
	return std::string(
		std::istreambuf_iterator<char>(file),
		std::istreambuf_iterator<char>()
	);
}
void error404(std::string &uri , std::string &contentType, int connection)
{
	std::string defaultPath = ".";
	std::string filePath = defaultPath + "/www/error/error404.html"; // Change this to your file path
	std::string content;

	std::cout << RED << uri << RESET << std::endl;
	//read the file content 
	content = readFile(filePath);
	//return the response
	std::cout << "the content type: " << contentType << std::endl;
	std::string response = "HTTP/1.1 404 Not found \r\n"
							// "Content-Type: text/html\r\n"
							"Content-Type: " + contentType + "\r\n"
							"Content-Length: " + to_string(content.size()) + "\r\n"
							"Connection: close\r\n"
							"\r\n" + content;
	if(send(connection, response.c_str(), response.size(), 0) < 0)
	{
		std::cout << strerror(errno) << std::endl;
		throw Response::ErrorSendingResponse(); 
	}
}

void getFileContent(std::string &uri , std::string &contentType, int connection)
{
	std::string defaultPath = ".";
	std::string filePath = defaultPath + uri; // Change this to your file path
	std::string content;

	//read the file content 
	content = readFile(filePath);
	//return the response
	std::cout << "the content type: " << contentType << std::endl;
	std::string response = "HTTP/1.1 200 OK \r\n"
							// "Content-Type: text/html\r\n"
							"Content-Type: " + contentType + "\r\n"
							"Content-Length: " + to_string(content.size()) + "\r\n"
							"Connection: close\r\n"
							"\r\n" + content;
	if(send(connection, response.c_str(), response.size(), 0) < 0)
	{
		std::cout << strerror(errno) << std::endl;
		throw Response::ErrorSendingResponse(); 
	}
}

// void getFileContent2(std::string &uri , std::string &contentType, int connection)
// {
// 	std::string	code;
// 	// uri = "." + uri; // Change this to your file path
// 	std::ifstream file(uri.c_str(), std::ios::binary);
// 	// if(!file.is_open())
// 	if(access(uri.c_str(), F_OK) != 0)
// 	{
// 		std::cout << uri << ": Fichier introuvable\n";
// 		code = "404 Not Found";
// 		uri = "/www/error/error404.html";
// 	}
// 	else if (access(uri.c_str(), R_OK) != 0)
// 	{
// 		std::cout << uri << ": Accès refusé (pas de droits de lecture)\n";
// 		code = "403 Forbidden";
// 		uri = "/www/error/error403.html";
// 	}
// 	else
// 		code = "200 OK";

// 	std::string filePath = "." + uri; // Change this to your file path

// 	std::string content;

// 	content = std::string(
// 		std::istreambuf_iterator<char>(file),
// 		std::istreambuf_iterator<char>()
// 	);

// 	std::cout << RED << uri << RESET << std::endl;

// 	// content = readFile(filePath);
// 	//return the response
// 	std::cout << "the content type: " << contentType << std::endl;
// 	std::string response = "HTTP/1.1 200 OK \r\n"
// 							// "Content-Type: text/html\r\n"
// 							"Content-Type: " + contentType + "\r\n"
// 							"Content-Length: " + to_string(content.size()) + "\r\n"
// 							"Connection: close\r\n"
// 							"\r\n" + content;
// 	if(send(connection, response.c_str(), response.size(), 0) < 0)
// 	{
// 		std::cout << strerror(errno) << std::endl;
// 		throw Response::ErrorSendingResponse(); 
// 	}
// }

void getFileContent2(std::string &uri , std::string &contentType, int connection)
{
	std::string	content;
	std::string	code = "200 OK";
	std::string	filePath = "." + uri;

	if(access(filePath.c_str(), F_OK) != 0)
	{
		std::cout << uri <<RED ": Fichier introuvable\n" RESET;
		code = "404 Not Found";
		uri = "/www/error/error404.html";
		contentType = "text/html";
	}
	else if (access(filePath.c_str(), R_OK) != 0)
	{
		std::cout << uri <<YELLOW ": Accès refusé (pas de droits de lecture)\n" RESET;
		code = "403 Forbidden";
		uri = "/www/error/error403.html";
		contentType = "text/html";
	}
	else
	{
		code = "200 OK";

	}
	filePath = "." + uri; // Change this to your file path
	
	// content = readFile(filePath);

	std::ifstream file(filePath.c_str(), std::ios::binary);
	if(!file.is_open())
	{
		std::cout << filePath << ": ";
		throw Response::ErrorOpeningFile();
	}
	content = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());


	std::cout << "contenu: " << content << std::endl;
	std::string response = "HTTP/1.1 " + code + " \r\n"
						// "Content-Type: text/html\r\n"
						"Content-Type: " + contentType + "\r\n"
						"Content-Length: " + to_string(content.size()) + "\r\n"
						"Connection: close\r\n"
						"\r\n" + content;
	if(send(connection, response.c_str(), response.size(), 0) < 0)
	{
		std::cout << strerror(errno) << std::endl;
		throw Response::ErrorSendingResponse(); 
	}
}

std::string getContentType(const std::string &path) 
{
	std::map<std::string, std::string> contentTypes;

	contentTypes.insert(std::pair<std::string, std::string>(".html", "text/html"));
	contentTypes.insert(std::pair<std::string, std::string>(".css", "text/css"));
	contentTypes.insert(std::pair<std::string, std::string>(".js", "application/javascript"));
	contentTypes.insert(std::pair<std::string, std::string>(".png", "image/png"));
	contentTypes.insert(std::pair<std::string, std::string>(".jpg", "image/jpeg"));
	contentTypes.insert(std::pair<std::string, std::string>(".gif", "image/gif"));

	size_t dotPos = path.find_last_of(".");
	if (dotPos != std::string::npos) {
		std::string extension = path.substr(dotPos);
		if (contentTypes.find(extension) != contentTypes.end()) {
		return contentTypes[extension];
		}
	}
	return "application/octet-stream"; // Default content type
}

// Fill from ServerAddr
void Server::createListenAddr(ConfigParser &config)
{
	std::vector<Server>::iterator itbeg = config.getServers().begin();
	// std::vector<Server> data(1, *itbeg);
	//creating the poll
	int epoll_fd = epoll_create(10);
	if (epoll_fd == -1) {
		std::cerr << "Failed to create epoll instance: " << strerror(errno) << std::endl;
		throw Response::Error();
	}

	while(itbeg != config.getServers().end())
	{
		//creation addrinfo struc to stock my addrinfo informations
		struct sockaddr_in addr;
		
		//I create my socket
		int sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd == -1)
		{
			std::cout << "Failed to create socket: " << strerror(errno) << std::endl;
			throw Response::ErrorCreatingSocket();
		}

		//add properties to allow the socket to be reusable even if it is in time wait
		int opt = 1;
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
			std::cerr << "Failed to set SO_REUSEADDR" << std::endl;
			throw Response::Error();
		}
		
		std::cout << itbeg->getPort() << "-" << itbeg->getServerName() << "-" << std::endl;

		//fill my sockaddr_in addr with the result of getaddrinfo
		addr.sin_family = AF_INET;
		addr.sin_port = htons(atoi(itbeg->getPort().c_str()));

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
			throw Response::ErrorListening();
		}
		t_serverData *data = new t_serverData;
		data->port = itbeg->getPort();
		data->server_name = itbeg->getServerName();
		data->path = itbeg->getPath();
		data->maxBody = itbeg->getMaxBody();
		data->index = itbeg->getIndex();
		data->errorPage = itbeg->getErrorPage();
		data->redir = itbeg->getRedir();
		data->location = itbeg->getLocation();

		struct epoll_event event;
		event.events = EPOLLIN; // Monitor for input events
		event.data.fd = sockfd;
		//I stock the info server on the event ptr data
		// event.data.ptr = static_cast<void*>(data);

		//add the epoll event to my epoll_fd instance
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &event) == -1) {
			std::cerr << "Failed to add socket to epoll: " << strerror(errno) << std::endl;
			throw Response::Error();
		}
		
		//add every socket to my set container
		this->setSocketFd(sockfd);
		itbeg++;
	}

	const int MAX_EVENTS = 20;
	struct epoll_event events[MAX_EVENTS];

	std::cout << "\nWaiting for connection...\n";

	while (true) {
		//epollwait return a number corresponding to all the files descriptor
		int num_fds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (num_fds == -1) {
			std::cerr << "epoll_wait error: " << strerror(errno) << std::endl;
			close(epoll_fd);
			throw Response::Error();
		}
		// I iterate through each fd
		for (int i = 0; i < num_fds; ++i) 
		{
			int fd = events[i].data.fd;
			// check if my fd is equal to a socket for handcheck
			if(this->socketfd.find(fd) != this->socketfd.end())
			{
				// listen to add new fd to my epoll structure
				struct sockaddr_in client_addr;
				socklen_t client_addr_len = sizeof(client_addr);
				
				//accept the connection
				int client_fd = accept(fd, (struct sockaddr*)&client_addr, &client_addr_len);
				if (client_fd == -1) {
					std::cerr << "accept() failed: " << strerror(errno) << std::endl;
					close(epoll_fd);
					throw Response::Error();                    
				}
				// add new fd to my epoll instance
				struct epoll_event client_event;
				client_event.events = EPOLLIN;
				client_event.data.fd = client_fd;
				// client_event.data.ptr = events[i].data.ptr;
				if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event) == -1)
				{
					std::cerr << "Failed to add socket to epoll: " << strerror(errno) << std::endl;
					close(epoll_fd);
					throw Response::Error();
				}
				std::cout << "\n0\n";
			}
			//Connection already etablish 
			else
			{
				// check for modification inside the socket
				if(events[i].events & EPOLLIN)
				{
					// t_serverData *info = static_cast<t_serverData*>(events[i].data.ptr);

					char buffer[1024];
					std::cout << "\nReading data...\n";
					ssize_t bytes_read = recv(fd, buffer, sizeof(buffer) - 1, 0);
					if (bytes_read == -1) {
						std::cerr << "recv() failed: " << strerror(errno) << std::endl;
						close(fd);
						continue;
					} else if (bytes_read == 0) {
						// Connection closed by the client
						std::cout << "Client disconnected: " << fd << std::endl;
						close(fd);
						continue;
					}

					// Null-terminate the buffer to make it a valid C-string
					buffer[bytes_read] = '\0';
					std::cout << "Received data from socket " << fd << ": " << buffer << std::endl;

					//formulate response
					std::string path = buffer;
					std::string content;
					std::string contentType;

					// get http request
					if(!path.empty())
					{
						//get uri
						path = path.substr(path.find('/'), path.size() - path.find('/'));
						path = path.substr(0, path.find(' '));
						std::cout << "the uri: " << path << std::endl;
						//get the type file
						contentType = getContentType(path);
						std::cout << "the content type is: " << contentType << std::endl;
					}
					else
						contentType = "text/html";

					// (void)info;
					
					// std::cout << "le pointeur vaut: " << events[i].data.ptr << std::endl; 
					// if(info) 
					//     std::cout << "return " << info->server_name << " " << info->port<< std::endl;

					//get the file content
					try
					{
						getFileContent2(path, contentType, fd);
					}
					catch(const std::exception& e)
					{
						std::cout << e.what() << std::endl;
					}
					std::cout << "\n\n";
					close(fd);
				}
			}
		}
	}
}

// Debug
void	Server::printConfig()
{
	std::vector<Location>::iterator	itbeg = this->_location.begin();
	std::vector<Location>::iterator	itend = this->_location.end();
	
	//print all server attributs
	if(!this->getPort().empty())
		std::cout << "listen " << this->getPort() << std::endl;
	if(!this->getServerName().empty())
		std::cout << "server_name " << this->getServerName() << std::endl;
	if(!this->getPath().empty())
		std::cout << "root " <<this->getPath() << std::endl;
	if(!this->getMaxBody().empty())
		std::cout << "client_max_body_size " << this->getMaxBody() << std::endl;
	if(!this->getIndex().empty())
		std::cout << "index " << this->getIndex() << std::endl;
	if(this->getErrorPage().begin()->first) 
		std::cout << "error_page " << this->getErrorPage().begin()->first << " " << this->getErrorPage().begin()->second << std::endl;
	if(this->getRedir().begin()->first) 
		std::cout << "return " << this->getRedir().begin()->first << " " << this->getRedir().begin()->second << std::endl;
	std::cout << std::endl;

	//print every location of my current server
	while(itbeg != itend)
	{
		(itbeg)->printConfig();
		itbeg++;
	}
}
