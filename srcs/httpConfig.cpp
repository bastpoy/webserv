#include "httpConfig.hpp"

httpConfig::httpConfig()
{}

httpConfig::~httpConfig()
{}

void httpConfig::addServer(serverConfig *server)
{
    this->server.push_back(server);
}


//getter

//setter


//parsing
void httpConfig::getServerAttributs(std::ifstream& file, serverConfig *server)
{
    std::string line;
    size_t pos = 0;

    while(getline(file, line))
    {
        // std::cout << "the line is " << line << std::endl;
        if (line.find("listen ") != std::string::npos)
        {
            pos = line.find("listen ");
            //get the port and convert to int
            server->setPort(atoi(line.substr(pos + strlen("listen "), line.length()).c_str()));
            std::cout << "the port is: " << server->getPort() << std::endl;
        }
        else if(line.find("server_name ") != std::string::npos)
        {
            //get the different name of the server domain
            pos = line.find("server_name ");
            server->setServerName(line.substr(pos + strlen("server_name "), line.length()));
            std::cout << "the server is: " << server->getServerName() << std::endl;
        }
        else if(line.find("root ") != std::string::npos)
        {
            //get the root for serving statick files
            pos = line.find("root ");
            server->setPath(line.substr(pos + strlen("root "), line.length()));
            //print
            std::cout << "the path is: " << server->getPath() << std::endl;
        }
        else if(line.find("return ") != std::string::npos)
        {
            pos = line.find("return ");
            int code = atoi(line.substr(pos + strlen("return "), 3).c_str());
            std::string domain = line.substr(pos + strlen("return ") + 3, line.length());
            server->setRedir(code, domain);

            //print
            std::map<int, std::string>::iterator it = server->getRedir().begin();
            std::cout << "the code is: " << it->first << "\t the domain is: " << it->second <<  std::endl;
        }
        else if(line.find("return ") != std::string::npos)
        if(line.find("}") != std::string::npos)
        {
            std::cout << "end of the server block\n" << std::endl;
            return ;
        }
    }
}

int httpConfig::parseConfig()
{
    httpConfig config;
    std::string line;

    std::ifstream file("./conf/conf.conf");
    if(!file.is_open())
    {
        std::cout << strerror(errno) << std::endl;
        throw resHeader::ErrorOpeningFile();
    }
    while(getline(file, line))
    {
        // std::cout << "the line before is: " << line << std::endl;
        if(line.find("server{") != std::string::npos)
        {
            std::cout << "New server" << std::endl;
            //create a server instance and add it to http class
            serverConfig *server = new serverConfig();
            config.addServer(server);
            //get server attributs
            config.getServerAttributs(file, server);
        }
    }
    return(0);
}