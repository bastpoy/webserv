#include "Header.hpp"

void Response::setStatusCode()
{
	//TODO
}

void Response::addHeader()
{
	//TODO
}

void Response::setBody()
{
	//TODO
}

void Response::generateResponse()
{
	//TODO
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