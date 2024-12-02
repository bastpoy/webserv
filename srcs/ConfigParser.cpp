#include "Header.hpp"

std::string	keywords[] = {
	"listen",
	"server_name",
	"root",
	"client_max_body_size",
	"autoindex",
	"index",
	"return",
	"error_page"
};
void (Server::*serverFunctions[8])(std::string line) = {
	&Server::fillPort,
	&Server::fillServerName,
	&Server::fillPath,
	&Server::fillMaxBody,
	&Server::fillAutoIndex,
	&Server::fillIndex,
	&Server::fillRedir,
	&Server::fillErrorPage
};
const int	keywordsSize = 8;

/* ================ */
/*	CANONICAL FORMS	*/
/* ================ */

ConfigParser::ConfigParser(void) {}

ConfigParser::ConfigParser(char *path)
{
	_path = path;
}

ConfigParser::~ConfigParser(void) {}

/* ================ */
/*		SETTER		*/
/* ================ */

//Setter
void ConfigParser::addServer(Server &server)
{
	_servers.push_back(server);
}

void ConfigParser::setListData(t_serverData *data)
{
    this->listData.push_back(data);
}

//getter
std::vector<Server>	&ConfigParser::getServers(void)
{
	return(_servers);
}

std::vector<t_serverData *> ConfigParser::getListData(void) const
{
    return(this->listData);
}

/* ================ */
/*		PARSING		*/
/* ================ */

void	ConfigParser::checkServerAttributs(Server &server, std::vector<Server> &servers) //TODO - Mettre dans utils
{
	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		if (it->getServerName() == server.getServerName())
			throw Response::ConfigurationFileServer("Server name already exist");
	}
}

bool ConfigParser::isFileEmpty(const std::string &filePath) //TODO - Mettre dans utils
{
	std::ifstream file(filePath.c_str(), std::ios::ate);
	if (!file.is_open())
		throw Response::ErrorOpeningFile("Can't opening file.");
	return file.tellg() == 0;
}

void checkTwoKeywordsSameLine(const std::string	line)
{
	int count = 0;

	for (std::size_t i = 0; i < keywordsSize; ++i)
		if (line.find(keywords[i]) != std::string::npos)
			++count;
	if (count >= 2)
		throw std::runtime_error("Two keywords on the same line.");
}

void	ConfigParser::parseConfig(std::vector<Server> &servers)
{
	std::string		line;
	std::ifstream	file(_path.c_str());
	bool			bracket = false;
	bool			httpBlock = false;

	if (isFileEmpty(_path))
		throw Response::ErrorOpeningFile("File empty");
	while (getline(file, line))
	{
		rmComments(line);
		checkTwoKeywordsSameLine(line);
		if (line.find("http") != std::string::npos)
			httpBlock = true;
		if (line.find("{") != std::string::npos)
			bracket = true;
		// fill new server block
		if (line.find("server") != std::string::npos && httpBlock)
		{
			Server server;
			getServerAttributs(file, server);
			checkServerAttributs(server, servers);
			addServer(server);
            std::cout << "end parsing" << std::endl;
		}
		else if (line.find("server") != std::string::npos && !httpBlock)
			throw Response::ConfigurationFileServer("http block is missing");
		if (line.find("}") != std::string::npos)
		{
			if (bracket == false)
				throw Response::ConfigurationFileServer("Missing '{' in http block");
			return ;
		}
	}
	throw Response::ConfigurationFileServer("Missing '}'");
}

void	ConfigParser::rmComments(std::string &line) //TODO - Mettre dans utils
{
	size_t commentPos = line.find('#');

	if (commentPos != std::string::npos)
		line = line.substr(0, commentPos);
}

void	ConfigParser::checkSemicolon(std::string &line) //TODO - Mettre dans utils
{
	bool conditions = line.find("}") == std::string::npos && line.find("location") == std::string::npos && !line.empty();

	if (conditions && line.find_last_of(';') == std::string::npos)
		throw Response::ConfigurationFileServer("';' is missing");
	if (conditions && line.find(';') != line.size() - 1)
		throw Response::ConfigurationFileServer("Unknown character after ';'");
	if (conditions)
		line.erase(line.size() - 1);
}

void ConfigParser::parseLine(std::string &line) //TODO - Mettre dans utils
{
	bool	present = line.find("{") != std::string::npos;

	rmComments(line);

	// Supprime les espaces inutiles au début et à la fin
	line.erase(0, line.find_first_not_of(" \t")); // Trim début
	line.erase(line.find_last_not_of(" \t") + 1); // Trim fin

	if (present)
	{
		if (line.find("{") != line.size() - 1)
			throw Response::ConfigurationFileServer("Unknown character after '{'");
		return ;
	}

	checkSemicolon(line);

	if (line.find(";") == std::string::npos
		&& line.find("location") == std::string::npos
		&& !line.empty()
		&& line.find("}") != std::string::npos
		&& line.find('}') != line.size() - 1)
		throw Response::ConfigurationFileServer("Unknown character after '}'");
}

/**
 * @brief	This function will find each attribute to parse and redirectto the good fill function.
 * @note	It will erase all spaces between key and value in the configuration file.ADJ_FREQUENCY
 * @author	Amandine, Bastien, Ozan.
*/
void ConfigParser::getServerAttributs(std::ifstream& file, Server &server)
{
	std::string	line;
	bool		bracket = false;

	while(getline(file, line))
	{
		int i = 0;
		parseLine(line);
		if (line.find("{") != std::string::npos && line.find("location") == std::string::npos)
		{
			bracket = true;
			continue;
		}
		if (line.find("location") != std::string::npos)
		{
			server.fillLocation(file, line, server.getLocation());
			continue ;
		}
		while (i < 8 && line.find(keywords[i]) == std::string::npos)
			i++;
		if (i < 8)
			(server.*serverFunctions[i])(line);
		else if (i > 7 && line.find("}") == std::string::npos && !line.empty())
			throw Response::ConfigurationFileServer("Unknown attribute: " + line);
		if (line.find("}") != std::string::npos)
		{
			if (bracket == false)
				throw Response::ConfigurationFileServer("Missing '{' in server block");
			return ;
		}
	}
	throw Response::ConfigurationFileServer("Missing '}'");

}

/* ================ */
/*		DEBUG		*/
/* ================ */

void ConfigParser::printConfig(void)
{
	std::vector<Server>::iterator	itbeg = _servers.begin();
	std::vector<Server>::iterator	itend = _servers.end();

	std::cout << "---------PRINTING CONF-----------\n\n";
	for (int i = 1; itbeg != itend; ++itbeg, ++i)
		std::cout << BWHITE "Server " << i << RESET << std::endl, (itbeg)->printConfig();
	std::cout << "---------------------------------\n\n";
}