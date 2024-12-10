#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "Header.hpp"

class Response
{
	public:
		static std::string	sendResponse(std::string statusCode, std::string contentType, std::string content, t_serverData *data);

		class ErrorOpeningFile : public std::exception{
			private:
				std::string _msg;
			public:
				explicit ErrorOpeningFile(const std::string &msg):
					_msg("Error opening File: " + msg) {}
				virtual const char* what() const throw();
				virtual ~ErrorOpeningFile() throw() {}
		};

		class ErrorSendingResponse : public std::exception{
			public:
				virtual const char* what() const throw();
		};

		class ConfigurationFileLocation : public std::exception{
			private:
				std::string _msg;
			public:
				explicit ConfigurationFileLocation(const std::string &msg):
					_msg("Location configuration (222) : " + msg) {}
				virtual const char* what() const throw();
				virtual ~ConfigurationFileLocation() throw() {}
		};

		class ConfigurationFileLocationPath : public std::exception{
			public:
				virtual const char* what() const throw();
		};

		class ConfigurationFileServer : public std::exception{
			private:
				std::string _msg;
			public:
				explicit ConfigurationFileServer(const std::string &msg):
					_msg("Server configuration: " + msg) {}
				virtual const char* what() const throw();
				virtual ~ConfigurationFileServer() throw() {}
		};

		class Error : public std::exception{
			public:
				virtual const char* what() const throw();
		};

		class ErrorCreatingSocket : public std::exception{
			private:
				std::string _msg;
			public:
				explicit ErrorCreatingSocket(const std::string &msg):
					_msg("Error creating socket: " + msg) {}
				virtual const char* what() const throw();
				virtual ~ErrorCreatingSocket() throw() {}
		};

		class ErrorBodyPostRequest : public std::exception{
			public:
				virtual const char* what() const throw();
		};

		class ErrorMaxBody : public std::exception{
			public:
				virtual const char* what() const throw();
		};

		class responseOk : public std::exception{
			public:
				virtual const char* what() const throw();
		};
};

#endif /* RESPONSE_HPP */