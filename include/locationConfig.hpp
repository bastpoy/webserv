#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include "resHeader.hpp"

class locationConfig
{
    public:
        std::string getIndex() const;
        std::map<int,std::string> getRedir() const;

    private:
        std::string index;
        std::map<int,std::string> redir;
};

#endif