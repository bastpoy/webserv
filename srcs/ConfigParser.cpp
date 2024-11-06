#include "Header.hpp"

/* ================ */
/*	CANONICAL FORMS	*/
/* ================ */

ConfigParser::ConfigParser(void)
{
	// std::cout << GREEN "Creating a ConfigParser " RESET << std::endl;
}

ConfigParser::ConfigParser(char *path)
{
	this->_path = path;
}

ConfigParser::~ConfigParser()
{
}

/* ================ */
/*		SETTER		*/
/* ================ */

//Setter
void ConfigParser::addServer(Server &server)
{
	this->_servers.push_back(server);
	// std::cout << GREEN "Server added" RESET << std::endl;
}

//getter
std::vector<Server>	&ConfigParser::getServers()
{
	return(this->_servers);
}

/* ================ */
/*		PARSING		*/
/* ================ */


void	ConfigParser::parseConfig( )
{
	// std::cout << "---------PARSING CONF-----------\n";

	// ConfigParser *config;
	std::string line;
	std::ifstream file("./conf/Bastien.conf");
	// std::ifstream file(this->_path);

	if (!file.is_open())
	{
		std::cout << strerror(errno) << std::endl;
		throw Response::ErrorOpeningFile();
	}
	while (getline(file, line))
	{
		// std::cout << YELLOW << line << RESET << std::endl;
		// fill new server block
		if (line.find("server") != std::string::npos)
		{
			// std::cout << GREEN "\nNew Server detected" RESET << std::endl;
			//create a server instance and add it to Server Class
			Server server;
			getServerAttributs(file, server);
			addServer(server);
		}
	}
}

/**
 * @brief	This function will find each attribute to parse and redirectto the good fill function.
 * @note	It will erase all spaces between key and value in the configuration file.ADJ_FREQUENCY
 * @author	Ozan
*/
void ConfigParser::getServerAttributs(std::ifstream& file, Server &server)
{
	std::string line;

	while(getline(file, line))
	{
		if (line.find("listen") != std::string::npos)
			server.fillPort(line);
		else if(line.find("server_name") != std::string::npos)
			server.fillServerName(line);
		else if(line.find("root") != std::string::npos)
			server.fillPath(line);
		else if(line.find("client_max_body_size") != std::string::npos)
			server.fillMaxBody(line);
		else if(line.find("index") != std::string::npos)
			server.fillIndex(line);
		else if(line.find("return") != std::string::npos)
			server.fillRedir(line);
		else if(line.find("error_page") != std::string::npos)
			server.fillErrorPage(line);
		else if(line.find("location") != std::string::npos)
			server.fillLocation(file, line);
		// getLocationAttributs(file, server, line);
		else if(line.find("}") != std::string::npos)
			return ;
	}
}

/* ================ */
/*		DEBUG		*/
/* ================ */

void ConfigParser::printConfig()
{
	std::vector<Server>::iterator	itbeg = this->_servers.begin();
	std::vector<Server>::iterator	itend = this->_servers.end();
	int								i = 1;
	std::cout << "---------PRINTING CONF-----------\n\n";

	while(itbeg != itend)
	{
		//print server config who call location config
		std::cout << BWHITE "Server "  << i++ << RESET << std::endl;
		(itbeg)->printConfig();
		itbeg++;
	}
}