#include "locationConfig.hpp"




//getter
std::string locationConfig::getPath() const
{
    return (this->path);
}

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
void locationConfig::setPath(std::string path)
{
    this->path = path;
}

void locationConfig::setIndex(std::string index)
{
    this->index = index;
}

void locationConfig::setMaxBody(std::string maxBody)
{
    this->maxBody = maxBody;
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

//fill
void locationConfig::fillPath(std::string line)
{
    size_t pos = line.find("location ") + strlen("location ");
    if(line.find(" {") == std::string::npos)
        throw resHeader::ConfigurationFileLocation(); 
    size_t pos2 = line.find(" {");
    this->setPath(line.substr(pos, pos2 - pos));
    std::cout << "the path: " << this->getPath() << std::endl;
}

void locationConfig::fillIndex(std::string line)
{
    size_t pos = line.find("index ");
    this->setIndex(line.substr(pos + strlen("index "), line.length() - (pos + strlen("index "))));
    //print
    std::cout << "the index is: " << this->getIndex() << std::endl;            
}

void locationConfig::fillMaxBody(std::string line)
{
    size_t pos = line.find("client_max_body_size ");
    this->setMaxBody(line.substr(pos + strlen("client_max_body_size "), line.length() - (pos + strlen("client_max_body_size "))));
    //print
    std::cout << "the maxBody is: " << this->getMaxBody() << std::endl;
}

void locationConfig::fillAutoIndex(std::string line)
{
    size_t pos = line.find("autoindex ");
    this->setAutoIndex(line.substr(pos + strlen("autoindex "), line.length() - (pos + strlen("autoindex "))));
    std::cout << "The auto index is " << this->getAutoIndex() << std::endl;
}

void locationConfig::fillRedir(std::string line, serverConfig *server)
{
    size_t pos = line.find("return ");
    int code = atoi(line.substr(pos + strlen("return "), 3).c_str());
    std::string domain = line.substr(pos + strlen("return ") + 3, line.length() - (pos + strlen("return ")));
    this->setRedir(code, domain);

    //print
    std::map<int, std::string>::iterator it = server->getRedir().begin();
    std::cout << "the code is: " << it->first << "\t the domain is: " << it->second <<  std::endl;
}

void locationConfig::fillErrorPage(std::string line, serverConfig *server)
{
    size_t pos = line.find("error_page ");
    int code = atoi(line.substr(pos + strlen("error_page "), 3).c_str());
    std::string domain = line.substr(pos + strlen("error_page ") + 3, line.length());
    this->setErrorPage(code, domain);

    //print
    std::map<int, std::string>::iterator it = server->getErrorPage().begin();
    std::cout << "the errorCode is: " << it->first << "\t the file is: " << it->second <<  std::endl;
}

//other
void locationConfig::printConfig()
{
    if(!this->getPath().empty())
        std::cout << "location " << this->getPath() << std::endl;
    if(!this->getIndex().empty())
        std::cout << "index " << this->getIndex() << std::endl;
    if(!this->getMaxBody().empty())
        std::cout << "client_max_body_size " << this->getMaxBody() << std::endl;
    if(!this->getAutoIndex().empty())
        std::cout << "autoindex " << this->getAutoIndex() << std::endl;
    if(this->getErrorPage().begin()->first) 
        std::cout << "error_page " << this->getErrorPage().begin()->first << " " << this->getErrorPage().begin()->second << std::endl;
    if(this->getRedir().begin()->first) 
        std::cout << "return " << this->getRedir().begin()->first << " " << this->getRedir().begin()->second << std::endl;    
}