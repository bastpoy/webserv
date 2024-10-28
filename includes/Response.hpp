#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "Header.hpp"

/*
But : Créer la réponse HTTP envoyée au client.
Rôle :
Générer la réponse HTTP en fonction de la requête reçue.
Gérer les codes de statut HTTP (200, 404, etc.), les en-têtes et le corps de la réponse.
*/
class Response
{
	public:
		void	setStatusCode();	// pour définir le code de statut.
		void	addHeader();		// pour ajouter des en-têtes à la réponse.
		void	setBody();			// pour définir le corps de la réponse.
		void	generateResponse();	// pour combiner tous les éléments de la réponse en une chaîne prête à être envoyée au client.
};

#endif