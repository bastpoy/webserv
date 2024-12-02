#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

# include "Header.hpp"


extern std::string	keywords[];
extern const int	keywordsSize;

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
		std::vector<Server>	_servers;
		std::string			_path;

	public:
		ConfigParser(void);
		ConfigParser(char *path);
		~ConfigParser(void);

		std::vector<Server>	&getServers(void);
		void				parseConfig(std::vector<Server> &servers);
		void				addServer(Server &server);
		void				getServerAttributs(std::ifstream &file, Server &server);
		void				printConfig(void);
		
		bool				isFileEmpty(const std::string &filePath); //TODO - Mettre dans utils :
		static void			rmComments(std::string &line);
		static void			checkSemicolon(std::string &line);
		static void			parseLine(std::string &line);
		void				checkServerAttributs(Server &server, std::vector<Server> &servers);

};

# endif /* CONFIGPARSER_HPP */