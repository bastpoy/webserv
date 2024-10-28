#ifndef SERVER_HPP
# define SERVER_HPP

# include "Header.hpp"

/*
But : Représente le serveur HTTP principal.
Rôle :
Écouter sur un ou plusieurs ports pour les connexions entrantes.
Accepter les connexions des clients et les rediriger vers un gestionnaire de requêtes.
*/
class Server
{
public:
	void	listen();			// pour écouter sur une adresse IP et un port spécifiés.
	void	acceptConnection();	// pour accepter les connexions entrantes.
	void	setup();			// pour la configuration du serveur avec les options données (par exemple, plusieurs configurations de serveur virtuel).

};

# endif