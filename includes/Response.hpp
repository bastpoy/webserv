#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "Header.hpp"

/*
But : Créer la réponse HTTP envoyée au client.
Rôle :
Générer la réponse HTTP en fonction de la requête reçue.
Gérer les codes de statut HTTP (200, 404, etc.), les en-têtes et le corps de la réponse.
*/

/**
 * @brief Toutes les exceptions
*/
class Response
{
	public:
		void	setStatusCode();	// pour définir le code de statut.
		void	addHeader();		// pour ajouter des en-têtes à la réponse.
		void	setBody();			// pour définir le corps de la réponse.
		void	generateResponse();	// pour combiner tous les éléments de la réponse en une chaîne prête à être envoyée au client.

		class ErrorOpeningFile : public std::exception{
			public:
				virtual const char* what() const throw();
		};

		class ErrorSendingResponse : public std::exception{
			public:
				virtual const char* what() const throw();
		};

		class ConfigurationFileLocation : public std::exception{
			public:
				virtual const char* what() const throw();
		};

		class ConfigurationFileServer : public std::exception{
			public:
				virtual const char* what() const throw();
		};

		class ErrorGetAddrInfo : public std::exception{
			public:
				virtual const char* what() const throw();
		};

		class ErrorBindAddress : public std::exception{
			public:
				virtual const char* what() const throw();
		};

		class ErrorListening : public std::exception{
			public:
				virtual const char* what() const throw();
		};

		class ErrorCreatingSocket : public std::exception{
			public:
				virtual const char* what() const throw();
		};
		class Error : public std::exception{
			public:
				virtual const char* what() const throw();
		};
};

#endif /* RESPONSE_HPP */