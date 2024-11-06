#include "serverConfig.hpp"


//basic
serverConfig::serverConfig()
{
    this->isfree = true;
}

serverConfig::~serverConfig()
{}


//getter
std::string serverConfig::getPort() const
{
    return (this->port);
}

std::string serverConfig::getServerName() const
{
    return (this->server_name);
}

std::string serverConfig::getPath() const
{
    return (this->path);
}

std::string serverConfig::getMaxBody() const
{
    return (this->maxBody);
}

std::string serverConfig::getIndex() const
{
    return (this->index);
}


std::map<int,std::string> &serverConfig::getRedir()
{
    return (this->redir);
}

std::map<int,std::string> &serverConfig::getErrorPage()
{
    return (this->errorPage);
}

std::vector<locationConfig> &serverConfig::getLocation()
{
    return (this->location);
}

//setter
void serverConfig::setPort(std::string port)
{
    this->port = port;
}

void serverConfig::setServerName(std::string server_name)
{
    this->server_name = server_name;
}

void serverConfig::setPath(std::string path)
{
    this->path = path;
}

void serverConfig::setMaxBody(std::string maxBody)
{
    this->maxBody = maxBody;
}

void serverConfig::setIndex(std::string index)
{
    this->index = index;
}

void serverConfig::setLocation(locationConfig &location)
{
    this->location.push_back(location);
}


void serverConfig::setRedir(int code, std::string domain)
{
    this->redir.insert(std::make_pair(code, domain));
}

void serverConfig::setErrorPage(int code, std::string errorFile)
{
    this->errorPage.insert(std::make_pair(code, errorFile));
}

//fill
void serverConfig::fillPort(std::string line)
{
    size_t pos = line.find("listen ");
    //get the port and convert to int
    this->setPort(line.substr(pos + strlen("listen "), line.length()).c_str());
    std::cout << "the port is: " << this->getPort() << std::endl;
}

void serverConfig::fillServerName(std::string line)
{
    size_t pos = line.find("server_name ");
    this->setServerName(line.substr(pos + strlen("server_name "), line.length() - (pos + strlen("server_name "))));
    //print
    std::cout << "the server is: " << this->getServerName() << std::endl;
}

void serverConfig::fillPath(std::string line)
{
    size_t pos = line.find("root ");
    this->setPath(line.substr(pos + strlen("root "), line.length() -(pos + strlen("root "))));
    //print
    std::cout << "the path is: " << this->getPath() << std::endl;
}

void serverConfig::fillMaxBody(std::string line)
{
    size_t pos = line.find("client_max_body_size ");
    this->setMaxBody(line.substr(pos + strlen("client_max_body_size "), line.length() - (pos + strlen("client_max_body_size "))));
    //print
    std::cout << "the maxBody is: " << this->getMaxBody() << std::endl;
}

void serverConfig::fillIndex(std::string line)
{
    size_t pos = line.find("index ");
    this->setIndex(line.substr(pos + strlen("index "), line.length() - (pos + strlen("index "))));
    //print
    std::cout << "the index is: " << this->getIndex() << std::endl;
}

void serverConfig::fillErrorPage(std::string line)
{
    size_t pos = line.find("error_page ");
    int code = atoi(line.substr(pos + strlen("error_page "), 3).c_str());
    std::string domain = line.substr(pos + strlen("error_page ") + 3, line.length());
    this->setErrorPage(code, domain);

    //print
    std::map<int, std::string>::iterator it = this->getErrorPage().begin();
    std::cout << "the errorCode is: " << it->first << "\t the file is: " << it->second <<  std::endl;
}

void serverConfig::fillRedir(std::string line)
{
    size_t pos = line.find("return ");
    int code = atoi(line.substr(pos + strlen("return "), 3).c_str());
    std::string domain = line.substr(pos + strlen("return ") + 3, line.length() - (pos + strlen("return ")));
    this->setRedir(code, domain);

    //print
    std::map<int, std::string>::iterator it = this->getRedir().begin();
    std::cout << "the code is: " << it->first << "\t the domain is: " << it->second <<  std::endl;
}

void serverConfig::fillLocation(std::ifstream &file, std::string line)
{
    locationConfig location;
    std::cout << "\nNew location" << std::endl;

    //get the path of the location
    location.fillPath(line);
    //get all the information
    while(getline(file, line))
    {
        //fill the index of the file to serve
        if(line.find("autoindex ") != std::string::npos)
            location.fillAutoIndex(line);
        //fill the maxbody size
        else if(line.find("client_max_body_size ") != std::string::npos)
            location.fillMaxBody(line);
        //fill the autoindex
        else if(line.find("index ") != std::string::npos)
            location.fillIndex(line);
        //fill a redirection 
        else if(line.find("return ") != std::string::npos)
            location.fillRedir(line, this);
        //fill an error code
        else if(line.find("error_page ") != std::string::npos)
            location.fillErrorPage(line, this);
        //get the end of location section
        else if(line.find("}") != std::string::npos)
        {
            this->setLocation(location);
            return;
        }
        //if there is not all the above things return an error
        else
            throw resHeader::ConfigurationFileLocation();
    }
    //add my location to my server
}

//other

void serverConfig::printConfig()
{
    std::vector<locationConfig>::iterator itbeg = this->location.begin();
    std::vector<locationConfig>::iterator itend = this->location.end();
    
    //print all server attributs
    if(!this->getPort().empty())
        std::cout << "listen " << this->getPort() << std::endl;
    if(!this->getServerName().empty())
        std::cout << "server_name " << this->getServerName() << std::endl;
    if(!this->getPath().empty())
        std::cout << "root " <<this->getPath() << std::endl;
    if(!this->getMaxBody().empty())
        std::cout << "client_max_body_size " << this->getMaxBody() << std::endl;
    if(!this->getIndex().empty())
        std::cout << "index " << this->getIndex() << std::endl;
    if(this->getErrorPage().begin()->first) 
        std::cout << "error_page " << this->getErrorPage().begin()->first << " " << this->getErrorPage().begin()->second << std::endl;
    if(this->getRedir().begin()->first) 
        std::cout << "return " << this->getRedir().begin()->first << " " << this->getRedir().begin()->second << std::endl;
    std::cout << std::endl;

    //print every location of my current server
    while(itbeg != itend)
    {
        (itbeg)->printConfig();
        itbeg++;
    }
}