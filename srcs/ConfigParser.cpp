#include "Header.hpp"

/* ================ */
/*	CANONICAL FORMS	*/
/* ================ */

ConfigParser::ConfigParser(void)
{
	std::cout << GREEN "Creating a ConfigParser " RESET << std::endl;
}

ConfigParser::ConfigParser(char *path)
{
	std::cout << GREEN "Creating a ConfigParser from \"" << path << "\"" RESET << std::endl;
	this->_path = path;
}

ConfigParser::~ConfigParser()
{
	// std::vector<serverConfig*>::iterator it = server.begin();
	// while(it != server.end())
	// {
	//     std::cout << (*it) << std::endl;
	//     delete (*it);
	//     it++;
	// }
	// server.clear();

	std::cout << RED "Destroying a ConfigParser " RESET << std::endl;
}

ConfigParser::ConfigParser(const ConfigParser &other)
{
	*this = other;
}

ConfigParser	&ConfigParser::operator=(const ConfigParser &other)
{
	if (this == &other)
		return (*this);
	return (*this);
}

/* ================ */
/*		SETTER		*/
/* ================ */

//Setter
void ConfigParser::addServer(Server &server)
{
	this->_servers.push_back(server);
	std::cout << GREEN "Server added" RESET << std::endl;
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
	// ConfigParser *config;
	std::string line;
	// std::ifstream file("./conf/Bastien.conf");
	std::ifstream file(this->_path);

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
			std::cout << GREEN "\nNew Server detected" RESET << std::endl;
			//create a server instance and add it to Server Class
			Server server;
			getServerAttributs(file, server);
			addServer(server);
		}
	}
}

//parsing
void ConfigParser::getServerAttributs(std::ifstream& file, Server &server)
{
	std::string line;

	while(getline(file, line))
	{
		//fill the port
		if (line.find("listen") != std::string::npos)
			server.fillPort(line);
		//fill the different name of the server domain
		else if(line.find("server_name ") != std::string::npos)
			server.fillServerName(line);
		//fill the root for serving statick files
		else if(line.find("root ") != std::string::npos)
			server.fillPath(line);
		//fill the maxbody size
		else if(line.find("client_max_body_size ") != std::string::npos)
			server.fillMaxBody(line);
		//fill the index of the file to serve
		else if(line.find("index ") != std::string::npos)
			server.fillIndex(line);
		//fille a redirection 
		else if(line.find("return ") != std::string::npos)
			server.fillRedir(line);
		//fill an error page
		else if(line.find("error_page ") != std::string::npos)
			server.fillErrorPage(line);
		//fill an entire location block
		else if(line.find("location ") != std::string::npos)
			server.fillLocation(file, line);
		// getLocationAttributs(file, server, line);
		else if(line.find("}") != std::string::npos)
		{
			std::cout << "\n\n";
			return ;
		}
	}
}


// Debug
void ConfigParser::printConfig()
{
	std::vector<Server>::iterator itbeg = this->_servers.begin();
	std::vector<Server>::iterator itend = this->_servers.end();
	std::cout << "---------PRINTING CONF-----------\n\n";  

	while(itbeg != itend)
	{
		//print server config who call location config
		(itbeg)->printConfig();
		itbeg++;
	}
	std::cout << "\n\n";
}