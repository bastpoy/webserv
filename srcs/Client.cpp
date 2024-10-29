#include "../includes/Client.hpp"

Client::Client(int socket_fd, const struct sockaddr_storage& addr): _socket_fd(socket_fd), _remote_addr(addr) {}

Client::~Client()
{

}

void	Client::connect()
{

}

ssize_t	Client::receive()
{

}

ssize_t	Client::send()
{

}

void	Client::close()
{

}