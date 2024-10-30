#include "serverAddr.hpp"

// serverAddr::~serverAddr()
// {}

//getter
// std::vector<struct sockaddr_in> &serverAddr::getListenAddr()
// {
//     return(this->listenAddr);
// }

// int serverAddr::getSocketFd()const 
// {
//     return (this->sockfd);
// }

//setter
// void serverAddr::setListenAddr(struct sockaddr_in addr)
// {
//     this->getListenAddr().push_back(addr);
// }

// void serverAddr::setSockfd(int sockfd)
// {
//     this->sockfd = sockfd;
// }

// void serverAddr::createListenAddr(httpConfig &config)
// {
// 	std::vector<serverConfig>::iterator itbeg = config.getServer().begin();

// 	while(itbeg != config.getServer().end())
// 	{
// 		//creation addrinfo struc to stock my addrinfo informations
// 		struct sockaddr_in addr;
		
// 		//I create my socket
// 		int sockfd = socket(AF_INET, SOCK_STREAM, 0);
// 		if (sockfd == -1) {
// 			std::cout << "Failed to create socket: " << strerror(errno) << std::endl;
// 			throw resHeader::ErrorCreatingSocket();
// 		}


// 		std::string ip = itbeg->getServerName();
// 		std::string port = itbeg->getPort();
// 		std::cout << port << "-" << ip << "-" << std::endl;

// 		//fill my sockaddr_in addr with the result of getaddrinfo
// 		addr.sin_family = AF_INET;
// 		addr.sin_port = htons(atoi(itbeg->getPort().c_str()));
// 		if(inet_pton(AF_INET, itbeg->getServerName().c_str(), &addr.sin_addr.s_addr) < 0)
// 		{
// 			std::cout << "wrong IP address: " << strerror(errno) << std::endl;
// 			return;
// 		}

// 		// this->setListenAddr(addr);
// 		// //fill my class with the sockfd
// 		// this->setSockfd(sockfd);
// 		//bind my socket with the current fill sockaddr_in
// 		if (bind(sockfd, (struct sockaddr*)&addr, sizeof(sockaddr)) < 0) 
// 		{
// 			std::cout << "BIND: "<< strerror(errno) << " ";
// 			throw resHeader::ErrorBindAddress();
// 		}
// 		//listen on the current socket created
// 		if (listen(sockfd, 10) < 0) 
// 		{
// 			std::cout << strerror(errno) << " ";
// 			throw resHeader::ErrorListening();
// 		}
// 		itbeg++;
// 	}
// }
