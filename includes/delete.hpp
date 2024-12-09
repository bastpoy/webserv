#ifndef DELETE_HPP
#define DELETE_HPP

#include "Header.hpp"

void deleteRequest(std::string &path, t_serverData *data);
void displayDeletePage(std::string path, t_serverData *data);
void parseAndDeleteRequest(std::string buffer, t_serverData *data, std::string typeRequest);

#endif