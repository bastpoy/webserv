#include "serverConfig.hpp"

//getter
int serverConfig::getPort() const
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

std::vector<locationConfig*> &serverConfig::getLocation()
{
    return (this->location);
}

std::map<int,std::string> &serverConfig::getRedir()
{
    return (this->redir);
}

std::map<int,std::string> &serverConfig::getErrorPage()
{
    return (this->errorPage);
}


//setter
void serverConfig::setPort(int port)
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

void serverConfig::setLocation(locationConfig *location)
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