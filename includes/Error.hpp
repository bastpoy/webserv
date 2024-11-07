#ifndef ERROR_HPP
#define ERROR_HPP

#include "Header.hpp"

void errorCloseEpollFd(int &epoll_fd, int errCode);
void contentToLarge(std::string size, t_serverData *data);
void badRequest(t_serverData *data);
void internalError(t_serverData *data);

#endif