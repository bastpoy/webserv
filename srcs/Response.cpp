#include "Header.hpp"

std::string Response::sendResponse(std::string statusCode, std::string contentType, int fd)
{
	std::string	response;
	response = "HTTP/1.1 " + statusCode + " \r\n";
	response += "Content-Type: " + contentType + " \r\n";
	response += "Content-Length: " + std::to_string(response.size()) + "\r\n";
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

const char* Response::ErrorGetAddrInfo::what() const throw()
{
	return("Error getting the address info");
}

const char* Response::ErrorBindAddress::what() const throw()
{
	return("Error binding socket with my address");
}

const char* Response::ErrorListening::what() const throw()
{
	return("Error listening on my current socket");
}

const char* Response::ErrorCreatingSocket::what() const throw()
{
	return("Error listening on my current socket");
}

const char* Response::Error::what() const throw()
{
	return("");
}