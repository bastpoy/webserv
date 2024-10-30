#include "httpConfig.hpp"

httpConfig::httpConfig()
{
    std::cout << "creating" << std::endl;
}

httpConfig::~httpConfig()
{

    // std::vector<serverConfig*>::iterator it = server.begin();
    // while(it != server.end())
    // {
    //     std::cout << (*it) << std::endl;
    //     delete (*it);
    //     it++;
    // }
    // server.clear();
}

void httpConfig::addServer(serverConfig &server)
{
    this->server.push_back(server);
}


//getter
std::vector<serverConfig> &httpConfig::getServer()
{
    return(this->server);
}

//setter


//parsing
void httpConfig::getServerAttributs(std::ifstream& file, serverConfig &server)
{
    std::string line;

    while(getline(file, line))
    {
        //fill the port
        if (line.find("listen ") != std::string::npos)
            server.fillPort(line);
        //fill the different name of the server domain
        else if(line.find("server_name ") != std::string::npos)
            server.fillServerName(line);
        //fill the root for serving statick files
        else if(line.find("root ") != std::string::npos)
            server.fillPath(line);
        //fill the maxbody size
        else if(line.find("client_max_body_size ") != std::string::npos)
            server.fillMaxBody(line);
        //fill the index of the file to serve
        else if(line.find("index ") != std::string::npos)
            server.fillIndex(line);
        //fille a redirection 
        else if(line.find("return ") != std::string::npos)
            server.fillRedir(line);
        //fill an error page
        else if(line.find("error_page ") != std::string::npos)
            server.fillErrorPage(line);
        //fill an entire location block
        else if(line.find("location ") != std::string::npos)
            server.fillLocation(file, line);
            // getLocationAttributs(file, server, line);
        else if(line.find("}") != std::string::npos)
        {
            std::cout << "\n\n";
            return ;
        }
    }
}

void httpConfig::parseConfig()
{
    // httpConfig *config;
    std::string line;

    std::ifstream file("./conf/conf.conf");
    if(!file.is_open())
    {
        std::cout << strerror(errno) << std::endl;
        throw resHeader::ErrorOpeningFile();
    }
    while(getline(file, line))
    {
        // fill new server block
        if(line.find("server {") != std::string::npos)
        {
            std::cout << "New server" << std::endl;
            //create a server instance and add it to http class
            serverConfig server;
            //get server attributs
            this->getServerAttributs(file, server);
            this->addServer(server);
            std::cout << "port: "<< server.getPort() << std::endl;
            std::cout << "port: "<<  this->server.begin()->getPort() << std::endl;
        }
    }
}

//other

void httpConfig::printConfig()
{
    std::vector<serverConfig>::iterator itbeg = this->server.begin();
    std::vector<serverConfig>::iterator itend = this->server.end();
    std::cout << "---------PRINTING CONF-----------\n\n";  

    while(itbeg != itend)
    {
        //print server config who call location config
        (itbeg)->printConfig();
        itbeg++;
    }
    std::cout << "\n\n";
}