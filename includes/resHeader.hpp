#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "Header.hpp"

class Response
{
	private:
		// char buffer[1024];
		// std::string path;
		// std::string content;
		// std::string contentType;

	public:

		class ErrorOpeningFile : public std::exception{
			public:
				virtual const char* what() const throw();
		};

		class ErrorSendingResponse : public std::exception{
			public:
				virtual const char* what() const throw();
		};
};

#endif /* RESPONSE_HPP */
