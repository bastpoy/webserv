#include "Error.hpp"

//Error function and close epoll fd
void errorCloseEpollFd(int &epoll_fd, int errCode)
{
    if(errCode != 7) // if its different from epoll_create
        close(epoll_fd);
    if(errCode == 1) // epoll waiting Error function
        std::cerr << "Error epoll waiting: ";
    else if(errCode == 2) // Error creating socket
        std::cerr << "Error to create socket: ";
    else if(errCode == 3) // Error setsocketopt
        std::cerr << "Error adding option to socket: ";
    else if(errCode == 4) // epoll ctl
        std::cerr << "Error controlling socket: ";
    else if(errCode == 5) // Error recv
        std::cerr << "Error receiving data: ";
    else if(errCode == 6) // error accept
        std::cerr << "Error accepting new client: ";
    else if(errCode == 7) // error accept
        std::cerr << "Error creating epoll instance: ";
    std::cout << strerror(errno) << std::endl;
    throw Response::Error();
}
