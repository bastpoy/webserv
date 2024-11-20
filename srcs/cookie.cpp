#include "Header.hpp"


//=================
//getter and setter
//=================

std::map<std::string, t_session> Cookie::get_session()
{
    return (this->session);
}

std::pair<std::string, t_session> Cookie::get_session_id(std::string id)
{
    std::map<std::string, t_session>::iterator it = this->session.find(id);

    std::pair<std::string, t_session> m;

    if(it != this->session.end())
        return(*it);
    return m;
}

bool Cookie::remove_session_id(std::string id)
{
    std::map<std::string, t_session>::iterator it = this->session.find(id);
    if (it != this->session.end())
    {
        this->session.erase (it);
        std::cout << "DECONNEXION SUCCESSFULL" << std::endl;
        return (true);
    }
    return(false);
}

void Cookie::add_session(std::pair<std::string, t_session> session)
{
    this->session.insert(session);
}


//=================
// other functions
//=================


std::string get_cookie_id(std::string buffer)
{
    size_t pos = buffer.find("Cookie: id=");
    
    if (pos != std::string::npos)
    {
        //retrieve the id
        std::string id = buffer.substr(pos + 11, 8);
        return (id);
    }
    return("");
}

bool check_cookie_validity(Cookie &cookie, std::string id)
{
    //i am checking if my cookie date session is still superior to my actual date
    time_t actualTime = time(NULL);

    if(!cookie.get_session().empty())
    {
        if(cookie.get_session_id(id).second.expireDate < actualTime)
            return (false);
    }
    else
        return(false);
    return(true);
}

std::string gen_random(const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string temp;
    temp.reserve(len);

    for (int i = 0; i < len; ++i) {
        temp += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    return temp;
}

std::string manageDate(time_t current_time)
{
    struct tm *local_time = localtime(&current_time);
    char buffer[80];

    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S %Z", local_time);
    std::cout << buffer << std::endl;
    return (buffer);
}

std::string newSessionCookie(std::map<std::string, std::string> values, Cookie &cookie, t_serverData *data)
{
    // creating a cookie session and save client info and credentials
    std::pair<std::string, t_session> newSession;
    //add all atributes to my session
    newSession.first = gen_random(8);
    // cookie during 15sec
    newSession.second.expireDate = time(NULL) + 15;
    std::cout << newSession.second.expireDate << std::endl;

    //set the validity to valid
    newSession.second.is_valid = 1;

    //adding email and password to the client session
    std::map<std::string, std::string>::iterator it = values.begin();
    if(it == values.end())
        errorPage("400", data);
    while(it != values.end())
    {
        if(!newSession.second.credentials.first.size())
            newSession.second.credentials.first = it->second;
        else
            newSession.second.credentials.second = it->second;
        it++;
    }
    cookie.add_session(newSession);
    return (newSession.first);
}
