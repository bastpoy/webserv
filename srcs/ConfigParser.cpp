#include "Header.hpp"

/* ================ */
/*	CANONICAL FORMS	*/
/* ================ */

ConfigParser::ConfigParser(void) {}

ConfigParser::ConfigParser(char *path): _path(path) {}

ConfigParser::~ConfigParser(void) {}

/* ================ */
/*		SETTER		*/
/* ================ */

void ConfigParser::addServer(Server &server)
{
	_servers.push_back(server);
}

/* ================ */
/*		GETTER		*/
/* ================ */

std::vector<Server>	&ConfigParser::getServers(void) { return(_servers); }

std::vector<std::string>	ConfigParser::getKeywords(void)
{
	std::vector<std::string>	keywords;

	for (int i = 0; i < _keywordsSize; ++i)
		keywords.push_back(_keywords[i][0]);
	return (keywords);
}

int ConfigParser::getKeywordsSize(void) { return (_keywordsSize); }

/* ================ */
/*		PARSING		*/
/* ================ */

void	ConfigParser::functionConfig(void)
{
	_keywords[0].push_back("listen");
	_keywords[1].push_back("server_name");
	_keywords[2].push_back("root");
	_keywords[3].push_back("client_max_body_size");
	_keywords[4].push_back("autoindex");
	_keywords[5].push_back("index");
	_keywords[6].push_back("return");
	_keywords[7].push_back("error_page");
	_keywords[8].push_back("cgi_path");
	_keywords[9].push_back("allowed_methods");

	_serverFunctions.push_back(&Server::setListen);
	_serverFunctions.push_back(&Server::setServerName);
	_serverFunctions.push_back(&Server::setRoot);
	_serverFunctions.push_back(&Server::setMaxBody);
	_serverFunctions.push_back(&Server::setAutoIndex);
	_serverFunctions.push_back(&Server::setIndex);
	_serverFunctions.push_back(&Server::setRedir);
	_serverFunctions.push_back(&Server::setErrorPage);
	_serverFunctions.push_back(&Server::setCgiPath);
	_serverFunctions.push_back(&Server::setAllowedMethods);
}

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

void checkTwoKeywordsSameLine(const std::string	line, std::vector<std::string> keywords, const int keywordsSize)
{
	int count = 0;

	for (int i = 0; i < keywordsSize; ++i)
		if (line.find(keywords[i]) != std::string::npos)
			++count;
	if (count >= 2)
		throw Response::ConfigurationFileServer("Two keywords on the same line.");
}

void	ConfigParser::parseConfig(std::vector<Server> &servers)
{
	std::string		line;
	std::ifstream	file(_path.c_str());
	bool			bracket = false;
	bool			httpBlock = false;

	functionConfig();
	if (isFileEmpty(_path))
		throw Response::ErrorOpeningFile("File empty");
	while (getline(file, line))
	{
		rmComments(line);
		checkTwoKeywordsSameLine(line, getKeywords(), getKeywordsSize());
		if (line.find("http") != std::string::npos)
			httpBlock = true;
		if (line.find("{") != std::string::npos)
			bracket = true;
		// fill new server block
		if (line.find("server") != std::string::npos && httpBlock)
		{
			Server server;
			getServerAttributs(file, server, getKeywords(), _serverFunctions);
			checkServerAttributs(server, servers);
			addServer(server);
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
void ConfigParser::getServerAttributs(std::ifstream& file, Server &server, std::vector<std::string> keywords, std::vector<void (Server::*)(std::string)> serverFunctions)
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
			server.functionConfig();
			server.fillLocation(file, line, server.getLocation());
			continue ;
		}
		while (i < getKeywordsSize() && line.find(keywords[i]) == std::string::npos)
			i++;
		if (i < getKeywordsSize())
			(server.*serverFunctions[i])(line);
		else if (i > (getKeywordsSize() - 1) && line.find("}") == std::string::npos && !line.empty())
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