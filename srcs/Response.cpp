#include "Header.hpp"

std::string Response::sendResponse(std::string statusCode, std::string contentType, std::string content, t_serverData *data)
{
	std::string	response;
	response = "HTTP/1.1 " + statusCode + " \r\n";
	response += "Content-Type: " + contentType + " \r\n";
	response += "Content-Length: " + to_string(content.size()) + "\r\n";
    if (is_keep_alive(data->header))
    {
        response += "Connection: keep-alive\r\n";
    }
    else
    {
        response += "Connection: close\r\n";
    }
	response += "\r\n" + content;

	if(send(data->sockfd, response.c_str(), response.size(), 0) < 0)
	{
		std::cout << strerror(errno) << std::endl;
		throw Response::ErrorSendingResponse(); 
	}
	throw Response::Error(); 
}

std::string httpGetResponse(std::string code, std::string contentType, std::string content, t_serverData *data)
{
	//make the header response
    std::string response = "HTTP/1.1 " + code + " \r\n" ;
    response += "Content-Type: " + contentType + "\r\n";
    response += "Content-Length: " + to_string(content.size()) + "\r\n";
    if (is_keep_alive(data->header))
    {
        response += "Connection: keep-alive\r\n";
    }
    else
    {
        response += "Connection: close\r\n";
    }
    std::cout << BLUE << response << RESET << std::endl;
    response += "\r\n" + content;
    return (response);
}

std::string httpGetResponseDownload(std::string code, std::string contentType, std::string content, t_serverData *data)
{
    //make the header response
    std::string response = "HTTP/1.1 " + code + " \r\n" ;
    response += "Content-Type: " + contentType + "\r\n";
    response += "Content-Length: " + to_string(content.size()) + "\r\n";
    response += "Content-Disposition: attachment\r\n";
    if (is_keep_alive(data->header))
    {
        response += "Connection: keep-alive\r\n";
    }
    else
    {
        response += "Connection: close\r\n";
    }
    response += "\r\n" + content;
    return (response);
}

void redirRequest(std::string location, int fd, t_serverData *data)
{
	std::string response = "HTTP/1.1 302 Found \r\n"
							"Location: " + location + "\r\n"
							"Content-Type: text/html\r\n"
							"Content-Length: 0 \r\n";
    if (is_keep_alive(data->header))
    {
        response += "Connection: keep-alive\r\n\r\n";
    }
    else
    {
        response += "Connection: close\r\n\r\n";
    }
	if(send(fd, response.c_str(), response.size(), 0) < 0)
	{
		std::cout << strerror(errno) << std::endl;
		std::cout << "Error redirection: " << strerror(errno) << std::endl;
	}
    // close(fd);
}

void httpPostResponse(std::string code , std::string contentType, std::string content, t_serverData *data, Cookie &cookie, std::string id)
{
	//build the http header response
	std::string response = "HTTP/1.1 " + code + " \r\n"
							"Content-Type: " + contentType + "\r\n";

    std::pair<std::string, t_session> session = cookie.get_session_id(id);
    //I check if i have a cookie with the current id pass
    if(!session.first.empty())
    {
        //fill my credentials in the response header
        response += "Set-Cookie: id=" + session.first + 
                    "; Expires=" + manageDate(session.second.expireDate) +
                    "; password=" + session.second.credentials.first +
                    "; Path=/";
        response += "; email=" + session.second.credentials.second + "\r\n";
    }
    if (is_keep_alive(data->header))
    {
        response += "Connection: keep-alive\r\n";
    }
    else
    {
        response += "Connection: close\r\n";
    }
	response +=	"Content-Length: " + to_string(content.size()) + "\r\n"
				"\r\n" + content;
	//send response
	if(send(data->sockfd, response.c_str(), response.size(), 0) < 0)
	{
		std::cout << strerror(errno) << std::endl;
		throw Response::ErrorSendingResponse(); 
	}
}

const char* Response::ErrorOpeningFile::what() const throw()
{
	return("Error opening File");
}

const char* Response::ErrorSendingResponse::what() const throw()
{
	return("Error sending response");
}

const char* Response::ConfigurationFileLocation::what() const throw()
{
	return("Error in the Location block configuration file");
}

const char* Response::ConfigurationFileServer::what() const throw()
{
	return("Error in the Server block configuration file");
}

const char* Response::Error::what() const throw()
{
	return("");
}

const char* Response::ErrorBodyPostRequest::what() const throw()
{
	return("Error format http POST body");
}

const char* Response::ErrorMaxBody::what() const throw()
{
	return("Error in the syntax of the max body");
}

const char* Response::responseOk::what() const throw()
{
	return("");
}