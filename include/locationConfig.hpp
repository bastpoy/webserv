#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include "resHeader.hpp"

class locationConfig
{
    public:

        //getter
        std::string getIndex() const;
        std::string getMaxBody() const;
        std::string getAutoIndex() const;
        std::map<int,std::string> &getRedir();
        std::map<int,std::string> &getErrorPage();
        
        //setter
        void setIndex(std::string index);
        void setMaxBody(std::string maxBody);
        void setAutoIndex(std::string autoIndex);
        void setRedir(int code, std::string path);
        void setErrorPage(int errCode, std::string errorPage);

    private:
    
        std::string index;
        std::string maxBody;
        std::string autoindex;
        std::map<int,std::string> redir;
        std::map<int, std::string> errorPage;
};

#endif