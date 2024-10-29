#ifndef SERVERSTRUCTADDR_HPP
#define SERVERSTRUCTADDR_HPP

#include "resHeader.hpp"
#include "httpConfig.hpp"

class serverAddr
{
    public:
        //base functions
        ~serverAddr();

        //getter
        std::vector<struct sockaddr_in> &getListenAddr();
        int getSocketFd() const;

        //setter
        void setListenAddr(struct sockaddr_in addr);
        void setSockfd(int sockfd);

        //functions
        void createListenAddr(httpConfig &config);

    private:
        std::vector<struct sockaddr_in> listenAddr;
        int sockfd;
};

#endif
