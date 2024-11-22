#include "Header.hpp"

/* ================ */
/*	CANONICAL FORMS	*/
/* ================ */

ConfigParser::ConfigParser(void) {}

ConfigParser::ConfigParser(char *path)
{
	this->_path = path;
}

ConfigParser::~ConfigParser() {}

/* ================ */
/*		SETTER		*/
/* ================ */

//Setter
void ConfigParser::addServer(Server &server)
{
	this->_servers.push_back(server);
}

//getter
std::vector<Server>	&ConfigParser::getServers()
{
	return(this->_servers);
}

/* ================ */
/*		PARSING		*/
/* ================ */

void	ConfigParser::checkServerAttributs(Server &server, std::vector<Server> &servers)
{
	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		if (it->getServerName() == server.getServerName())
			throw Response::ConfigurationFileServer("Server name already exist");
	}
}

bool ConfigParser::isFileEmpty(const std::string &filePath)
{
	std::ifstream file(filePath.c_str(), std::ios::ate);
	if (!file.is_open())
		throw Response::ErrorOpeningFile("Can't opening file.");
	return file.tellg() == 0;
}

void	ConfigParser::parseConfig(std::vector<Server> &servers)
{
	std::string line;
	bool	serverFound = false;
	std::ifstream file(this->_path.c_str());
	(void)servers;

	if (isFileEmpty(this->_path))
		throw Response::ErrorOpeningFile("File empty");
	while (getline(file, line))
	{
		rmComments(line);

		// fill new server block
		if (line.find("server ") != std::string::npos)
		{
			Server server;
			getServerAttributs(file, server);
			// checkServerAttributs(server, servers); // TODO - look si besoin
			addServer(server);
			serverFound = true;
		}
	}
	if (!serverFound)
		throw Response::ConfigurationFileServer("No server found");
}

void	ConfigParser::rmComments(std::string &line)
{
	size_t commentPos = line.find('#');

	if (commentPos != std::string::npos)
		line = line.substr(0, commentPos);
}

void	ConfigParser::checkSemicolon(std::string &line)
{
	bool conditions = line.find("}") == std::string::npos && line.find("location") == std::string::npos && !line.empty();
	if (conditions && line.find_last_of(';') == std::string::npos)
		throw Response::ConfigurationFileServer("\';\' is missing");
	if (conditions)
		line.erase(line.size() - 1);
}

void ConfigParser::parseLine(std::string &line)
{
	rmComments(line);

	// Supprime les espaces inutiles au début et à la fin
	line.erase(0, line.find_first_not_of(" \t")); // Trim début
	line.erase(line.find_last_not_of(" \t") + 1); // Trim fin

	checkSemicolon(line);
}

/**
 * @brief	This function will find each attribute to parse and redirectto the good fill function.
 * @note	It will erase all spaces between key and value in the configuration file.ADJ_FREQUENCY
 * @author	Amandine, Bastien, Ozan.
*/
void ConfigParser::getServerAttributs(std::ifstream& file, Server &server)
{
	std::string	line;
	std::string	directives[8] = {"listen", "server_name", "root", "client_max_body_size", "autoindex", "index", "return", "error_page"};
	void		(Server::*ptrs[8])(std::string line) = {&Server::fillPort, &Server::fillServerName, &Server::fillPath, &Server::fillMaxBody, &Server::fillAutoIndex, &Server::fillIndex, &Server::fillRedir, &Server::fillErrorPage};
	int			i = -1;

	while(getline(file, line))
	{
		i = 0;
		if (line.find("{") != std::string::npos)
			continue;
		parseLine(line);
		if (line.find("location") != std::string::npos)
		{
			server.fillLocation(file, line, server.getLocation());
			// continue ;
		}
		while (i < 8 && line.find(directives[i]) == std::string::npos)
			i++;
		if (i < 8)
			(server.*ptrs[i])(line);
		else if (i > 7 && line.find("}") == std::string::npos && !line.empty())
			throw Response::ConfigurationFileServer("Unknown attribute: " + line);
		if (line.find("}") != std::string::npos)
			return;
	}
}

/* ================ */
/*		DEBUG		*/
/* ================ */

void ConfigParser::printConfig()
{
	std::vector<Server>::iterator	itbeg = this->_servers.begin();
	std::vector<Server>::iterator	itend = this->_servers.end();

	std::cout << "---------PRINTING CONF-----------\n\n";
	for (int i = 1; itbeg != itend; ++itbeg, ++i)
		std::cout << BWHITE "Server " << i << RESET << std::endl, (itbeg)->printConfig();
	std::cout << "---------------------------------\n\n";
}