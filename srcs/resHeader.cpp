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