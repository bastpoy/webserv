#include "resHeader.hpp"


//Throw error
const char* resHeader::ErrorOpeningFile::what() const throw()
{
	return("Error opening File");
}

const char* resHeader::ErrorSendingResponse::what() const throw()
{
	return("Error sending response");
}

const char* resHeader::ConfigurationFileLocation::what() const throw()
{
    return("Error in the Location block configuration file");
}

const char* resHeader::ConfigurationFileServer::what() const throw()
{
    return("Error in the Server block configuration file");
}

const char* resHeader::ErrorGetAddrInfo::what() const throw()
{
    return("Error getting the address info");
}

const char* resHeader::ErrorBindAddress::what() const throw()
{
    return("Error binding socket with my address");
}

const char* resHeader::ErrorListening::what() const throw()
{
    return("Error listening on my current socket");
}

const char* resHeader::ErrorCreatingSocket::what() const throw()
{
    return("Error listening on my current socket");
}

const char* resHeader::Error::what() const throw()
{
    return("");
}