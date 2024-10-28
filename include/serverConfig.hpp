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
        std::map<int,std::string> &getErrorPage();
        std::map<int,std::string> &getRedir();
        std::vector<locationConfig*> &getLocation();

        //setter
        void setPort(int port);
        void setServerName(std::string server_name);
        void setPath(std::string path);
        void setMaxBody(std::string maxBody);
        void setIndex(std::string index);
        void setErrorPage(int code, std::string errorFile);
        void setRedir(int code, std::string domain);
        void setLocation(locationConfig *location);

        //fill
        void fillPort(std::string line);
        void fillServerName(std::string line);
        void fillPath(std::string line);
        void fillMaxBody(std::string line);
        void fillIndex(std::string line);
        void fillErrorPage(std::string line);
        void fillRedir(std::string line);
        void fillLocation(std::ifstream &file, std::string line);

        //other
        void printConfig();

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