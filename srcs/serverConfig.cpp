#include "serverConfig.hpp"

serverConfig::serverConfig()
{}

serverConfig::~serverConfig()
{}

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

void serverConfig::setRedir(int code, std::string domain)
{
    this->redir.insert(std::make_pair(code, domain));
}

void serverConfig::setErrorPage(int code, std::string errorFile)
{
    this->redir.insert(std::make_pair(code, errorFile));
}