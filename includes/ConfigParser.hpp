#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

# include "Header.hpp"

/**
 * @brief	Class responsible for parsing the configuration file for the server.
 * @note	This class reads and stores configuration parameters such as server port, root directory,
 * 			and virtual server (host) settings. It allows for flexible server configuration and 
 * 			manages the setup of multiple virtual hosts.
 * @details	Similar functionality to "httpConfig.hpp", designed to handle HTTP server configurations.
 */
class ConfigParser
{
	private:
		std::vector<Server>	_servers; // Stores the list of configured servers
		std::string			_path;
	// 	int		_port;
	// 	int		_server_root;

	public:
		// Canonical form
		ConfigParser(void);
		ConfigParser(char *path);
		~ConfigParser(void);

		void				addServer(Server &server);
		std::vector<Server>	&getServers();
		void				parseLine(std::string &line);
		void				checkServerAttributs(Server &server, std::vector<Server> &servers);
		void				parseConfig(std::vector<Server> &servers); // pour analyser le fichier de configuration et remplir les attributs.
		void				getServerAttributs(std::ifstream &file, Server &server);
		void				printConfig();

};

# endif /* CONFIGPARSER_HPP */