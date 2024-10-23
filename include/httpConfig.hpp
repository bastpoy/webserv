#ifndef HTTPCONFIG_HPP
#define HTTPCONFIG_HPP

#include <vector>
#include "serverConfig.hpp"

class httpConfig
{
    public:
        httpConfig();
        ~httpConfig();
        std::vector<serverConfig> getServer();
        void addServer(serverConfig *server);
        
        //parsing
        int parseConfig();
        int getServerAttributs(std::ifstream file);
    private:
        std::vector<serverConfig*> server;
};

#endif