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
        

        //setter
        void setSocketfd(int socketfd);

        //functions
        void createListenAddr(httpConfig &config);

    private:
        std::set<int> socketfd;
};

#endif
