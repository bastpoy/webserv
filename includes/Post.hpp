#ifndef POST_HPP
#define POST_HPP

#include "Header.hpp"

void parsePostBody(std::string &body);
void sendPostData(std::string code , std::string contentType, std::string content, t_serverData *data);
void translateJson();
int getContentLength(std::string header, t_serverData *data);
std::string getFileName(std::string body);
#endif

