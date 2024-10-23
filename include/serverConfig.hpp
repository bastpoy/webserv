#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <map>
#include "resHeader.hpp"
#include "httpConfig.hpp"

class serverConfig
{
    public:
        int getPort() const;
        std::string getServerName() const;
        std::string getPath() const;
        std::string getIndex() const; 
        std::map<int,std::string> getRedir() const;

    private:
        int port;
        std::string server_name;  
        std::string path;
        std::string index;
        std::map<int,std::string> redir;

};

#endif