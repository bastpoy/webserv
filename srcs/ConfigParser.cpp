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

void ConfigParser::setListData(t_serverData *data)
{
	this->listData.push_back(data);
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

std::vector<t_serverData *> ConfigParser::getListData(void) const
{
	return(this->listData);
}

/* ================ */
/*		PARSING		*/
/* ================ */

/**
 * @brief	This function will set the keywords and the functions to call for each keyword.
 * @note	Each keyword is associated with a function to call in the Server class.
*/
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

/**
 * @brief	This function will check if the server name already exist in the configuration file.
 * @note	Each server name must be unique.
 * @param	server		The server to check.
 * @param	servers		The list of servers.
*/
void	ConfigParser::checkServerAttributs(Server &server, std::vector<Server> &servers)
{
	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
		if (it->getServerName() == server.getServerName())
			throw Response::ConfigurationFileServer("Server name already exist");
}

/**
 * @brief	This function will check if the file is empty.
 * @note	An empty file will throw an exception.
 * @param	filePath	The path to the file.
*/
bool ConfigParser::isFileEmpty(const std::string &filePath)
{
	std::ifstream file(filePath.c_str(), std::ios::ate);
	if (!file.is_open())
		throw Response::ErrorOpeningFile("Can't opening file.");
	return file.tellg() == 0;
}

/**
 * @brief	This function will check if two keywords are on the same line.
 * @note	If two keywords are on the same line, an exception will be thrown.
 * @param	line		The line to check.
 * @param	keywords	The list of keywords.
 * @param	keywordsSize	The size of the list of keywords.
*/
void checkTwoKeywordsSameLine(const std::string	line, std::vector<std::string> keywords, const int keywordsSize)
{
	int count = 0;

	for (int i = 0; i < keywordsSize; ++i)
		if (line.find(keywords[i]) != std::string::npos)
			++count;
	if (count >= 2)
		throw Response::ConfigurationFileServer("Two keywords on the same line.");
}

/**
 * @brief	This function will parse the configuration file.
 * @note	It will read the file line by line and parse each line.
 * @param	servers		The list of servers.
*/
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
		if (line.find("server") != std::string::npos && httpBlock)
		{
			Server server;
			getServerAttributs(file, server, getKeywords(), _serverFunctions, line.find("{") != std::string::npos);
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

/**
 * @brief	This function will remove comments from a line.
 * @note	Comments are defined by a '#' character.
 * @param	line	The line to remove comments from.
*/
void	ConfigParser::rmComments(std::string &line)
{
	size_t commentPos = line.find('#');

	if (commentPos != std::string::npos)
		line = line.substr(0, commentPos);
}

/**
 * @brief	This function will check if a semicolon is present at the end of a line.
 * @note	If a semicolon is missing, an exception will be thrown.
 * @param	line	The line to check.
*/
void	ConfigParser::checkSemicolon(std::string &line)
{
	bool conditions = line.find("}") == std::string::npos && line.find("location") == std::string::npos && !line.empty();

	if (conditions && line.find_last_of(';') == std::string::npos)
		throw Response::ConfigurationFileServer("';' is missing");
	if (conditions && line.find(';') != line.size() - 1)
		throw Response::ConfigurationFileServer("Unknown character after ';'");
	if (conditions)
		line.erase(line.size() - 1);
}

/**
 * @brief	This function will parse a line.
 * @note	It will remove all spaces at the beginning and end of the line.
 * @param	line	The line to parse.
*/
void ConfigParser::parseLine(std::string &line)
{
	bool	present = line.find("{") != std::string::npos;

	rmComments(line);
	line.erase(0, line.find_first_not_of(" \t"));
	line.erase(line.find_last_not_of(" \t") + 1);
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
 * @brief	This function will find each attribute to parse and redirect to the good fill function.
 * @note	It will erase all spaces between key and value in the configuration file.
 * @param	file			The file to read.
 * @param	server			The server to fill.
 * @param	keywords		The list of keywords.
 * @param	serverFunctions	The list of functions to call.
 * @param	bracket			True if a bracket is present.
*/
void ConfigParser::getServerAttributs(std::ifstream& file, Server &server, std::vector<std::string> keywords, std::vector<void (Server::*)(std::string)> serverFunctions, bool bracket)
{
	std::string	line;

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

/**
 * @brief	This function will print the configuration.
*/
void ConfigParser::printConfig(void)
{
	std::vector<Server>::iterator	itbeg = _servers.begin();
	std::vector<Server>::iterator	itend = _servers.end();

	std::cout << "---------PRINTING CONF-----------\n\n";
	for (int i = 1; itbeg != itend; ++itbeg, ++i)
		std::cout << BWHITE "Server " << i << RESET << std::endl, (itbeg)->printConfig();
	std::cout << "---------------------------------\n\n";
}
