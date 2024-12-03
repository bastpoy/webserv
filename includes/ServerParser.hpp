#ifndef SERVERPARSER_HPP
# define SERVERPARSER_HPP

# include "Header.hpp"

class Location;
class ConfigParser;

#define MAX_EVENTS 100
/*
But : Représente le serveur HTTP principal.
Rôle :
Écouter sur un ou plusieurs ports pour les connexions entrantes.
Accepter les connexions des clients et les rediriger vers un gestionnaire de requêtes.
*/

extern std::string	locationKeywords[];
extern const int	locKeywordsSize;

/**
 * @brief	Class representing the main HTTP server.
 * @note	Responsible for listening on one or more ports for incoming connections.
 * 			Accepts connections from clients and redirects them to a request handler.
 * @details	Responsible for server configuration and management of virtual hosts.
*/


typedef struct s_cgi
{
	int     cgifd;
	int     cgipid;
	int     parentsocket;
	time_t  cgiTimeout;
}t_cgi;

typedef struct s_serverData
{
	int									sockfd;
	std::string 						port;
	std::string 						ip;
	std::string							server_name;
	std::string							path;
	std::string							maxBody;
	std::string							index;
	std::string							autoindex;
	std::string							buffer;
	std::string							header;
	std::string							body;
	t_cgi								*cgi;
	std::map<std::string, std::string>	errorPage;
	std::map<std::string, std::string>	cgiPath;
	std::map<std::string, std::string>	redir;
	std::vector<Location>				location;
	std::vector<std::string>			requestAllow;
	// t_session							*session;
}	t_serverData;

class Server
{
	private:

		// Server (config)									// Some example
		std::string 						_port;			// 8080
		std::string 						_ip;			// 127.0.0.1
		std::string							_server_name;	// exemple.com
		std::string							_path;			// ? root ?
		std::string							_maxBody;		// 36M
		std::string							_index;			// index.html
		std::string							_autoindex;		// on/off
		std::map<std::string, std::string>	_errorPage;		// 404: /var/www/error/error404.html
		std::map<std::string, std::string>	_cgiPath;
		std::map<std::string, std::string>	_redir;			// 302: http://127.0.0.3:8080
		std::vector<Location>				_location;
		std::set<int>						socketfd;
	
	public:

		// Setter
		// void	setPort(std::string port);
		void	setIP(std::string port);
		void	setServerName(std::string server_name);
		void	setListen(std::string line);
		void	setRoot(std::string line);
		void	setMaxBody(std::string line);
		void	setIndex(std::string line);
		void	setAutoIndex(std::string line);
		void	setErrorPage(std::string line);
		void 	setCgiPath(std::string language, std::string path);
		void	setRedir(std::string line);
		void	setLocation(Location &location);
		// ServerAddr Setters
		void	setSocketFd(int sockfd);

		// Getter
		std::string							getPort() const;
		std::string							getIP() const;
		std::string							getServerName() const;
		std::string							getPath() const;
		std::string							getMaxBody() const;
		std::string							getIndex() const;
		std::string							getAutoIndex() const;
		std::map<std::string,std::string>	&getErrorPage();
		std::map<std::string,std::string>	&getCgiPath();
		std::map<std::string,std::string>	&getRedir();
		std::vector<Location>				&getLocation();

		// Fill
		// void	fillPort(std::string line);
		// void	fillListen(std::string line);
		// void	fillServerName(std::string line);
		// void	fillPath(std::string line);
		// void	fillMaxBody(std::string line);
		// void	fillIndex(std::string line);
		// void	fillAutoIndex(std::string line);
		// void	fillErrorPage(std::string line);
		void	fillCgiPath(std::string line);
		// void	fillRedir(std::string line);
		void	fillLocation(std::ifstream &file, std::string line, std::vector<Location> &location);
		
		// ServerAddr Fill
		void				createListenAddr(ConfigParser &config);
		void				configuringNetwork(std::vector<Server>::iterator &itbeg, ConfigParser &config, int &epoll_fd);
		struct epoll_event	fillEpoolDataInfo(int &client_fd, t_serverData *info);
		struct epoll_event	fillEpoolDataIterator(int sockfd, std::vector<Server>::iterator itbeg);
		void				setupSocket(int &sockfd, struct sockaddr_in &addr, std::vector<Server>::iterator itbeg);

		
		// Debug
		void	printConfig();
};

# endif /* SERVER_HPP */