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
void httpConfig::getLocationAttributs(std::ifstream& file, serverConfig *server)
{
    std::string line;
    size_t pos = 0;
    locationConfig *location = new locationConfig();

    std::cout << "\nNew location" << std::endl;
    //get all the information
    while(getline(file, line))
    {
        if(line.find("index ") != std::string::npos)
        {
            //get the index of the file to serve
            pos = line.find("index ");
            location->setIndex(line.substr(pos + strlen("index "), line.length() - (pos + strlen("index "))));
            //print
            std::cout << "the index is: " << location->getIndex() << std::endl;            
        }
        else if(line.find("client_max_body_size ") != std::string::npos)
        {
            //get the maxbody size
            pos = line.find("client_max_body_size ");
            location->setMaxBody(line.substr(pos + strlen("client_max_body_size "), line.length() - (pos + strlen("client_max_body_size "))));
            //print
            std::cout << "the maxBody is: " << location->getMaxBody() << std::endl;
        }

        else if(line.find("}") != std::string::npos)
        {
            return;
        }
    }
    //add my location to my server
    server->setLocation(location);
}

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
            server->setServerName(line.substr(pos + strlen("server_name "), line.length() - (pos + strlen("server_name "))));
            //print
            std::cout << "the server is: " << server->getServerName() << std::endl;
        }
        else if(line.find("root ") != std::string::npos)
        {
            //get the root for serving statick files
            pos = line.find("root ");
            server->setPath(line.substr(pos + strlen("root "), line.length() -(pos + strlen("root "))));
            //print
            std::cout << "the path is: " << server->getPath() << std::endl;
        }
        else if(line.find("client_max_body_size ") != std::string::npos)
        {
            //get the maxbody size
            pos = line.find("client_max_body_size ");
            server->setMaxBody(line.substr(pos + strlen("client_max_body_size "), line.length() - (pos + strlen("client_max_body_size "))));
            //print
            std::cout << "the maxBody is: " << server->getMaxBody() << std::endl;
        }
        else if(line.find("index ") != std::string::npos)
        {
            //get the index of the file to serve
            pos = line.find("index ");
            server->setIndex(line.substr(pos + strlen("index "), line.length() - (pos + strlen("index "))));
            //print
            std::cout << "the index is: " << server->getIndex() << std::endl;
        }
        else if(line.find("return ") != std::string::npos)
        {
            pos = line.find("return ");
            int code = atoi(line.substr(pos + strlen("return "), 3).c_str());
            std::string domain = line.substr(pos + strlen("return ") + 3, line.length() - (pos + strlen("return ")));
            server->setRedir(code, domain);

            //print
            std::map<int, std::string>::iterator it = server->getRedir().begin();
            std::cout << "the code is: " << it->first << "\t the domain is: " << it->second <<  std::endl;
        }
        else if(line.find("error_page ") != std::string::npos)
        {
            pos = line.find("error_page ");
            int code = atoi(line.substr(pos + strlen("error_page "), 3).c_str());
            std::string domain = line.substr(pos + strlen("error_page ") + 3, line.length());
            server->setErrorPage(code, domain);

            //print
            std::map<int, std::string>::iterator it = server->getErrorPage().begin();
            std::cout << "the errorCode is: " << it->first << "\t the file is: " << it->second <<  std::endl;
        }
        else if(line.find("location ") != std::string::npos)
        {
            getLocationAttributs(file, server);
        }
        else if(line.find("}") != std::string::npos)
        {
            std::cout << "\n\n";
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
        if(line.find("server {") != std::string::npos)
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