// #include "resHeader.hpp"
// #include <sys/epoll.h>

// template <typename T>
// std::string to_string(T value) {
//   std::ostringstream oss;
//   oss << value;
//   return oss.str();
// }

// std::string readFile(std::string &path)
// {
// 	std::string content;
// 	std::string finalContent;
// 	std::ifstream file(path.c_str(), std::ios::binary);
// 	if(!file.is_open())
// 	{
// 		throw resHeader::ErrorOpeningFile();
// 	}
// 	return std::string(
// 		std::istreambuf_iterator<char>(file),
// 		std::istreambuf_iterator<char>()
// 	);
// }

// void getFileContent(std::string &uri , std::string &contentType, int connection)
// {
// 	std::string defaultPath = "./website";
// 	std::string filePath = defaultPath + uri; // Change this to your file path
// 	std::string content;

// 	//read the file content 
// 	content = readFile(filePath);
// 	//return the response
// 	std::cout << "the content type: " << contentType << std::endl;
// 	std::string response = "HTTP/1.1 200 OK \r\n"
// 							// "Content-Type: text/html\r\n"
// 							"Content-Type: " + contentType + "\r\n"
// 							"Content-Length: " + to_string(content.size()) + "\r\n"
// 							"Connection: close\r\n"
// 							"\r\n" + content;
// 	if(send(connection, response.c_str(), response.size(), 0) < 0)
// 	{
// 		std::cout << strerror(errno) << std::endl;
// 		throw resHeader::ErrorSendingResponse(); 
// 	}
// }

// std::string getContentType(const std::string &path) 
// {
// 	std::map<std::string, std::string> contentTypes;

// 	contentTypes.insert(std::pair<std::string, std::string>(".html", "text/html"));
// 	contentTypes.insert(std::pair<std::string, std::string>(".css", "text/css"));
// 	contentTypes.insert(std::pair<std::string, std::string>(".js", "application/javascript"));
// 	contentTypes.insert(std::pair<std::string, std::string>(".png", "image/png"));
// 	contentTypes.insert(std::pair<std::string, std::string>(".jpg", "image/jpeg"));
// 	contentTypes.insert(std::pair<std::string, std::string>(".gif", "image/gif"));

// 	size_t dotPos = path.find_last_of(".");
// 	if (dotPos != std::string::npos) {
// 		std::string extension = path.substr(dotPos);
// 		if (contentTypes.find(extension) != contentTypes.end()) {
// 		return contentTypes[extension];
// 		}
// 	}
// 	return "application/octet-stream"; // Default content type
// }


// int main() {
// 	// Create a socket (IPv4, TCP)
// 	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
// 	if (sockfd == -1)
// 	{
// 		std::cerr << "Failed to create socket. errno: " << errno << std::endl;
// 		exit(EXIT_FAILURE);
// 	}

// 	// Bind the socket to an address and port
// 	sockaddr_in sockaddr;
// 	sockaddr.sin_family = AF_INET;
// 	sockaddr.sin_addr.s_addr = INADDR_ANY;
// 	sockaddr.sin_port = htons(9999); // htons is necessary to convert a number to network byte order
// 	if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) 
// 	{
// 		std::cerr << "Failed to bind to port 9999. errno: " << strerror(errno) << std::endl;
// 		exit(EXIT_FAILURE);
// 	}

// 	// Start listening. Hold at most 10 connections in the queue
// 	if (listen(sockfd, 10) < 0) 
// 	{
// 		std::cerr << "Failed to listen on socket. errno: " << strerror(errno) << std::endl;
// 		exit(EXIT_FAILURE);
// 	}
// 	std::cout << "Server is listening on port 9999..." << std::endl;


// 	//Ozan

// 	// Creating an epoll instance
// 	int epoll_fd = epoll_create1(0);
// 	if (epoll_fd == -1)
// 	{
// 		std::cerr << "Failed to create epoll file descriptor" << std::endl;
// 		exit(EXIT_FAILURE);
// 	}

// 	// Add listening-socket to epoll
// 	epoll_event event;
// 	event.events = EPOLLIN;
// 	event.data.fd = sockfd;
// 	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &event);

// 	//Stocking elements
// 	epoll_event events[10];

// 	//End Ozan


// 	//Ozan 2 (while)
// 	while (true)
// 	{
// 		int	n = epoll_wait(epoll_fd, events, 10, -1);

// 		for (int i = 0; i < n; i++)
// 		{
// 			if (events[i].data.fd == sockfd)
// 			{
// 				// New Connection
// 				int		connection = accept(sockfd, nullptr, nullptr);

// 				event.events = EPOLLIN;
// 				event.data.fd = connection;
// 				epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connection, &event);
// 			}
// 			else
// 			{
// 				// Read the request
// 				int		connection = events[i].data.fd;
// 				char	buffer[1024];
// 				ssize_t	bytesRead = recv(connection, buffer, sizeof(buffer) - 1, 0);

// 				buffer[bytesRead] = '\0';
// 				std::string path = buffer;
// 				path = path.substr(path.find('/'), path.size() - path.find('/'));
// 				path = path.substr(0, path.find(' '));

// 				std::string contentType = getContentType(path);
// 				try
// 				{
// 					getFileContent(path, contentType, connection);
// 				}
// 				catch(const std::exception& e)
// 				{
// 					std::cout << e.what() << std::endl;
// 				}
// 				close(connection); //Close connection when his done
				

// 			}
// 		}
// 	}


	// while(true)
	// {
	// 	// Accept a connection
	// 	socklen_t addrlen = sizeof(sockaddr);  
	// 	int connection = accept(sockfd, (struct sockaddr*)&sockaddr, &addrlen);
	// 	if (connection < 0) {
	// 		std::cerr << "Failed to grab connection. errno: " << errno << std::endl;
	// 		exit(EXIT_FAILURE);
	// 	}
	// 	std::cout << "Connection accepted." << std::endl;

	// 	// Receive data from the connection
	// 	char buffer[1024];
	// 	ssize_t bytesRead = recv(connection, buffer, sizeof(buffer) - 1, 0);
	// 	if (bytesRead < 0) 
	// 	{
	// 		std::cerr << "Error reading data. errno: " << strerror(errno) << std::endl;
	// 		return (errno);
	// 	} 
	// 	buffer[bytesRead] = '\0'; // Null-terminate the buffer

	// 	std::string path = buffer;
	// 	std::string content;
	// 	std::string contentType;

	// 	// get http request
	// 	if(!path.empty())
	// 	{
	// 		//get uri
	// 		std::cout << "-----the path before-----\n: " << path;
	// 		std::cout << "\n\n";
	// 		path = path.substr(path.find('/'), path.size() - path.find('/'));
	// 		path = path.substr(0, path.find(' '));
	// 		std::cout << "the path after: " << path << std::endl;
	// 		//get the type file
	// 		contentType = getContentType(path);
	// 	}
	// 	else
	// 		contentType = "/index.html";

	// 	//get the file content
	// 	try
	// 	{
	// 		getFileContent(path, contentType, connection);
	// 	}
	// 	catch(const std::exception& e)
	// 	{
	// 		std::cout << e.what() << std::endl;
	// 	}
	// 	close(connection);
	// 	std::cout << "\n\n";#define MAX_EVENTS 3

	// }
	// close(sockfd);

// 	return 0;
// }

//===============================================================
//===============================================================
//===============================================================
//===============================================================
//===============================================================
//===============================================================

//epoll et global
#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <cerrno>

//creation de socket
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH1 "/tmp/socket1"
#define SOCKET_PATH2 "/tmp/socket2"
#define SOCKET_PATH3 "/tmp/socket3"
// fin creation de socket

#define MAX_EVENTS 3
#define TIMEOUT -1

// Fonction pour gérer et afficher les erreurs
int handle_error(const char *msg, int epoll_fd = -1)
{
	std::cerr << msg << ": " << strerror(errno) << std::endl;
	if (epoll_fd != -1)
		close(epoll_fd);	// Ferme epoll_fd si fourni
	return -1;
}

// Fonction pour configurer epoll avec 3 sockets
int	setup_epoll(int sock1, int sock2, int sock3)
{
	int					epoll_fd; 
	epoll_event			ev;
	std::vector<int>	sockets = {sock1, sock2, sock3};	// Sera remplacer par la structure epoll() de Bastien

	epoll_fd = epoll_create1(1);	// Création d'un file descriptor epoll

	if (epoll_fd == -1)
		return handle_error("Failed to create epoll file descriptor");

	// Initialisation de la structure epoll_event pour surveiller les événements
	memset(&ev, 0, sizeof(ev));
	ev.events = EPOLLIN;	// EPOLLIN pour surveiller la lecture des sockets

	for (int sock : sockets)
	{
		ev.data.fd = sock;	// Associe le socket courant
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock, &ev) == -1)
			return handle_error("Failed to add socket to epoll", epoll_fd);
	}
	return epoll_fd;
}


/**
 * @brief Fonction qui gere les événements des sockets, lit les données et les affiche
 * @param epoll_fd Le file descriptor epoll
*/
void handle_events(int epoll_fd)
{
	epoll_event	events[MAX_EVENTS];		// Tableau pour stocker les événements
	int			nfds;					// Nombre de file descriptors prêts à être lus (prêts pour une opération d'I/O)

	while (true)
	{
		// Attend les événements et les récupère dans le tableau events
		nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, TIMEOUT);
		if (nfds == -1)
		{
			handle_error("epoll_wait error");
			break;
		}
		for (int i = 0; i < nfds; ++i)
		{
			int fd = events[i].data.fd;
			if (events[i].events & EPOLLIN)
			{
				// Si des données sont disponibles pour lecture
				std::cout << "Data available on socket " << fd << std::endl;
				// Appelle ta fonction de lecture ici
			}
		}
	}
	close(epoll_fd); // Ferme epoll_fd une fois fini
}

// Fonction pour créer un socket UNIX local === Pas obligaatoire
int create_unix_socket(const char *path)
{
	int sockfd;

	sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sockfd == -1) {
		std::cerr << "Failed to create socket: " << strerror(errno) << std::endl;
		return -1;
	}

	sockaddr_un addr;
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);

	unlink(path); // Supprime le fichier de socket s'il existe déjà

	if (bind(sockfd, (sockaddr*)&addr, sizeof(addr)) == -1) {
		std::cerr << "Failed to bind socket: " << strerror(errno) << std::endl;
		close(sockfd);
		return -1;
	}
	return sockfd;
}
//==============

// Code de test principal
int main() {
	int sock1 = create_unix_socket(SOCKET_PATH1);
	int sock2 = create_unix_socket(SOCKET_PATH2);
	int sock3 = create_unix_socket(SOCKET_PATH3);
	if (sock1 == -1 || sock2 == -1 || sock3 == -1) {
		return 1;
	}

	// Initialiser epoll avec les sockets
	int epoll_fd = setup_epoll(sock1, sock2, sock3);
	if (epoll_fd == -1) {
		close(sock1); close(sock2); close(sock3);
		return 1;
	}

	// Envoyer un message de test à chaque socket
	sockaddr_un addr;
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, SOCKET_PATH1, sizeof(addr.sun_path) - 1);
	sendto(sock2, "Hello, socket1!", 15, 0, (sockaddr*)&addr, sizeof(addr));
	
	strncpy(addr.sun_path, SOCKET_PATH2, sizeof(addr.sun_path) - 1);
	sendto(sock3, "Hello, socket2!", 15, 0, (sockaddr*)&addr, sizeof(addr));
	
	strncpy(addr.sun_path, SOCKET_PATH3, sizeof(addr.sun_path) - 1);
	sendto(sock1, "Hello, socket3!", 15, 0, (sockaddr*)&addr, sizeof(addr));

	// Gérer les événements pour afficher la réception des messages
	handle_events(epoll_fd);

	// Nettoyer
	close(sock1); close(sock2); close(sock3);
	unlink(SOCKET_PATH1); unlink(SOCKET_PATH2); unlink(SOCKET_PATH3);

	return 0;
}