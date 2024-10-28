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
public:
	void	readRequest(); // pour lire la requête envoyée par le client.
	void	sendResponse(); // pour envoyer la réponse HTTP au client.
	void	closeConnection(); // pour fermer la connexion.
};

# endif