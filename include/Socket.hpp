#ifndef SOCKET_HPP
# define SOCKET_HPP

# include "Header.hpp"

/*
But : Gérer les sockets
*/
class Socket
{
	private:
		int					_socket_fd; // File descriptor du socket
		struct sockaddr_in	_address; // Structure d'adresse du socket
		int					_domain; // Domaine du socket (AF_INET pour IPv4)
		int					_service; // Type de service (SOCK_STREAM pour TCP)
		int					_protocol; // Protocole (souvent 0 pour laisser par défaut)
		int					_port; // Port du serveur
		u_long				_interface; // Interface (INADDR_ANY pour toutes les interfaces)

		void	createSocket();
	public:
		void	bindSocket(); // Lier le socket à une adresse et un port
		void	listenSocket(int backlog); // Écouter les connexions entrantes
		void	acceptConnection(); // Accepter une connexion client et retourner son file descriptor

		ssize_t	sendData(int clientSocket, const std::string& data); // Envoyer des données à un client
		ssize_t	receiveddata(int clientSocket, std::string& buffer); // Recevoir des données d'un client

		void	closeSocket(); // Méthode pour fermer le socket

		int		getSocketFd() const; // Getters pour le file descriptor
};

# endif /* SOCKET_HPP */