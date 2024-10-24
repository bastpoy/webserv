#ifndef HTTPCONFIG_HPP
#define HTTPCONFIG_HPP

#include <vector>
#include "serverConfig.hpp"
#include "resHeader.hpp"

class serverConfig;
class httpConfig
{
    public:

        //canonical
        httpConfig();
        ~httpConfig();

        //getter
        std::vector<serverConfig*> getServer();
        
        //setter
        void addServer(serverConfig *server);
        
        //parsing
        int parseConfig();
        void getServerAttributs(std::ifstream& file, serverConfig *server);

    private:
        std::vector<serverConfig*> server;
};

#endif