#include "Header.hpp"

std::string Response::sendResponse(std::string statusCode, std::string contentType, std::string content, t_serverData *data)
{
	std::string	response;
	response = "HTTP/1.1 " + statusCode + " \r\n";
	response += "Content-Type: " + contentType + " \r\n";
	response += "Content-Length: " + to_string(content.size()) + "\r\n";
	response += "\r\n" + content;

	if(send(data->sockfd, response.c_str(), response.size(), 0) < 0)
	{
		std::cout << strerror(errno) << std::endl;
		throw Response::ErrorSendingResponse(); 
	}
	throw Response::Error(); 
}

std::string httpGetResponse(std::string code, std::string contentType, std::string content)
{
	//make the header response
	return ("HTTP/1.1 " + code + " \r\n"
			"Content-Type: " + contentType + "\r\n"
			"Content-Length: " + to_string(content.size()) + "\r\n"
			"Connection: close\r\n"
			"\r\n" + content);
}

std::string httpGetResponseDownload(std::string code, std::string contentType, std::string content)
{
	//make the header response
	return ("HTTP/1.1 " + code + " \r\n"
			"Content-Type: " + contentType + "\r\n"
			"Content-Length: " + to_string(content.size()) + "\r\n"
            "Content-Disposition: attachment\r\n"
            // ; filename=\"" + file + "\"
			"Connection: close\r\n"
			"\r\n" + content);
}

void httpPostResponse(std::string code , std::string contentType, std::string content, t_serverData *data)
{
	//build the http header response
	std::string response = "HTTP/1.1 " + code + " \r\n"
							"Content-Type: " + contentType + "\r\n";
    //if i have a cookie session i return it
    if(data->session)
    {
        //convert my second date into strings
        std::stringstream ss;
        ss << data->session->expireDate;
        std::string ts = ss.str();

        std::map<std::string, std::string>::iterator it = data->session->credentials.begin();
        //fill my credentials in the response header
        while(it != data->session->credentials.end())
        {
            response += "Set-Cookie: id=" + data->session->id + 
                        "; Expires=" + manageDate(data->session->expireDate) +
                        "; password=" + it->second +
                        "; Path=/";
            it++;
            response += "; email=" + it->second + "\r\n";
            it++;
        }
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