#ifndef GET_HPP
#define GET_HPP

#include "Header.hpp"

void getRequest(std::string &uri, t_serverData *data);
bool redirHeader(std::map<std::string, std::string>::iterator redir, int fd);

#endif