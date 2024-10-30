#ifndef SERVER_HPP
# define SERVER_HPP

# include "Header.hpp"

class Location;
class ConfigParser;

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
class Server
{
	private:
		// Server Address
		int								_sockfd;	// or server_fd
		std::vector<struct sockaddr_in>	_listenAddr;

		// Server (config)
		bool						_isfree;
		std::string 				_port;
		std::string					_server_name;
		std::string					_path;
		std::string					_maxBody;
		std::string					_index;
		std::map<int, std::string>	_errorPage;
		std::map<int, std::string>	_redir;
		std::vector<Location>		_location;


	public:
		// Canonical form (constructor, destructor, copy operations)
		Server(void);
		~Server(void);
		Server(const Server &other);
		Server	&operator=(const Server &other);

		// Setter
		void	setPort(std::string port);
		void	setServerName(std::string server_name);
		void	setPath(std::string path);
		void	setMaxBody(std::string maxBody);
		void	setIndex(std::string index);
		void	setErrorPage(int code, std::string errorFile);
		void	setRedir(int code, std::string domain);
		void	setLocation(Location &location);
			// ServerAddr Setters
		void	setSocketFd(int sockfd);
		void	setListenAddr(struct sockaddr_in addr);

		// Getter
		std::string						getPort() const;
		std::string						getServerName() const;
		std::string						getPath() const;
		std::string						getMaxBody() const;
		std::string						getIndex() const;
		std::map<int,std::string>		&getErrorPage();
		std::map<int,std::string>		&getRedir();
		std::vector<Location>			&getLocation();
			// ServerAddr Getters
		int								getSocketFd() const;
		std::vector<struct sockaddr_in>	&getListenAddr();

		// Fill
		void	fillPort(std::string line);
		void	fillServerName(std::string line);
		void	fillPath(std::string line);
		void	fillMaxBody(std::string line);
		void	fillIndex(std::string line);
		void	fillErrorPage(std::string line);
		void	fillRedir(std::string line);
		void	fillLocation(std::ifstream &file, std::string line);
			// ServerAddr Fill
		void	createListenAddr(ConfigParser &config);

		// Debug
		void	printConfig();

		// Amandine functions
		// void	listen();			// pour écouter sur une adresse IP et un port spécifiés.
		// void	acceptConnection();	// pour accepter les connexions entrantes.
		// void	setup();			// pour la configuration du serveur avec les options données (par exemple, plusieurs configurations de serveur virtuel).

};

# endif /* SERVER_HPP */