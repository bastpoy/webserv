#ifndef GET_HPP
#define GET_HPP

#include "Header.hpp"

void	getRequest(std::string uri, t_serverData *data);
void	redirRequest(std::map<std::string, std::string>::iterator redir, int fd);
bool    checkAccessFile(std::string &code, std::string &filePath);
std::string check_location(std::string &uri, std::string &content, std::vector<Location> &location, t_serverData *data);
std::string pathLocation(std::string &content, std::string &uri, std::vector<Location>::iterator it, t_serverData *data, std::string root);
std::string getContentType(std::string &path);

#endif