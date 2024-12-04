#include "Header.hpp"

//TODO - ENUM
std::string	locationKeywords[] = {
	"root",
	"client_max_body_size",
	"autoindex",
	"index",
	"return",
	"error_page",
	"cgi_path"
};

//TODO - Trouver une solution plus propre
void (Location::*locationFunctions[7])(std::string line) = {
	&Location::fillRoot,
	&Location::fillMaxBody,
	&Location::fillAutoIndex,
	&Location::fillIndex,
	&Location::fillRedir,
	&Location::fillErrorPage,
	&Location::fillCgiPath
};

const int	locKeywordsSize = 7;


/* ================ */
/*		SETTER		*/
/* ================ */

// void	Server::setPort(std::string port)
// {
// 	// // size_t	pos = line.find("listen");
// 	// int		int_port = 0;
// 	// // setPort(line.substr(pos + strlen("listen"), line.length()).c_str());
// 	// port.erase(std::remove(port.begin(), port.end(), ' '), port.end());
// 	// _port = port;
// 	// if (getPort().empty())
// 	// 	throw Response::ConfigurationFileServer("Port is empty");
// 	// if (!ft_stoi(getPort().c_str(), int_port))
// 	// 	throw Response::ConfigurationFileServer("Port is not a number");
// 	// if (int_port < 0 || int_port > 65535)
// 	// 	throw Response::ConfigurationFileServer("Port out of range");
// 	(void)port;
// }

void	Server::setIP(std::string ip)
{
	ip.erase(std::remove(ip.begin(), ip.end(), ' '), ip.end());
	ip.erase(std::remove(ip.begin(), ip.end(), ':'), ip.end());
	std::vector<std::string>	substr_ip = ft_split(ip, '.');

	if (substr_ip.size() != 4
		|| ip.find_first_not_of("0123456789.") != std::string::npos)
		throw Response::ConfigurationFileServer("Wrong IP format");
	for (size_t i = 0; i < substr_ip.size(); i++)
	{
		int result = 0;
		if (!ft_stoi(substr_ip[i], result))
			break ;
		if (!(result >= 0 && result <= 255))
			throw Response::ConfigurationFileServer("IP out of range");
	}
	_ip = ip;
}

void	Server::setListen(std::string line)
{
	std::string	port;
	int			int_port = 0;
	size_t		pos = line.find("listen");
	size_t		pos2 = line.find(":");
	size_t		len = strlen("listen");

	line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
	port = line.substr((pos2 != std::string::npos) ? pos2 : pos + len, line.length());
	if (pos2 != std::string::npos)
		setIP(line.substr(pos + len, pos2 - (pos + len)));
	if (port.empty())
		throw Response::ConfigurationFileServer("Port is empty");
	if (!ft_stoi(port.c_str(), int_port))
		throw Response::ConfigurationFileServer("Port is not a number");
	if (int_port < 0 || int_port > 65535)
		throw Response::ConfigurationFileServer("Port out of range");
	_port = port;
}

void	Server::setServerName(std::string line)
{
	size_t		pos = line.find("server_name");
	size_t		len = strlen("server_name");
	std::string	server_name = line.substr(pos + len, line.length() - (pos + len));
	server_name.erase(std::remove(server_name.begin(), server_name.end(), ' '), server_name.end());
	bool	is_ip = server_name.find_first_of("0123456789") != std::string::npos;
	_server_name = server_name;

	if (is_ip)
	{
		std::vector<std::string>	substr_ip = ft_split(_server_name, '.');
		if (!_ip.empty())
			throw Response::ConfigurationFileServer("Server name is an IP and IP is already set");
		if (substr_ip.size() != 4
			|| _server_name.find_first_not_of("0123456789.") != std::string::npos)
			throw Response::ConfigurationFileServer("Wrong IP format");
		for (size_t i = 0; i < substr_ip.size(); i++)
		{
			int result = 0;
			if (!ft_stoi(substr_ip[i], result))
				break ;
			if (!(result >= 0 && result <= 255))
				throw Response::ConfigurationFileServer("IP out of range");
		}
		return ;
	}
}

void	Server::setRoot(std::string line)
{
	size_t		pos = line.find("root");
	size_t		len = strlen("root");
	std::string	path = line.substr(pos + len, line.length() - (pos + len));

	path.erase(std::remove(path.begin(), path.end(), ' '), path.end());
	if(path.at(path.size() - 1) != '/')
		path = path + "/";
	if(path.at(0) != '.')
		path = "." + path;
	_path = path;
}

void	Server::setMaxBody(std::string line)
{
	size_t		pos = line.find("client_max_body_size");
	size_t		len = strlen("client_max_body_size");
	std::string	maxBody = line.substr(pos + len, line.length() - (pos + len));

	maxBody.erase(std::remove(maxBody.begin(), maxBody.end(), ' '), maxBody.end());
	if(maxBody.find_first_not_of("0123456789kmKM") != std::string::npos)
		throw Response::ErrorMaxBody();
	// Replace the k and m by real number and check for errors
	// maxBodyParsing("k", maxBody);
	// maxBodyParsing("K", maxBody);
	// maxBodyParsing("m", maxBody);
	// maxBodyParsing("M", maxBody);
	size_t pos_character = maxBody.find_first_of("kKmM");
	if (pos_character != std::string::npos)
	{
		std::string caracter = maxBody.substr(pos_character, 1);
		if(caracter == "k" || caracter == "K")
			maxBody.replace(pos_character, 1, "000");
		else
			maxBody.replace(pos_character, 1, "000000");
	}
	_maxBody = maxBody;
}

void	Server::setIndex(std::string line)
{
	size_t		pos = line.find("index");
	size_t		len = strlen("index");
	std::string	index = line.substr(pos + len, line.length() - (pos + len));

	index.erase(std::remove(index.begin(), index.end(), ' '), index.end());
	_index = index;
}

void	Server::setAutoIndex(std::string line)
{
	size_t		pos = line.find("autoindex"); 
	size_t		len = strlen("autoindex");
	std::string	autoindex = line.substr(pos + len, line.length() - (pos + len));

	autoindex.erase(std::remove(autoindex.begin(), autoindex.end(), ' '), autoindex.end());
	_autoindex = autoindex;
}

void	Server::setLocation(Location &location)
{
	_location.push_back(location);
}

void	Server::setRedir(std::string line)
{
	size_t		pos = line.find("return");
	size_t		len = strlen("return");
	std::string	code = line.substr(pos + len, 4);
	std::string	domain = line.substr(pos + len + 4, line.length() - (pos + len));

	domain.erase(std::remove(domain.begin(), domain.end(), ' '), domain.end());
	code.erase(std::remove(code.begin(), code.end(), ' '), code.end());
	// If i have already a redirection i delete it and replace it
	if(getRedir().size())
		_redir.erase(this->_redir.begin());
	//add the new redirection
	_redir.insert(std::make_pair(code, domain));
}

void Server::setErrorPage(std::string line)
{
	line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
	size_t pos = line.find("error_page");
	size_t len = strlen("error_page");
	std::string code = line.substr(pos + len, 3).c_str();
	std::string errorFile = line.substr(pos + len + 4, line.length());

	_errorPage.insert(std::make_pair(code, errorFile));
}

void Server::setCgiPath(std::string line)
{
	size_t		pos = line.find("cgi_path ");
	size_t		len = strlen("cgi_path ");
	std::string	trimLine = line.substr(pos + len, line.length() - (pos + len));
	std::vector<std::string>	substr = ft_split(trimLine, ' ');

	for (size_t i = 0; i < substr.size(); i++)
	{
		std::vector<std::string>	map = ft_split(substr[i], ':');
		std::string					language = map[0];
		std::string					path = map[1];
		std::cout << MAGENTA"Language: '" << language << "'\nPath: '" << path << "'" << RESET << std::endl;
		_cgiPath.insert(std::make_pair(language, path));
	}
}

void Server::setAllowedMethods(std::string line)
{
	size_t pos = line.find("allowed_methods");
	size_t len = strlen("allowed_methods");
	std::string methods = line.substr(pos + len, line.length() - (pos + len));

	methods.erase(std::remove(methods.begin(), methods.end(), ' '), methods.end());
	if (methods.find_first_not_of("GETPOSTDELETE") != std::string::npos)
		throw Response::ConfigurationFileServer("Allowed methods are not GET, POST or DELETE");
	if (methods.find("GET") != std::string::npos)
		_allowedMethods.push_back("GET");
	if (methods.find("POST") != std::string::npos)
		_allowedMethods.push_back("POST");
	if (methods.find("DELETE") != std::string::npos)
		_allowedMethods.push_back("DELETE");
}

void	Server::setSocketFd(int sockfd)
{
	socketfd.insert(sockfd);
}

/* ================ */
/*		GETTER		*/
/* ================ */

std::string	Server::getPort() const { return (_port); }

std::string	Server::getIP() const { return (_ip); }

std::string	Server::getServerName() const { return (_server_name); }

std::string	Server::getPath() const { return (_path); }

std::string	Server::getMaxBody() const { return (_maxBody); }

std::string	Server::getIndex() const { return (_index); }

std::string	Server::getAutoIndex() const { return (_autoindex); }

std::map<std::string,std::string>	&Server::getRedir() { return (_redir); }

std::map<std::string,std::string>	&Server::getErrorPage() { return (this->_errorPage); }

std::map<std::string,std::string>	&Server::getCgiPath() { return (this->_cgiPath); }

std::vector<std::string>			&Server::getAllowedMethods() { return (_allowedMethods); }

std::vector<Location>	&Server::getLocation() { return (_location); }

/* ================ */
/*		FILL		*/
/* ================ */

// void	Server::fillPort(std::string line)
// {
// 	size_t	pos = line.find("listen");
// 	int		port = 0;
// 	setPort(line.substr(pos + strlen("listen"), line.length()).c_str());
// 	if (getPort().empty())
// 		throw Response::ConfigurationFileServer("Port is empty");
// 	if (!ft_stoi(getPort().c_str(), port))
// 		throw Response::ConfigurationFileServer("Port is not a number");
// 	if (port < 0 || port > 65535)
// 		throw Response::ConfigurationFileServer("Port out of range");
// }

// void Server::fillListen(std::string line)
// {
// 	size_t	pos = line.find("listen");
// 	size_t	pos2 = line.find(":");
// 	size_t	len = strlen("listen");
// 	if (pos2 != std::string::npos)
// 	{
// 		// fillPort(line.substr(pos2 + 1, line.length()));
// 		setPort(line.substr(pos2 + 1, line.length()));
// 		setIP(line.substr(pos + len, pos2 - (pos + len)));
// 	}
// 	else
// 		throw Response::ConfigurationFileServer("Missing ':' in listen block");
// }

// void	Server::fillServerName(std::string line)
// {
// 	size_t	pos = line.find("server_name");
// 	setServerName(line.substr(pos + strlen("server_name"), line.length() - (pos + strlen("server_name"))));
// 	bool	is_ip = _server_name.find_first_of("0123456789") != std::string::npos;
// 	if (is_ip)
// 	{
// 		std::vector<std::string>	substr_ip = ft_split(_server_name, '.');
// 		if (substr_ip.size() != 4
// 			|| _server_name.find_first_not_of("0123456789.") != std::string::npos)
// 			throw Response::ConfigurationFileServer("Wrong IP format");
// 		for (size_t i = 0; i < substr_ip.size(); i++)
// 		{
// 			int result = 0;
// 			if (!ft_stoi(substr_ip[i], result))
// 				break ;
// 			if (!(result >= 0 && result <= 255))
// 				throw Response::ConfigurationFileServer("IP out of range");
// 		}
// 		return ;
// 	}
// 	(void)line;
// }

// void	Server::fillPath(std::string line)
// {
	// size_t pos = line.find("root");
	// setPath(line.substr(pos + strlen("root"), line.length() - (pos + strlen("root"))));
	// //if no "/" at the end add it
	// if(getPath().at(getPath().size() - 1) != '/')
	// 	setPath(getPath() + "/");
	// //if no . at the begining add it
	// if(getPath().at(0) != '.')
	// 	setPath("." + getPath());
	// (void)line;
// }

// void	Server::fillMaxBody(std::string line)
// {
	// size_t pos = line.find("client_max_body_size");
	// std::string size = line.substr(pos + strlen("client_max_body_size"), line.length() - (pos + strlen("client_max_body_size")));
	// // Erase spaces
	// size.erase(std::remove(size.begin(), size.end(), ' '), size.end());
	// // Check autorize caracter
	// if(size.find_first_not_of("0123456789kmKM") != std::string::npos)
	// 	throw Response::ErrorMaxBody();
	// // Replace the k and m by real number and check for errors
	// maxBodyParsing("k", size);
	// maxBodyParsing("K", size);
	// maxBodyParsing("m", size);
	// maxBodyParsing("M", size);
	// setMaxBody(size);
	// (void)line;
// }

// void	Server::fillIndex(std::string line)
// {
// 	// size_t pos = line.find("index");
// 	// setIndex(line.substr(pos + strlen("index"), line.length() - (pos + strlen("index"))));
// 	(void)line;
// }

// void	Server::fillAutoIndex(std::string line)
// {
// 	// size_t pos = line.find("autoindex"); 
// 	// this->setAutoIndex(line.substr(pos + strlen("autoindex"), line.length() - (pos + strlen("autoindex"))));
// 	//print
// 	// std::cout << "the autoindex is: " << this->getAutoIndex() << std::endl;
// 	(void)line;
// }

// void	Server::fillErrorPage(std::string line)
// {
// 	size_t pos = line.find("error_page ");
// 	// line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
// 	// std::cout << line << std::endl;
// 	std::string code = line.substr(pos + strlen("error_page "), 3).c_str();
// 	std::string domain = line.substr(pos + strlen("error_page ") + 4, line.length());
// 	this->setErrorPage(code, domain);
// 	print
// 	std::map<int, std::string>::iterator it = this->getErrorPage().begin();
// 	std::cout << "the errorCode is: " << it->first << "\t the file is: " << it->second <<  std::endl;
// 	(void)line;
// }

// void	Server::fillCgiPath(std::string line)
// {
// 	size_t	pos = line.find("cgi_path ");
// 	std::string trimLine = line.substr(pos + strlen("cgi_path "), line.length() - (pos + strlen("cgi_path ")));
// 	std::vector<std::string>	substr = ft_split(trimLine, ' ');

// 	for (size_t i = 0; i < substr.size(); i++)
// 	{
// 		std::vector<std::string>	map = ft_split(substr[i], ':');
// 		std::string language = map[0];
// 		std::string path = map[1];
// 		std::cout << MAGENTA"Language: '" << language << "'\nPath: '" << path << "'" << RESET << std::endl;
// 		this->setCgiPath(language, path);
// 	}
// }

// void	Server::fillRedir(std::string line)
// {
// 	// size_t pos = line.find("return");
// 	// std::string code = line.substr(pos + strlen("return"), 4);
// 	// std::string domain = line.substr(pos + strlen("return") + 4, line.length() - (pos + strlen("return")));
// 	// setRedir(code, domain);
// 	(void)line;
// }

/**
 * @brief	Gonna Fill all Location data info detected in a location instance
 * 			It will get the path of the location first, and then get all the information.
 * 			In last add my Location to my Server.
 * @note	It will erase spaces between key and value.
 * @author	Ozan, if you need to ask something...
*/
void	Server::fillLocation(std::ifstream &file, std::string line, std::vector<Location> &locations)
{
	Location	location;
	bool		bracket = false;

	location.fillPath(line);
	checkLocationPath(location, locations);
	while(getline(file, line))
	{
		int i = 0;
		ConfigParser::parseLine(line); //TODO - Retoucher et mettre dans utils
		if (line.find("{") != std::string::npos)
		{
			bracket = true;
			continue;
		}
		while (i < locKeywordsSize && line.find(locationKeywords[i]) == std::string::npos)
			i++;
		if (i < locKeywordsSize)
			(location.*locationFunctions[i])(line);
		else if (i > (locKeywordsSize - 1) && line.find("}") == std::string::npos && !line.empty())
			throw Response::ConfigurationFileLocation("Unknown attribute: " + line);
		if (line.find("}") != std::string::npos)
		{
			if (bracket == false)
				throw Response::ConfigurationFileLocation("Missing '{' in location block");
			location.checkNotEmptys();
			return setLocation(location);
		}
	}
	throw Response::ConfigurationFileLocation("Missing '}'");

}

/* ================ */
/*		DEBUG		*/
/* ================ */

void	Server::printConfig()
{
	std::vector<Location>::iterator	itbeg = _location.begin();
	std::vector<Location>::iterator	itend = _location.end();
	
	//print all server attributs
	if (!getPort().empty())
		std::cout << "listen\t\t" YELLOW << getPort() << RESET << std::endl;
	if (!getIP().empty())
		std::cout << "ip\t\t" YELLOW << getIP() << RESET << std::endl;
	if (!getServerName().empty())
		std::cout << "server_name\t" YELLOW << getServerName() << RESET << std::endl;
	if (!getPath().empty())
		std::cout << "root\t\t" YELLOW <<getPath() << RESET << std::endl;
	if (!getMaxBody().empty())
		std::cout << "maxBody\t\t" YELLOW << getMaxBody() << RESET << std::endl;
	if (!getIndex().empty())
		std::cout << "index\t\t" YELLOW << getIndex() << RESET << std::endl;
	if (!getAutoIndex().empty())
		std::cout << "autoindex\t" YELLOW << getAutoIndex() << RESET << std::endl;
	if (getErrorPage().size()) 
		std::cout << "error_page\t" YELLOW << getErrorPage().begin()->first << " " << getErrorPage().begin()->second << RESET << std::endl;
	if (getCgiPath().size())
	{
		std::map<std::string, std::string>::iterator it = getCgiPath().begin();
		std::cout << "cgi_path\t" YELLOW;
		for (; it != getCgiPath().end(); it++)
			std::cout << it->first << ":" << it->second << " ";
		std::cout << RESET << std::endl;
	}
	if (getRedir().size()) 
		std::cout << "return\t\t" YELLOW << getRedir().begin()->first << " " << getRedir().begin()->second << RESET << std::endl;
	if (_allowedMethods.size())
	{
		std::cout << "allowed_methods\t";
		for (size_t i = 0; i < _allowedMethods.size(); i++)
			std::cout << YELLOW << _allowedMethods[i] << " " << RESET;
		std::cout << std::endl;
	}
	std::cout << std::endl;
	//print every location of my current server
	for (int i = 1; itbeg != itend; ++itbeg, ++i)
		std::cout << BWHITE "\tLocation " << i << RESET << std::endl, (itbeg)->printConfig();

}

