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

/**
 * @brief	Class representing the main HTTP server.
 * @note	Responsible for listening on one or more ports for incoming connections.
 * 			Accepts connections from clients and redirects them to a request handler.
 * @details	Responsible for server configuration and management of virtual hosts.
*/

// typedef struct s_session {
//     time_t expireDate;
//     std::string id;
//     int is_valid;
//     std::map<std::string, std::string>  credentials;
// } t_session;

typedef struct s_serverData
{
	int									sockfd;
	std::string 						port;
	std::string							server_name;
	std::string							path;
	std::string							maxBody;
	std::string							index;
	std::string							autoindex;
    std::string                         buffer;
    std::string                         header;
    std::string                         body;
	std::map<std::string, std::string>	errorPage;
	std::map<std::string, std::string>	redir;
	std::vector<Location>				location;
    std::vector<std::string>            requestAllow;
    // t_session                           *session;
}t_serverData;

class Server
{
	private:

		// Server (config)									// Some example
		std::string 						_port;			// 8080
		std::string							_server_name;	// 127.0.0.3
		std::string							_path;			// ? root ?
		std::string							_maxBody;		// 36M
		std::string							_index;			// index.html
		std::string							_autoindex;		// on/off
        std::map<std::string, std::string>	_errorPage;		// 404: /var/www/error/error404.html
		std::map<std::string, std::string>	_redir;			// 302: http://127.0.0.3:8080
		std::vector<Location>				_location;
		// Server file descriptor
		std::set<int>						socketfd;
	
	public:

		// Setter
		void	setPort(std::string port);
		void	setServerName(std::string server_name);
		void	setPath(std::string path);
		void	setMaxBody(std::string maxBody);
		void	setIndex(std::string index);
		void	setAutoIndex(std::string autoindex);
		void	setErrorPage(std::string code, std::string errorFile);
		void	setRedir(std::string code, std::string domain);
		void	setLocation(Location &location);
		// ServerAddr Setters
		void	setSocketFd(int sockfd);

		// Getter
		std::string							getPort() const;
		std::string							getServerName() const;
		std::string							getPath() const;
		std::string							getMaxBody() const;
		std::string							getIndex() const;
		std::string							getAutoIndex() const;
        std::map<std::string,std::string>   &getErrorPage();
		std::map<std::string,std::string>	&getRedir();
		std::vector<Location>				&getLocation();

		// Fill
		void	fillPort(std::string line);
		void	fillServerName(std::string line);
		void	fillPath(std::string line);
		void	fillMaxBody(std::string line);
		void	fillIndex(std::string line);
		void	fillAutoIndex(std::string line);
		void	fillErrorPage(std::string line);
		void	fillRedir(std::string line);
		void	fillLocation(std::ifstream &file, std::string line);
		
		// ServerAddr Fill
		void	createListenAddr(ConfigParser &config);
		void	configuringNetwork(std::vector<Server>::iterator &itbeg, ConfigParser &config, int &epoll_fd);
		// Debug
		void	printConfig();

		// Amandine functions
		// void	listen();			// pour écouter sur une adresse IP et un port spécifiés.
		// void	acceptConnection();	// pour accepter les connexions entrantes.
		// void	setup();			// pour la configuration du serveur avec les options données (par exemple, plusieurs configurations de serveur virtuel).

};

void			maxBodyParsing(std::string caracter, std::string &size);
std::ostream&	operator<<(std::ostream& os, const Server& server);

# endif /* SERVER_HPP */