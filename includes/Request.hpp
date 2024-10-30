#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "Header.hpp"

/*
But : Représenter une requête HTTP reçue par le serveur.
Rôle :
- Analyser la requête HTTP brute reçue du client.
- Stocker les informations de la requête (méthode HTTP, chemin, en-têtes, corps).
*/
class Request
{
	private:
		std::string	_method;
		std::string	_path;
		std::map<std::string, std::string>	_headers;
		std::string	_body;
	public:
		void	parseRequest(); // pour analyser les données brutes et remplir les attributs de la requête.

//From  RequestHandler
		// void	handleGetRequest();
		// void	handlePostRequest();
		// void	handleDeleteRequest();

		// void	serveStaticFile(); // pour récupérer le contenu des fichiers statiques.
		// void	handleError(); // pour créer une réponse d'erreur (par exemple, 404 Not Found).


};

# endif