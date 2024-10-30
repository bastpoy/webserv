#ifndef CONFIG_HPP
# define CONFIG_HPP

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
	// 	int		_port;
	// 	int		_server_root;

	public:
		// Canonical form (constructor, destructor, copy operations)
		ConfigParser(void);
		~ConfigParser(void);
		ConfigParser(const ConfigParser &other);
		ConfigParser	&operator=(const ConfigParser &other);

		// Setter
		void	addServer(Server &server);

		// Getter
		std::vector<Server>	&getServers();

		// Parsing
		void	parseConfig(); // pour analyser le fichier de configuration et remplir les attributs.
		// void	parseConfigFile(); 
		void	getServerAttributs(std::ifstream &file, Server &server);

		// Debug
		void	printConfig();

};


# endif /* CONFIG_HPP */