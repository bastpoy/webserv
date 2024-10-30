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
		int								_socket_fd;
		const struct sockaddr_storage&	_addr;
		Response						*_response;
		Request							*_request;
	public:
		Client(int socket_fd, const struct sockaddr_storage& addr);
		~Client();
		bool		clientConnection(); // Establishes a connection with the server.
		ssize_t 	receive(char* buffer, size_t buffer_size); // Reads data from the client socket.
		ssize_t 	send(const char* data, size_t data_size); // pour envoyer la réponse HTTP au client.
		void		closeConnection(); // pour fermer la connexion.

		void	setRequest(Request* request);
		void	setResponse(Response* response);
		Request*	getRequest() const;
		Response*	getResponse() const;
};

# endif