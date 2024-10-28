#ifndef REQUESTHANDLER_HPP
# define REQUESTHANDLER_HPP

# include "Header.hpp"

/*
But : Traiter les requêtes en fonction de la logique de l’application.
Rôle :
Gérer les différentes méthodes HTTP (GET, POST, DELETE) et sélectionner la réponse appropriée.
Accéder aux fichiers statiques (HTML, CSS, images) si nécessaires, et gérer les erreurs.
*/
class RequestHandler
{
	private:
		std::string	_method;
	public:
	 	//pour traiter les requêtes en fonction de leur méthode.
		void	handleGetRequest();
		void	handlePostRequest();
		void	handleDeleteRequest();

		void	serveStaticFile(); // pour récupérer le contenu des fichiers statiques.
		void	handleError(); // pour créer une réponse d'erreur (par exemple, 404 Not Found).

};

# endif