#include "Header.hpp"

bool check_cookie_validity(t_serverData *data)
{
    //i am checking if my cookie date session is still superior to my actual date
    time_t actualTime = time(NULL);

    if(data->session)
    {
        std::cout << "actual time: " << actualTime << " and cookies time: " << data->session->expireDate << std::endl;
        if(data->session->expireDate < actualTime)
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
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    
    return tmp_s;
}

std::string manageDate(time_t current_time)
{
    struct tm *local_time = localtime(&current_time);
    char buffer[80];

    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S %Z", local_time);
    std::cout << buffer << std::endl;
    return (buffer);
}

void newSessionCookie(std::map<std::string, std::string> values, t_serverData *data)
{
    // creating a cookie session and save client credentials
    t_session *session = new t_session();
    data->session = session;

    //add all atributes to my session
    session->id = gen_random(8);
    session_id++;
    session->expireDate = time(NULL) + 360;
    std::cout << session->expireDate << std::endl;
    manageDate(session->expireDate);
    session->is_valid = 0;
    //adding email and password to the client session
    std::map<std::string, std::string>::iterator it = values.begin();
    while(it != values.end())
    {
        std::pair<std::string, std::string> pair = *it;
        data->session->credentials.insert(pair);
        it++;
    }
}

void putDataSession(std::map<std::string, std::string> values, t_serverData *data)
{
    (void)data;
    // write all my connection data inside a file
    std::map<std::string, std::string>::iterator it = values.begin();

	int fd = open("./www/data/session/data.txt", O_WRONLY | O_APPEND | O_CREAT, 0644);
	if(fd < 0)
	{
		std::cout << "Error during opening file:" << strerror(errno) << std::endl;
		throw Response::Error();
	}
	while(it != values.end())
	{
		write(fd, "\"", 1);
		write(fd, it->first.c_str(), it->first.size());
		write(fd, "\" : \"", 5);
		write(fd, it->second.c_str(), it->second.size());
		it++;
		if(it != values.end())
			write(fd, "\",\n", 3);
		else
			write(fd, "\"\n", 2);
	}
	close(fd);
}
