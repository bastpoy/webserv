#include "Header.hpp"

/* ================ */
/*	CANONICAL FORMS	*/
/* ================ */

Server::Server()
{
	// std::cout << GREEN "Creating a Server configuration" RESET << std::endl;
	this->_isfree = true;
}

Server::~Server()
{
	// std::cout << RED "Destroying a Server configuration" RESET << std::endl;
}

// Server::Server(const Server &other)
// {
// 	*this = other;
// }

// Server	&Server::operator=(const Server &other)
// {
// 	if (this == &other)
// 		return (*this);
// 	return (*this);
// }

/* ================ */
/*		SETTER		*/
/* ================ */

void Server::setPort(std::string port)
{
	port.erase(std::remove(port.begin(), port.end(), ' '), port.end());
	this->_port = port;
}

void Server::setServerName(std::string server_name)
{
	server_name.erase(std::remove(server_name.begin(), server_name.end(), ' '), server_name.end());
	this->_server_name = server_name;
}

void Server::setPath(std::string path)
{
	path.erase(std::remove(path.begin(), path.end(), ' '), path.end());
	this->_path = path;
}

void Server::setMaxBody(std::string maxBody)
{
	maxBody.erase(std::remove(maxBody.begin(), maxBody.end(), ' '), maxBody.end());
	this->_maxBody = maxBody;
}

void Server::setIndex(std::string index)
{
	index.erase(std::remove(index.begin(), index.end(), ' '), index.end());
	this->_index = index;
}

void Server::setLocation(Location &location)
{
	// location.erase(std::remove(location.begin(), location.end(), ' '), location.end());
	this->_location.push_back(location);
}


void Server::setRedir(int code, std::string domain)
{
	// code.erase(std::remove(code.begin(), code.end(), ' '), code.end());
	domain.erase(std::remove(domain.begin(), domain.end(), ' '), domain.end());
	this->_redir.insert(std::make_pair(code, domain));
}

void Server::setErrorPage(int code, std::string errorFile)
{
	errorFile.erase(std::remove(errorFile.begin(), errorFile.end(), ' '), errorFile.end());
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
	size_t pos = line.find("listen");
	//get the port and convert to int
	this->setPort(line.substr(pos + strlen("listen"), line.length()).c_str());
	// std::cout << "the port is:\t\t" YELLOW << this->getPort() << RESET << std::endl;
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
	size_t pos = line.find("server_name");
	this->setServerName(line.substr(pos + strlen("server_name"), line.length() - (pos + strlen("server_name"))));
	//print
	// std::cout << "the server is:\t\t" YELLOW << this->getServerName() << RESET << std::endl;
}

void	Server::fillPath(std::string line)
{
	size_t pos = line.find("root");
	this->setPath(line.substr(pos + strlen("root"), line.length() -(pos + strlen("root"))));
	//print
	// std::cout << "the path is:\t\t" YELLOW << this->getPath() << RESET << std::endl;
}

void	Server::fillMaxBody(std::string line)
{
	size_t pos = line.find("client_max_body_size");
	this->setMaxBody(line.substr(pos + strlen("client_max_body_size"), line.length() - (pos + strlen("client_max_body_size"))));
	//print
	// std::cout << "the maxBody is:\t\t" YELLOW << this->getMaxBody() << RESET << std::endl;
}

void	Server::fillIndex(std::string line)
{
	size_t pos = line.find("index");
	this->setIndex(line.substr(pos + strlen("index"), line.length() - (pos + strlen("index"))));
	//print
	// std::cout << "the index is:\t\t" YELLOW << this->getIndex() << RESET << std::endl;
}

void	Server::fillErrorPage(std::string line)
{
	size_t pos = line.find("error_page");
	int code = atoi(line.substr(pos + strlen("error_page"), 3).c_str());
	std::string domain = line.substr(pos + strlen("error_page") + 3, line.length());
	this->setErrorPage(code, domain);

	//print
	std::map<int, std::string>::iterator it = this->getErrorPage().begin();
	it->second.erase(std::remove(it->second.begin(), it->second.end(), ' '), it->second.end()); // erase spaces
	// std::cout << "the errorCode is:\t" YELLOW << it->first << RESET " (" YELLOW << it->second << RESET ")" <<  std::endl;
}

void	Server::fillRedir(std::string line)
{
	size_t pos = line.find("return");
	int code = atoi(line.substr(pos + strlen("return"), 3).c_str());
	std::string domain = line.substr(pos + strlen("return") + 3, line.length() - (pos + strlen("return")));
	this->setRedir(code, domain);

	//print
	std::map<int, std::string>::iterator it = this->getRedir().begin();
	// std::cout << "the code is:\t\t" YELLOW << it->first << RESET " (" YELLOW  << it->second << RESET ")" <<  std::endl;
}

/**
 * @brief	Gonna Fill all Location data info detected in a location instance
 * 			It will get the path of the location first, and then get all the information.
 * 			In last add my Location to my Server.
 * @note	It will erase spaces between key and value.
 * @author	Ozan, if you need to ask something...
*/
void	Server::fillLocation(std::ifstream &file, std::string line)
{
	// std::cout << GREEN "\nNew Location detected" RESET << std::endl;
	Location location;

	location.fillPath(line);
	while(getline(file, line))
	{
		if (line.find("{") != std::string::npos)
			continue ;
		else if (line.find("index") != std::string::npos)
			location.fillIndex(line);
		else if (line.find("autoindex") != std::string::npos)
			location.fillAutoIndex(line);
		else if (line.find("client_max_body_size") != std::string::npos)
			location.fillMaxBody(line);
		else if (line.find("return") != std::string::npos)
			location.fillRedir(line, this);
		else if (line.find("error_page") != std::string::npos)
			location.fillErrorPage(line, this);
		else if (line.find("}") != std::string::npos)
			return this->setLocation(location);
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
	int								i = 1;
	
	//print all server attributs
	if(!this->getPort().empty())
		std::cout << "listen\t\t" YELLOW << this->getPort() << RESET << std::endl;
	if(!this->getServerName().empty())
		std::cout << "server_name\t" YELLOW << this->getServerName() << RESET << std::endl;
	if(!this->getPath().empty())
		std::cout << "root\t\t" YELLOW <<this->getPath() << RESET << std::endl;
	if(!this->getMaxBody().empty())
		std::cout << "maxBody\t\t" YELLOW << this->getMaxBody() << RESET << std::endl;
	if(!this->getIndex().empty())
		std::cout << "index\t\t" YELLOW << this->getIndex() << RESET << std::endl;
	if(this->getErrorPage().begin()->first) 
		std::cout << "error_page\t" YELLOW << this->getErrorPage().begin()->first << " " << this->getErrorPage().begin()->second << RESET << std::endl;
	if(this->getRedir().begin()->first) 
		std::cout << "return\t\t" YELLOW << this->getRedir().begin()->first << " " << this->getRedir().begin()->second << RESET << std::endl;
	std::cout << std::endl;

	//print every location of my current server
	while(itbeg != itend)
	{
		std::cout << BWHITE "\tLocation "  << i++ << RESET << std::endl;
		(itbeg)->printConfig();
		itbeg++;
	}
}