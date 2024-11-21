#include "Header.hpp"

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
	// maxBody.erase(std::remove(maxBody.begin(), maxBody.end(), ' '), maxBody.end());
	this->_maxBody = maxBody;
}

void Server::setIndex(std::string index)
{
	index.erase(std::remove(index.begin(), index.end(), ' '), index.end());
	this->_index = index;
}

void Server::setAutoIndex(std::string autoindex)
{
	autoindex.erase(std::remove(autoindex.begin(), autoindex.end(), ' '), autoindex.end());
	this->_autoindex = autoindex;
}

void Server::setLocation(Location &location)
{
	// location.erase(std::remove(location.begin(), location.end(), ' '), location.end());
	this->_location.push_back(location);
}

void Server::setRedir(std::string code, std::string domain)
{
	// code.erase(std::remove(code.begin(), code.end(), ' '), code.end());
	domain.erase(std::remove(domain.begin(), domain.end(), ' '), domain.end());
	code.erase(std::remove(code.begin(), code.end(), ' '), code.end());
	this->_redir.insert(std::make_pair(code, domain));
}

void Server::setErrorPage(int code, std::string errorFile)
{
	errorFile.erase(std::remove(errorFile.begin(), errorFile.end(), ' '), errorFile.end());
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

std::string	Server::getAutoIndex() const
{
	return (this->_autoindex);
}

std::map<std::string,std::string>	&Server::getRedir()
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
	size_t pos = line.find("listen");
	//get the port and convert to int
	this->setPort(line.substr(pos + strlen("listen"), line.length()).c_str());
	// std::cout << "the port is: " << this->getPort() << std::endl;
}

void	Server::fillServerName(std::string line)
{
	size_t pos = line.find("server_name");
	this->setServerName(line.substr(pos + strlen("server_name"), line.length() - (pos + strlen("server_name"))));
	//print
	// std::cout << "the server is: " << this->getServerName() << std::endl;
}

void	Server::fillPath(std::string line)
{
	size_t pos = line.find("root");
	this->setPath(line.substr(pos + strlen("root"), line.length() - (pos + strlen("root"))));
	//if no "/" at the end add it
	if(this->getPath().at(this->getPath().size() - 1) != '/')
		this->setPath(this->getPath() + "/");
	//if no . at the begining add it
	if(this->getPath().at(0) != '.')
		this->setPath("." + this->getPath());
	//print
	// std::cout << "the path is: " << this->getPath() << std::endl;
}

void	Server::fillMaxBody(std::string line)
{
	size_t pos = line.find("client_max_body_size");
	std::string size = line.substr(pos + strlen("client_max_body_size"), line.length() - (pos + strlen("client_max_body_size")));
	// Erase spaces
	size.erase(std::remove(size.begin(), size.end(), ' '), size.end());
	// Check autorize caracter
	if(size.find_first_not_of("0123456789kmKM") != std::string::npos)
		throw Response::ErrorMaxBody();
	// Replace the k and m by real number and check for errors
	maxBodyParsing("k", size);
	maxBodyParsing("K", size);
	maxBodyParsing("m", size);
	maxBodyParsing("M", size);

	this->setMaxBody(size);
}

void	Server::fillIndex(std::string line)
{
	size_t pos = line.find("index");
	this->setIndex(line.substr(pos + strlen("index"), line.length() - (pos + strlen("index"))));
	//print
	// std::cout << "the index is: " << this->getIndex() << std::endl;
}

//TODO - Delete spaces
void	Server::fillAutoIndex(std::string line)
{
	size_t pos = line.find("autoindex"); 
	this->setAutoIndex(line.substr(pos + strlen("autoindex"), line.length() - (pos + strlen("autoindex"))));
	//print
	// std::cout << "the autoindex is: " << this->getAutoIndex() << std::endl;
}

void	Server::fillErrorPage(std::string line)
{
	size_t pos = line.find("error_page");
	line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
	int code = atoi(line.substr(pos + strlen("error_page"), 3).c_str());
	std::string domain = line.substr(pos + strlen("error_page") + 3, line.length());
	this->setErrorPage(code, domain);

	//print
	// std::map<int, std::string>::iterator it = this->getErrorPage().begin();
	// std::cout << "the errorCode is: " << it->first << "\t the file is: " << it->second <<  std::endl;
}

void	Server::fillRedir(std::string line)
{
	size_t pos = line.find("return");
	std::string code = line.substr(pos + strlen("return"), 4);
	std::string domain = line.substr(pos + strlen("return") + 4, line.length() - (pos + strlen("return")));
	this->setRedir(code, domain);

	//print
	// std::map<std::string, std::string>::iterator it = this->getRedir().begin();
	// std::cout << "the code is: " << it->first << "\t the domain is: " << it->second <<  std::endl;
}

/**
 * @brief	Gonna Fill all Location data info detected in a location instance
 * 			It will get the path of the location first, and then get all the information.
 * 			In last add my Location to my Server.
 * @note	It will erase spaces between key and value.
 * @author	Ozan, if you need to ask something...
*/
void	Server::fillLocation(std::ifstream &file, std::string line, std::vector<Location> &locations)
{
	// std::cout << GREEN "\nNew Location detected" RESET << std::endl;
	Location location;

	location.fillPath(line, locations);
	while(getline(file, line))
	{
		if (line.find("{") != std::string::npos)
			continue ;
		if (line.find("autoindex") != std::string::npos)
			location.fillAutoIndex(line);
		else if (line.find("client_max_body_size") != std::string::npos)
			location.fillMaxBody(line);
		else if (line.find("root") != std::string::npos)
			location.fillRoot(line);
		else if (line.find("index") != std::string::npos)
			location.fillIndex(line);
		else if (line.find("return") != std::string::npos)
			location.fillRedir(line, this);
		else if (line.find("error_page") != std::string::npos)
			location.fillErrorPage(line, this);
		else if (line.find("}") != std::string::npos)
		{
			this->setLocation(location);
			return;
		}
		else
			throw Response::ConfigurationFileLocation();
	}
	//add my location to my server
}

/* ================ */
/*		DEBUG		*/
/* ================ */

void	Server::printConfig()
{
	std::vector<Location>::iterator	itbeg = this->_location.begin();
	std::vector<Location>::iterator	itend = this->_location.end();
	int								i = 1;
	
	//print all server attributs
	if (!this->getPort().empty())
		std::cout << "listen\t\t" YELLOW << this->getPort() << RESET << std::endl;
	if (!this->getServerName().empty())
		std::cout << "server_name\t" YELLOW << this->getServerName() << RESET << std::endl;
	if (!this->getPath().empty())
		std::cout << "root\t\t" YELLOW <<this->getPath() << RESET << std::endl;
	if (!this->getMaxBody().empty())
		std::cout << "maxBody\t\t" YELLOW << this->getMaxBody() << RESET << std::endl;
	if (!this->getIndex().empty())
		std::cout << "index\t\t" YELLOW << this->getIndex() << RESET << std::endl;
	if (!this->getAutoIndex().empty())
		std::cout << "autoindex\t" YELLOW << this->getAutoIndex() << RESET << std::endl;
	if (this->getErrorPage().begin()->first) 
		std::cout << "error_page\t" YELLOW << this->getErrorPage().begin()->first << " " << this->getErrorPage().begin()->second << RESET << std::endl;
	if (this->getRedir().size()) 
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

void	maxBodyParsing(std::string caracter, std::string &size)
{
	size_t pos;
	
	if(size.find(caracter) != std::string::npos)
	{
		pos = size.find(caracter);
		//compare the position of the actual caracter
		if(size.size() - 1 != pos)
			throw Response::ErrorMaxBody();
		if(caracter == "k" || caracter == "K")
			size.replace(pos, 1, "000");
		else
			size.replace(pos, 1, "000000");
	}
}