#ifndef HEADER_HPP
# define HEADER_HPP

// Bibliothèques standard C++
#include <string>				// Manipulation de chaînes
#include <vector>				// Conteneur vector
#include <map>					// Conteneur map
#include <exception>			// Gestion des exceptions
#include <stdexcept>			// Exceptions standard
#include <utility>				// Utilitaires
#include <cstddef>				// Types de taille

#include <iostream>				// Pour cout
#include <sstream>				// Pour les flux de chaînes
#include <fstream>				// std::ifstream

// Bibliothèques système / C
#include <sys/socket.h>			// Pour les fonctions de socket
#include <sys/wait.h>
#include <netinet/in.h>			// Pour sockaddr_in
#include <cstdlib>				// Pour exit() et EXIT_FAILURE
#include <errno.h>				// Pour errno
#include <string.h>				// Pour strerror
#include <unistd.h>				// Pour close
#include <arpa/inet.h>			// Pour inet_ntoa

// Fichiers d'en-tête spécifiques au projet
#include "Server.hpp"			// Classe pour le serveur
#include "ConfigParser.hpp"		// Classe pour parser la configuration
#include "Location.hpp"			// Classe pour les locations
// #include "Logger.hpp"			// Logger pour les messages
#include "Request.hpp"			// Classe pour les requêtes
#include "RequestHandler.hpp"	// Gestion des requêtes
#include "Response.hpp"			// Classe pour les réponses
#include "Client.hpp"			// Classe Client
// #include "Socket.hpp"			// Classe pour les sockets
// #include "VirtualHost.hpp"		// Classe pour les hôtes virtuels

/* ------------- COLORS ------------- */
// Reset
#define RESET   "\033[0m"

// Regular Colors
#define BLACK   "\033[0;30m"
#define RED     "\033[0;31m"
#define GREEN   "\033[0;32m"
#define YELLOW  "\033[0;33m"
#define BLUE    "\033[0;34m"
#define MAGENTA "\033[0;35m"
#define CYAN    "\033[0;36m"
#define WHITE   "\033[0;37m"

// Bold Colors
#define BBLACK   "\033[1;30m"
#define BRED     "\033[1;31m"
#define BGREEN   "\033[1;32m"
#define BYELLOW  "\033[1;33m"
#define BBLUE    "\033[1;34m"
#define BMAGENTA "\033[1;35m"
#define BCYAN    "\033[1;36m"
#define BWHITE   "\033[1;37m"

// Background Colors
#define ON_BLACK   "\033[40m"
#define ON_RED     "\033[41m"
#define ON_GREEN   "\033[42m"
#define ON_YELLOW  "\033[43m"
#define ON_BLUE    "\033[44m"
#define ON_MAGENTA "\033[45m"
#define ON_CYAN    "\033[46m"
#define ON_WHITE   "\033[47m"


// Class Prototypes
class Client;
class ConfigParser;
class location;
// class Logger;
class Request;
class RequestHandler;
class Response;
class Server;
// class Socket;
// class VirtualHost;

#endif /* HEADER_HPP */