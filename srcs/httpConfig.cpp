#include "httpConfig.hpp"

httpConfig::httpConfig()
{
    this->isfree = true;
    std::cout << "creating" << std::endl;
}

httpConfig::~httpConfig()
{
    if(isfree)
    {
        std::vector<serverConfig*>::iterator it = server.begin();
        while(it != server.end())
        {
            std::cout << (*it) << std::endl;
            delete (*it);
            it++;
        }
        server.clear();
        isfree = false;
    }
}

void httpConfig::addServer(serverConfig *server)
{
    this->server.push_back(server);
}


//getter
std::vector<serverConfig*> &httpConfig::getServer()
{
    return(this->server);
}

//setter


//parsing
void httpConfig::getLocationAttributs(std::ifstream& file, serverConfig *server, std::string line)
{
    locationConfig *location = new locationConfig();
    std::cout << "\nNew location with line: " << line << std::endl;

    //get the path of the location
    location->fillPath(line);
    //get all the information
    while(getline(file, line))
    {
        //fill the index of the file to serve
        if(line.find("index ") != std::string::npos)
            location->fillIndex(line);
        //fill the maxbody size
        else if(line.find("client_max_body_size ") != std::string::npos)
            location->fillMaxBody(line);
        //fill the autoindex
        else if(line.find("autoindex ") != std::string::npos)
            location->fillAutoIndex(line);
        //fill a redirection 
        else if(line.find("return ") != std::string::npos)
            location->fillRedir(line, server);
        //fill an error code
        else if(line.find("error_page ") != std::string::npos)
            location->fillErrorPage(line, server);
        //get the end of location section
        else if(line.find("}") != std::string::npos)
            return;
        //if there is not all the above things return an error
        else
            throw resHeader::ConfigurationFileLocation();
    }
    //add my location to my server
    server->setLocation(location);
}

void httpConfig::getServerAttributs(std::ifstream& file, serverConfig *server)
{
    std::string line;

    while(getline(file, line))
    {
        //fill the port
        if (line.find("listen ") != std::string::npos)
            server->fillPort(line);
        //fill the different name of the server domain
        else if(line.find("server_name ") != std::string::npos)
            server->fillServerName(line);
        //fill the root for serving statick files
        else if(line.find("root ") != std::string::npos)
            server->fillPath(line);
        //fill the maxbody size
        else if(line.find("client_max_body_size ") != std::string::npos)
            server->fillMaxBody(line);
        //fill the index of the file to serve
        else if(line.find("index ") != std::string::npos)
            server->fillIndex(line);
        //fille a redirection 
        else if(line.find("return ") != std::string::npos)
            server->fillRedir(line);
        //fill an error page
        else if(line.find("error_page ") != std::string::npos)
            server->fillErrorPage(line);
        //fill an entire location block
        else if(line.find("location ") != std::string::npos)
            server->fillLocation(file, line);
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
            serverConfig *server = new serverConfig();
            //get server attributs
            this->getServerAttributs(file, server);
            this->addServer(server);
        }
    }
}

//other

void httpConfig::printConfig()
{
    std::vector<serverConfig*>::iterator itbeg = this->server.begin();
    std::vector<serverConfig*>::iterator itend = this->server.end();
    std::cout << "---------PRINTING CONF-----------\n\n";  

    while(itbeg != itend)
    {
        //print server config who call location config
        (*itbeg)->printConfig();
        itbeg++;
    }
    std::cout << "\n\n";
}