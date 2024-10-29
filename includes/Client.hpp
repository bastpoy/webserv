#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Header.hpp"

/* 
But : Gérer la connexion avec chaque client de manière individuelle.
Rôle :
Gérer les sockets pour chaque client.
Lire et envoyer des données sur le socket.
Identifier la requête HTTP du client et l’envoyer au gestionnaire de requêtes.
*/
class Client
{
	private:
		int	_socket_fd;
		const struct sockaddr_storage& _addr;
	public:
		Client(int socket_fd, const struct sockaddr_storage& addr);
		~Client();
		void		connect(); // Establishes a connection with the server.
		ssize_t 	receive(); // Reads data from the client socket.
		ssize_t 	send(); // pour envoyer la réponse HTTP au client.
		void		close(); // pour fermer la connexion.
};

# endif