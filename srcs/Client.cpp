#include "../includes/Client.hpp"

Client::Client(int socket_fd, const struct sockaddr_storage& addr): _socket_fd(socket_fd), _addr(addr), _request(NULL), _response(NULL) {}

Client::~Client()
{
	if (_request)
		delete _request;
	if (_response)
		delete _response;
	close(_socket_fd);
}

bool	Client::clientConnection()
{
	if (connect(_socket_fd, reinterpret_cast<const struct sockaddr*>(&_addr), sizeof(_addr)) == -1)
	{
		std::cout << "Error connecting to client" << std::endl;
		return false;
	}
	return true;
}

ssize_t	Client::receive(char* buffer, size_t buffer_size)
{
	ssize_t	bytes_read = read(_socket_fd, buffer, buffer_size);
	if (bytes_read = -1)
	{
		std::cout << "Error reading from client socket" << std::endl;
		return -1;
	}
	return bytes_read;
}

ssize_t	Client::send(const char* data, size_t data_size)
{
	ssize_t	bytes_written = write(_socket_fd, data, data_size);
	if (bytes_written == -1)
	{
		std::cout << "Error writing to client socket" << std::endl;
		return -1;
	}
	return bytes_written;
}

void	Client::closeConnection()
{
	close(_socket_fd);
}

void	Client::setRequest(Request* request)
{
	_request = request;
}

void	Client::setResponse(Response* response)
{
	_response = response;
}

Request*	Client::getRequest() const
{
	return _request;
}

Response*	Client::getResponse() const
{
	return _response;
}