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
		std::string id = buffer.substr(pos + 11, 8);
		return (id);
	}
	return("");
}

bool check_cookie_validity(Cookie &cookie, std::string id)
{
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

std::string display_user_connection(Cookie &cookie, t_serverData *data, std::string response)
{
	std::string html;
	std::string id = get_cookie_id(data->buffer);
	if(id.size() && check_cookie_validity(cookie, id))
	{
		std::pair<std::string, t_session> session_id = cookie.get_session_id(id);
		if(!session_id.first.empty())
		{
			html =    
				"\t<div class='user-info'>\n"
				"\t\t<h2>User Connection Details</h2>\n"
				"\t\t<p><strong>Email:</strong> " + session_id.second.credentials.second + " </p>\n"
				"\t\t<p><strong>Password:</strong> " + session_id.second.credentials.first + " </p>\n"
				"\t</div>\n" ;
				// "</body>\n</html>";

				// size_t pos = response.find("</body>\n</html>");
				size_t pos = response.find("</body>");

				if(pos != std::string::npos)
				{
					response = response.insert(pos, html);
				}
				pos = response.find("Content-Length: ");
				if(pos != std::string::npos)
				{
					pos += 16;
					size_t pos1 = response.find("\r\n", pos);
					if(pos != std::string::npos)
					{
						int length = response.size(); 
						std::string lalength = to_string(length);
						response.erase(pos, pos1 - pos);
						response.insert(pos, lalength);
					}
				}
		}
	}
	return (response);
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

	std::map<std::string, std::string>::iterator it = values.begin();
	if(it == values.end())
		errorPage(NULL, "400", data);
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
