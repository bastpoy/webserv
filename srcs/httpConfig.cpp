#include "httpConfig.hpp"

httpConfig::httpConfig()
{}

httpConfig::~httpConfig()
{}

void httpConfig::addServer(serverConfig *server)
{
    this->server.push_back(server);
}

int httpConfig::getServerAttributs(std::ifstream file)
{
    std::string line;

    while(getline(file, line))
    {
        
    }
}

int httpConfig::parseConfig()
{
    httpConfig config;
    std::string line;

    std::ifstream file("../conf/conf.conf");
    if(!file.is_open())
    {
        std::cout << "Fail to open file" << std::endl;
        return (1);
    }
    while(getline(file, line))
    {
        if(line.find("server{"))
        {
            //create a server instance and add it to http class
            serverConfig *server = new serverConfig();
            config.addServer(server);
            //get server attributs
        }
    }
}