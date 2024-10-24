#include "locationConfig.hpp"




//getter
std::string locationConfig::getIndex() const
{
    return (this->index);
}

std::string locationConfig::getMaxBody() const
{
    return (this->maxBody);
}

std::string locationConfig::getAutoIndex() const
{
    return (this->autoindex);
}

std::map<int,std::string> &locationConfig::getRedir()
{
    return (this->redir);
}

std::map<int,std::string> &locationConfig::getErrorPage()
{
    return (this->errorPage);
}


//setter
void locationConfig::setMaxBody(std::string maxBody)
{
    this->maxBody = maxBody;
}

void locationConfig::setIndex(std::string index)
{
    this->index = index;
}

void locationConfig::setAutoIndex(std::string autoIndex)
{
    this->autoindex = autoIndex;
}

void locationConfig::setRedir(int code, std::string domain)
{
    this->redir.insert(std::make_pair(code, domain));
}

void locationConfig::setErrorPage(int code, std::string errorFile)
{
    this->errorPage.insert(std::make_pair(code, errorFile));
}