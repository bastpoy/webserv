#include "serverAddr.hpp"

serverAddr::~serverAddr()
{}

//getter

//setter
void serverAddr::setSocketfd(int socketfd)
{
    this->socketfd.insert(socketfd);
}

void serverAddr::createListenAddr(httpConfig &config)
{
    std::vector<serverConfig>::iterator itbeg = config.getServer().begin();
    
    //creating the poll
    int epoll_fd = epoll_create(10);
    if (epoll_fd == -1) {
        std::cerr << "Failed to create epoll instance: " << strerror(errno) << std::endl;
        throw resHeader::Error();
    }
    while(itbeg != config.getServer().end())
    {
        //creation addrinfo struc to stock my addrinfo informations
        struct sockaddr_in addr;
        
        //I create my socket
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) {
            std::cout << "Failed to create socket: " << strerror(errno) << std::endl;
            throw resHeader::ErrorCreatingSocket();
        }
        //print address and port
        std::cout << itbeg->getPort() << "-" << itbeg->getServerName() << "-" << std::endl;

        //fill my sockaddr_in addr with the result of getaddrinfo
        addr.sin_family = AF_INET;
        addr.sin_port = htons(atoi(itbeg->getPort().c_str()));
        if(inet_pton(AF_INET, itbeg->getServerName().c_str(), &addr.sin_addr.s_addr) < 0)
        {
            std::cout << "INET_PTON: " << strerror(errno) << std::endl;
            throw resHeader::Error();
        }

        //bind my socket with the current fill sockaddr_in
        if (bind(sockfd, (struct sockaddr*)&addr, sizeof(sockaddr)) < 0) 
        {
            std::cout << "BIND: "<< strerror(errno) << " ";
            throw resHeader::Error();
        }

        //listen on the current socket created
        if (listen(sockfd, 10) < 0) 
        {
            std::cout << "LISTEN: " << strerror(errno) << " ";
            throw resHeader::Error();
        }


        //create an epoll event for every socketfd
        struct epoll_event event;
        event.events = EPOLLIN; // Monitor for input events
        event.data.fd = sockfd;

        //add the epoll event to my epoll_fd instance
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &event) == -1) {
            std::cerr << "Failed to add socket to epoll: " << strerror(errno) << std::endl;
            throw resHeader::Error();
        }
        
        itbeg++;
    }
    const int MAX_EVENTS = 10;
    struct epoll_event events[MAX_EVENTS];

    while (true) {
        //epollwait return a number corresponding to all the files descriptor
        int num_fds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (num_fds == -1) {
            std::cerr << "epoll_wait error: " << strerror(errno) << std::endl;
            throw resHeader::Error();
        }
        // I iterate through each fd
        for (int i = 0; i < num_fds; ++i) 
        {
            int fd = events[i].data.fd;
            // check if my fd is equal to a socket for handcheck
            if(this->socketfd.find(fd) != this->socketfd.end())
            {
                // listen to add new fd to my
                struct sockaddr_in client_addr;
                socklen_t client_addr_len = sizeof(client_addr);

                int client_fd = accept(fd, (struct sockaddr*)&client_addr, &client_addr_len);
                if (client_fd == -1) {
                    std::cerr << "accept() failed: " << strerror(errno) << std::endl;
                    throw resHeader::Error();                    
                }
                // add new fd to my epoll instance
                struct epoll_event client_event;
                client_event.events = EPOLLIN;
                client_event.data.fd = client_fd;
                if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event) == -1)
                {
                    std::cerr << "Failed to add socket to epoll: " << strerror(errno) << std::endl;
                    throw resHeader::Error();
                }
                
            }
            //Connection already etablish 
            else
            {

            }
        }
    }

}
