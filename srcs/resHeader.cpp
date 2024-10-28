#include "resHeader.hpp"

const char* resHeader::ErrorOpeningFile::what() const throw()
{
	return("Error opening File");
}

const char* resHeader::ErrorSendingResponse::what() const throw()
{
	return("Error sending response");
}