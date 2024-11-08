#ifndef POST_HPP
#define POST_HPP

#include "Header.hpp"

void		sendPostData(std::string code , std::string contentType, std::string content, t_serverData *data);
int 		getContentLength(std::string header, t_serverData *data);
std::string	getFileName(std::string body);
void		postRequest(std::string buffer, t_serverData *data);

#endif

