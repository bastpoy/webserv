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
		static std::string	sendResponse(std::string statusCode, std::string contentType, std::string content, t_serverData *data);

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

		class Error : public std::exception{
			public:
				virtual const char* what() const throw();
		};

		class ErrorBodyPostRequest : public std::exception{
			public:
				virtual const char* what() const throw();
		};

		class ErrorMaxBody : public std::exception{
			public:
				virtual const char* what() const throw();
		};

};

#endif /* RESPONSE_HPP */