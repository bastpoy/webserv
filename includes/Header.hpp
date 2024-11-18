#ifndef HEADER_HPP
# define HEADER_HPP

// Bibliothèques standard C++
#include <string>				// Manipulation de chaînes
#include <vector>				// Conteneur vector
#include <map>
# include <set>					// Conteneur map
#include <exception>			// Gestion des exceptions
#include <stdexcept>			// Exceptions standard
#include <utility>				// Utilitaires
#include <cstddef>				// Types de taille

#include <iostream>				// Pour cout
#include <sstream>				// Pour les flux de chaînes
#include <fstream>				// std::ifstream
#include <algorithm>			// Pour erase()

// Bibliothèques système / C
#include <dirent.h>				// For auto indexing
#include <sys/stat.h>			// For auto indexing
#include <sys/socket.h>			// Pour les fonctions de socket
#include <sys/wait.h>
#include <netinet/in.h>			// Pour sockaddr_in
#include <cstdlib>				// Pour exit() et EXIT_FAILURE
#include <errno.h>				// Pour errno
#include <string.h>				// Pour strerror
#include <unistd.h>				// Pour close
#include <arpa/inet.h>			// Pour inet_ntoa
#include <sys/epoll.h>
#include <fcntl.h>

// Fichiers d'en-tête spécifiques au projet
#include "CGIHandler.hpp"		// Classe pour gerer les CGI
#include "ServerParser.hpp"		// Classe pour le serveur
#include "Server.hpp"			// Classe pour le serveur
#include "ConfigParser.hpp"		// Classe pour parser la configuration
#include "Location.hpp"			// Classe pour les locations
#include "Request.hpp"			// Classe pour les requêtes
#include "RequestHandler.hpp"	// Gestion des requêtes
#include "Response.hpp"			// Classe pour les réponses
#include "Utils.hpp"
#include "delete.hpp"

/* ------------- COLORS ------------- */
// Reset
#define RESET   "\e[0m"

// Regular Colors
#define BLACK   "\e[0;30m"
#define RED     "\e[0;31m"
#define GREEN   "\e[0;32m"
#define YELLOW  "\e[0;33m"
#define BLUE    "\e[0;34m"
#define MAGENTA "\e[0;35m"
#define CYAN    "\e[0;36m"
#define WHITE   "\e[0;37m"

// Bold Colors
#define BBLACK   "\e[1;30m"
#define BRED     "\e[1;31m"
#define BGREEN   "\e[1;32m"
#define BYELLOW  "\e[1;33m"
#define BBLUE    "\e[1;34m"
#define BMAGENTA "\e[1;35m"
#define BCYAN    "\e[1;36m"
#define BWHITE   "\e[1;37m"

// Background Colors
#define ON_BLACK   "\e[40m"
#define ON_RED     "\e[41m"
#define ON_GREEN   "\e[42m"
#define ON_YELLOW  "\e[43m"
#define ON_BLUE    "\e[44m"
#define ON_MAGENTA "\e[45m"
#define ON_CYAN    "\e[46m"
#define ON_WHITE   "\e[47m"


// Class Prototypes
class Client;
class ConfigParser;
class location;
// class Logger;
class Request;
class RequestHandler;
class Response;
class Server;

extern int session_id;

// autoindex.cpp Functions
bool						isDirectory(const std::string& path);
std::vector<std::string>	listDirectory(const std::string& directory);
std::string					generateAutoIndexPage(const std::string directory, const std::vector<std::string>& files);
std::string					handleAutoIndex(const std::string& path);

// get.cpp Functions
void						redirRequest(std::string location, int fd);
std::string                 check_location(std::string &uri, std::string &content, std::vector<Location> &location, t_serverData *data);
std::string                 getContentType(std::string &path);
void                        checkAccessFile(std::string &code, std::string &filePath, t_serverData *data);
std::string                 httpGetResponse(std::string code, std::string contentType, std::string content);
void                        parseAndGetRequest(std::string buffer, t_serverData *data);

// post.cpp Functions
void		sendPostData(std::string code , std::string contentType, std::string content, t_serverData *data);
int 		getContentLength(std::string header, t_serverData *data);
int         getContentLength(std::string header, t_serverData *data);
std::string	getFileName(std::string body);
void		postRequest(std::string buffer, t_serverData *data);

//Cookies
void            putDataSession(std::map<std::string, std::string> values, t_serverData *data);
void            newSessionCookie(std::map<std::string, std::string> values, t_serverData *data);
std::string     manageDate(time_t current_time);
bool check_cookie_validity(t_serverData *data);

//Response
std::string     httpGetResponseDownload(std::string code, std::string contentType, std::string content);
std::string     httpGetResponse(std::string code, std::string contentType, std::string content);
void            httpPostResponse(std::string code , std::string contentType, std::string content, t_serverData *data);

// error.cpp Functions
void	errorCloseEpollFd(int &epoll_fd, int errCode);
void	contentTooLarge(std::string size, t_serverData *data);
void	notFound(t_serverData *data);
void	errorPage(std::string error, t_serverData *data);
void    notFoundFavicon(t_serverData *data);


#endif /* HEADER_HPP */