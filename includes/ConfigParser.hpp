#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

# include "Header.hpp"


// extern std::string	keywords[];
// extern const int	keywordsSize;

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
		std::vector<Server>	                _servers;
		std::string			                _path;
		// std::vector<std::string>					_keywords[9];

		std::vector<std::string>					_keywords[10];
		std::vector<void (Server::*)(std::string)>	_serverFunctions;
		static int const							_keywordsSize = 10;
        std::vector<t_serverData *>         listData;

	public:
		ConfigParser(void);
		ConfigParser(char *path);
		~ConfigParser(void);

		std::vector<Server>	        &getServers(void);
		std::vector<std::string>	getKeywords(void);
		int 				        getKeywordsSize(void);


		void				        parseConfig(std::vector<Server> &servers);
		void				        addServer(Server &server);
		void				        getServerAttributs(std::ifstream& file, Server &server, std::vector<std::string> keywords, std::vector<void (Server::*)(std::string)> serverFunctions);
		// void				        getServerAttributs(std::ifstream &file, Server &server);
		void				        printConfig(void);
        std::vector<t_serverData *> getListData(void) const;
		
		void				functionConfig(void);
		bool				isFileEmpty(const std::string &filePath); //TODO - Mettre dans utils :
		static void			rmComments(std::string &line);
		static void			checkSemicolon(std::string &line);
		static void			parseLine(std::string &line);
		void				checkServerAttributs(Server &server, std::vector<Server> &servers);
        void                setListData(t_serverData *data);
};

# endif /* CONFIGPARSER_HPP */