#include "serverAddr.hpp"

serverAddr::~serverAddr()
{
    std::vector<struct sockaddr_in*>::iterator it = listenAddr.begin();
    while(it != listenAddr.end())
    {
        delete (*it);
        it++;
    }
    listenAddr.clear();
}

//getter
std::vector<struct sockaddr_in*> &serverAddr::getListenAddr()
{
    return(this->listenAddr);
}

int serverAddr::getSocketFd()const 
{
    return (this->sockfd);
}

//setter
void serverAddr::setListenAddr(struct sockaddr_in *addr)
{
    this->getListenAddr().push_back(addr);
}

void serverAddr::setSockfd(int sockfd)
{
    this->sockfd = sockfd;
}

void serverAddr::createListenAddr(httpConfig config)
{
    std::vector<serverConfig*>::iterator itbeg = config.getServer().begin();

    while(itbeg != config.getServer().end())
    {
        //creation addrinfo struc to stock my addrinfo informations
        struct addrinfo hints, *res;
        struct sockaddr_in *addr = new sockaddr_in;
        
        //I create my socket
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) {
            std::cerr << "Failed to create socket. errno: " << errno << std::endl;
            exit(EXIT_FAILURE);
        }

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET; // IPv4
        hints.ai_socktype = SOCK_STREAM; // TCP

        //convert int to string
        std::stringstream ss;
        ss << (*itbeg)->getPort();

        std::string ip = (*itbeg)->getPath();
        std::string port = ss.str();

        //getaddrinfo to convert my string ip to a network ip
        if(getaddrinfo(ip.c_str(), port.c_str(), &hints, &res) != 0)
        {
            gai_strerror(errno);
            throw resHeader::ErrorGetAddrInfo();
        }
        //fill my sockaddr_in addr with the result of getaddrinfo
        memcpy(addr, res->ai_addr, sizeof(struct sockaddr_in));
        this->setListenAddr(addr);
        //fill my class with the sockfd
        this->setSockfd(sockfd);
        //bind my socket with the current fill sockaddr_in
        if (bind(sockfd, (struct sockaddr*)&addr, sizeof(sockaddr)) < 0) 
        {
            std::cout << strerror(errno) << " ";
            throw resHeader::ErrorBindAddress();
        }
        //listen on the current socket created
        if (listen(sockfd, 10) < 0) 
        {
            std::cout << strerror(errno) << " ";
            throw resHeader::ErrorListening();
        }
        freeaddrinfo(res);
        itbeg++;
    }
}
