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

const char*	Response::ErrorOpeningFile::what() const throw()
{
	return _msg.c_str();
}

const char* Response::ErrorSendingResponse::what() const throw()
{
	return("Error sending response");
}

const char* Response::ConfigurationFileLocation::what() const throw()
{
	return("Error in the Location block configuration file");
}

const char* Response::ConfigurationFileLocationPath::what() const throw()
{
	return("Location configuration: Path already exist");
}

const char* Response::ConfigurationFileServer::what() const throw()
{
	return _msg.c_str();
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