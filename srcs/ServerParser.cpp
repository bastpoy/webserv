#include "Header.hpp"

std::string	locationKeywords[] = {
	"root",
	"client_max_body_size",
	"autoindex",
	"index",
	"return",
	"error_page"
};

void (Location::*locationFunctions[6])(std::string line) = {
	&Location::fillRoot,
	&Location::fillMaxBody,
	&Location::fillAutoIndex,
	&Location::fillIndex,
	&Location::fillRedir,
	&Location::fillErrorPage
};

const int	locKeywordsSize = 6;


/* ================ */
/*		SETTER		*/
/* ================ */

void	Server::setPort(std::string port)
{
	port.erase(std::remove(port.begin(), port.end(), ' '), port.end());
	_port = port;
}

void	Server::setServerName(std::string server_name)
{
	server_name.erase(std::remove(server_name.begin(), server_name.end(), ' '), server_name.end());
	_server_name = server_name;
}

void	Server::setPath(std::string path)
{
	path.erase(std::remove(path.begin(), path.end(), ' '), path.end());
	_path = path;
}


void	Server::setMaxBody(std::string maxBody)
{
	// maxBody.erase(std::remove(maxBody.begin(), maxBody.end(), ' '), maxBody.end());
	_maxBody = maxBody;
}

void	Server::setIndex(std::string index)
{
	index.erase(std::remove(index.begin(), index.end(), ' '), index.end());
	_index = index;
}

void	Server::setAutoIndex(std::string autoindex)
{
	autoindex.erase(std::remove(autoindex.begin(), autoindex.end(), ' '), autoindex.end());
	_autoindex = autoindex;
}

void	Server::setLocation(Location &location)
{
	_location.push_back(location);
}

void	Server::setRedir(std::string code, std::string domain)
{
	// code.erase(std::remove(code.begin(), code.end(), ' '), code.end());
	domain.erase(std::remove(domain.begin(), domain.end(), ' '), domain.end());
	code.erase(std::remove(code.begin(), code.end(), ' '), code.end());
    
    // If i have already a redirection i delete it and replace it
    if(this->getRedir().size())
    {
        this->_redir.erase(this->_redir.begin());
    }
    //add the new redirection
	this->_redir.insert(std::make_pair(code, domain));
}

void Server::setErrorPage(std::string code, std::string errorFile)
{
	// errorFile.erase(std::remove(errorFile.begin(), errorFile.end(), ' '), errorFile.end());
	_errorPage.insert(std::make_pair(code, errorFile));
}

void	Server::setSocketFd(int sockfd)
{
	socketfd.insert(sockfd);
}

/* ================ */
/*		GETTER		*/
/* ================ */

std::string	Server::getPort() const
{
	return (_port);
}

std::string	Server::getServerName() const
{
	return (_server_name);
}

std::string	Server::getPath() const
{
	return (_path);
}

std::string	Server::getMaxBody() const
{
	return (_maxBody);
}

std::string	Server::getIndex() const
{
	return (_index);
}

std::string	Server::getAutoIndex() const
{
	return (_autoindex);
}

std::map<std::string,std::string>	&Server::getRedir()
{
	return (_redir);
}

std::map<std::string,std::string>	&Server::getErrorPage()
{
	return (_errorPage);
}

std::vector<Location>	&Server::getLocation()
{
	return (_location);
}

/* ================ */
/*		FILL		*/
/* ================ */

void	Server::fillPort(std::string line)
{
	size_t	pos = line.find("listen");
	int		port = 0;

	setPort(line.substr(pos + strlen("listen"), line.length()).c_str());
	if (getPort().empty())
		throw Response::ConfigurationFileServer("Port is empty");
	if (!ft_stoi(getPort().c_str(), port))
		throw Response::ConfigurationFileServer("Port is not a number");
	if (port < 0 || port > 65535)
		throw Response::ConfigurationFileServer("Port out of range");
}

void	Server::fillServerName(std::string line)
{
	size_t	pos = line.find("server_name");

	setServerName(line.substr(pos + strlen("server_name"), line.length() - (pos + strlen("server_name"))));
	bool	is_ip = _server_name.find_first_of("0123456789") != std::string::npos;
	if (is_ip)
	{
		std::vector<std::string>	substr_ip = ft_split(_server_name, '.');

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

void	Server::fillPath(std::string line)
{
	size_t pos = line.find("root");
	setPath(line.substr(pos + strlen("root"), line.length() - (pos + strlen("root"))));
	//if no "/" at the end add it
	if(getPath().at(getPath().size() - 1) != '/')
		setPath(getPath() + "/");
	//if no . at the begining add it
	if(getPath().at(0) != '.')
		setPath("." + getPath());
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

	setMaxBody(size);
}

void	Server::fillIndex(std::string line)
{
	size_t pos = line.find("index");
	setIndex(line.substr(pos + strlen("index"), line.length() - (pos + strlen("index"))));
}

//TODO - Delete spaces
void	Server::fillAutoIndex(std::string line)
{
	size_t pos = line.find("autoindex"); 
	setAutoIndex(line.substr(pos + strlen("autoindex"), line.length() - (pos + strlen("autoindex"))));
}

void	Server::fillErrorPage(std::string line)
{
	size_t pos = line.find("error_page");
	line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
	std::string code = line.substr(pos + strlen("error_page"), 3).c_str();
	std::string domain = line.substr(pos + strlen("error_page") + 3, line.length());
	setErrorPage(code, domain);
}

void	Server::fillRedir(std::string line)
{
	size_t pos = line.find("return");
	std::string code = line.substr(pos + strlen("return"), 4);
	std::string domain = line.substr(pos + strlen("return") + 4, line.length() - (pos + strlen("return")));
	setRedir(code, domain);
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
		else if (i > 5 && line.find("}") == std::string::npos && !line.empty())
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
	if (getRedir().size()) 
		std::cout << "return\t\t" YELLOW << getRedir().begin()->first << " " << getRedir().begin()->second << RESET << std::endl;
	std::cout << std::endl;
	//print every location of my current server
	for (int i = 1; itbeg != itend; ++itbeg, ++i)
		std::cout << BWHITE "\tLocation " << i << RESET << std::endl, (itbeg)->printConfig();

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