#ifndef RESHEADER_HPP
#define RESHEADER_HPP

#include <sys/socket.h>		// For socket functions
#include <netinet/in.h>		// For sockaddr_in
#include <cstdlib>			// For exit() and EXIT_FAILURE
#include <iostream>			// For cout
#include <unistd.h>			// For close
#include <errno.h>			// For errno
#include <string.h>			// For strerror
#include <sstream>			// For string streams
#include <fstream>			// std::ifstream
#include <stdexcept>
#include <map>

class resHeader
{
	public:

		class ErrorOpeningFile : public std::exception{
			public:
				virtual const char* what() const throw();
		};
		class ErrorSendingResponse : public std::exception{
			public:
				virtual const char* what() const throw();
		};

	private:
		// char buffer[1024];
		// std::string path;
		// std::string content;
		// std::string contentType;
};

#endif
