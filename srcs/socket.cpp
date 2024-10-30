// #include "Socket.hpp"

// Socket::Socket(int domain, int service, int protocol, int port, u_long interface): _domain(domain), _service(service), _protocol(protocol), _port(port), _interface(interface)
// {
// 	_socketFileDescriptor = socket(_domain, _service, _protocol);
// 	if (_socketFileDescriptor < 0)
// 	{
// 		std::cout << "Error: can't create the socket" << std::endl;
// 		exit(EXIT_FAILURE);
// 	}

// 	_address.sin_family = _domain;
// 	_address.sin_port = htons(port);
// 	_address.sin_addr.s_addr = htonl(interface);

// 	int opt = 1;
// 	if (setsockopt(_socketFileDescriptor, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
// 	{
// 		std::cout << "Error: can't configure SO_REUSEADDR's option" << std::endl;
// 		close(_socketFileDescriptor);
// 		exit(EXIT_FAILURE);
// 	}
// }

// Socket::~Socket()
// {
// 	closeSocket();
// }

// void	Socket::bindSocket()
// {
// 	if (bind(_socketFileDescriptor, (struct sockaddr*)&_address, sizeof(_address)) < 0)
// 	{
// 		std::cout << "Error: can't bind the socket to the address" << std::endl;
// 		close(_socketFileDescriptor);
// 		exit(EXIT_FAILURE);
// 	}
// }

// void	Socket::listenSocket(int backlog)
// {
// 	if (listen(_socketFileDescriptor, backlog) < 0)
// 	{
// 		std::cout << "Error: can't put sockt in listening mod" << std::endl;
// 		close(_socketFileDescriptor);
// 		exit(EXIT_FAILURE);
// 	}
// }

// int	Socket::acceptConnection()
// {
// 	int	clientSocket = accept(_socketFileDescriptor, NULL, NULL);
// 	if (clientSocket < 0)
// 		std::cout << "Error : can't accept the connexion" << std::endl;
// 	return (clientSocket);
// }

// void	Socket::closeSocket()
// {
// 	if (close(_socketFileDescriptor) < 0)
// 		std::cout << "Error: can't close the socket";
// }

// ssize_t	Socket::sendData(int clientSocket, std::string& data)
// {
// 	ssize_t	bytesSent = send(clientSocket, data.c_str(), data.size(), 0);
// 	if (bytesSent < 0)
// 		std::cout << "Error: can't send datas" << std::endl;
// 	return (bytesSent);
// }

// ssize_t	Socket::receiveData(int clientSocket, std::string& buffer)
// {
// 	char	recvBuffer[1024];
// 	ssize_t	bytesReceived = recv(clientSocket, recvBuffer, sizeof(recvBuffer) -1, 0);

// 	if (bytesReceived > 0)
// 	{
// 		recvBuffer[bytesReceived] = 0;
// 		buffer = recvBuffer;
// 	} else if (bytesReceived == 0)
// 		std::cout << "Connection has been closed by the client" << std::endl;
// 	else
// 		std::cout << "Error: can't receive datas" << std::endl;
// 	return (bytesReceived);
// }

// int	Socket::getSocketFd() const
// {
// 	return (_socketFileDescriptor);
// }