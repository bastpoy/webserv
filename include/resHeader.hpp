#ifndef RESHEADER_HPP
#define RESHEADER_HPP

#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <arpa/inet.h> //inet_pton
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <cstdlib> // For exit() and EXIT_FAILURE
#include <stdlib.h>
#include <stdio.h>
#include <iostream> // For cout
#include <unistd.h> // For close
#include <errno.h> // For errno
#include <string.h> // For strerror
#include <sstream> // For string streams
#include <fstream>      // std::ifstream
#include <stdexcept>
#include <vector>
#include <map>
#include <set>
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

    private:

};

#endif