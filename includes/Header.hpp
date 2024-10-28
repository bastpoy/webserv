#ifndef HEADER_HPP
# define HEADER_HPP

# include <string>
# include <vector>
# include <exception>
# include <utility>
# include <sys/socket.h> // For socket functions
# include <netinet/in.h> // For sockaddr_in
# include <cstdlib> // For exit() and EXIT_FAILURE
# include <iostream> // For cout
# include <unistd.h> // For close
# include <errno.h> // For errno
# include <string.h> // For strerror
# include <sstream> // For string streams
# include <fstream>      // std::ifstream
# include <stdexcept>
# include <map>
# include <cstddef>
# include "Client.hpp"
# include "ConfigParser.hpp"
# include "Logger.hpp"
# include "Request.hpp"
# include "RequestHandler.hpp"
# include "Response.hpp"
# include "Server.hpp"
# include "Socket.hpp"
# include "VirtualHost.hpp"

/* ------------- COLORS ------------- */
// Reset
#define RESET   "\033[0m"

// Regular Colors
#define BLACK   "\033[0;30m"
#define RED     "\033[0;31m"
#define GREEN   "\033[0;32m"
#define YELLOW  "\033[0;33m"
#define BLUE    "\033[0;34m"
#define MAGENTA "\033[0;35m"
#define CYAN    "\033[0;36m"
#define WHITE   "\033[0;37m"

// Bold Colors
#define BBLACK   "\033[1;30m"
#define BRED     "\033[1;31m"
#define BGREEN   "\033[1;32m"
#define BYELLOW  "\033[1;33m"
#define BBLUE    "\033[1;34m"
#define BMAGENTA "\033[1;35m"
#define BCYAN    "\033[1;36m"
#define BWHITE   "\033[1;37m"

// Background Colors
#define ON_BLACK   "\033[40m"
#define ON_RED     "\033[41m"
#define ON_GREEN   "\033[42m"
#define ON_YELLOW  "\033[43m"
#define ON_BLUE    "\033[44m"
#define ON_MAGENTA "\033[45m"
#define ON_CYAN    "\033[46m"
#define ON_WHITE   "\033[47m"

#endif /* HEADER_HPP */