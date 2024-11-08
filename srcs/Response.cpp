#include "Header.hpp"

std::string Response::sendResponse(std::string statusCode, std::string contentType)
{
	std::string	response;
	response = "HTTP/1.1 " + statusCode + " \r\n";
	response += "Content-Type: " + contentType + " \r\n";
	response += "Content-Length: " + to_string(response.size()) + "\r\n";
	response += "\r\n";

	return response;
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