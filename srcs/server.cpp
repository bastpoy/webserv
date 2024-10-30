#include "Header.hpp"

/* ================ */
/*	CANONICAL FORMS	*/
/* ================ */

Server::Server()
{
	std::cout << GREEN "Creating a Server configuration" RESET << std::endl;
	this->_isfree = true;
}

Server::~Server()
{
	std::cout << RED "destroying a server configuration" RESET << std::endl;
}

Server::Server(const Server &other)
{
	*this = other;
}

Server	&Server::operator=(const Server &other)
{
	if (this == &other)
		return (*this);
	return (*this);
}

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

// Setter from ServerAddr
void Server::setListenAddr(struct sockaddr_in addr)
{
	this->getListenAddr().push_back(addr);
}

void Server::setSocketFd(int sockfd)
{
	this->_sockfd = sockfd;
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

//getter from ServerAddr
std::vector<struct sockaddr_in> &Server::getListenAddr()
{
	return(this->_listenAddr);
}

int Server::getSocketFd()const 
{
	return (this->_sockfd);
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

// void	Server::fillPort(std::string line)
// {
// 	size_t pos = line.find("listen");
// 	//get the port and convert to int
// 	str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
// 	// this->setPort(line.substr(pos + strlen("listen "), line.length()).c_str());
// 	// std::cout << "the port is: " << this->getPort() << std::endl;
// }

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

// Fill from ServerAddr
void Server::createListenAddr(ConfigParser &config)
{
	std::vector<Server>::iterator itbeg = config.getServers().begin();

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


		std::string ip = itbeg->getServerName();
		std::string port = itbeg->getPort();
		std::cout << port << "-" << ip << "-" << std::endl;

		//fill my sockaddr_in addr with the result of getaddrinfo
		addr.sin_family = AF_INET;
		addr.sin_port = htons(atoi(itbeg->getPort().c_str()));

		if(inet_pton(AF_INET, itbeg->getServerName().c_str(), &addr.sin_addr.s_addr) < 0)
		{
			std::cout << "wrong IP address: " << strerror(errno) << std::endl;
			return;
		}

		// this->setListenAddr(addr);
		// //fill my class with the sockfd
		// this->setSockfd(sockfd);
		//bind my socket with the current fill sockaddr_in
		if (bind(sockfd, (struct sockaddr*)&addr, sizeof(sockaddr)) < 0) 
		{
			std::cout << "BIND: "<< strerror(errno) << " ";
			throw Response::ErrorBindAddress();
		}

		//listen on the current socket created
		if (listen(sockfd, 10) < 0)
		{
			std::cout << strerror(errno) << " ";
			throw Response::ErrorListening();
		}
		itbeg++;
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