#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "resHeader.hpp"
#include "locationConfig.hpp"

class locationConfig;

class serverConfig
{
    public:

        //getter
        int getPort() const;
        std::string getServerName() const;
        std::string getPath() const;
        std::string getMaxBody() const;
        std::string getIndex() const;
        std::vector<locationConfig*> &getLocation();
        std::map<int,std::string> &getRedir();
        std::map<int,std::string> &getErrorPage();

        //setter
        void setPort(int port);
        void setServerName(std::string server_name);
        void setPath(std::string path);
        void setMaxBody(std::string maxBody);
        void setIndex(std::string index);
        void setLocation(locationConfig *location);
        void setRedir(int code, std::string domain);
        void setErrorPage(int code, std::string errorFile);

    private:
        int port;
        std::string server_name;  
        std::string path;
        std::string maxBody;
        std::string index;
        std::map<int, std::string> errorPage;
        std::map<int, std::string> redir;
        std::vector<locationConfig*> location;
};

#endif