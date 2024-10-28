#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include "resHeader.hpp"
#include "serverConfig.hpp"

class serverConfig;

class locationConfig
{
    public:

        //getter
        std::string getPath() const;
        std::string getIndex() const;
        std::string getMaxBody() const;
        std::string getAutoIndex() const;
        std::map<int,std::string> &getRedir();
        std::map<int,std::string> &getErrorPage();
        
        //setter
        void setPath(std::string path);
        void setIndex(std::string index);
        void setMaxBody(std::string maxBody);
        void setAutoIndex(std::string autoIndex);
        void setRedir(int code, std::string path);
        void setErrorPage(int errCode, std::string errorPage);

        //fill
        void fillPath(std::string line);
        void fillIndex(std::string line);
        void fillMaxBody(std::string line);
        void fillAutoIndex(std::string line);
        void fillRedir(std::string line, serverConfig *server);
        void fillErrorPage(std::string line, serverConfig *server);

        //other
        void printConfig();
        
    private:
    
        std::string path;
        std::string index;
        std::string maxBody;
        std::string autoindex;
        std::map<int,std::string> redir;
        std::map<int, std::string> errorPage;
};

#endif