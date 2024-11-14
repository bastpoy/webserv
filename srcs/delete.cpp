#include "Header.hpp"

std::string httpDeleteResponse(std::string code, std::string contentType, std::string content)
{
	return ("HTTP/1.1 " + code + " \r\n"
			"Content-Type: " + contentType + "\r\n"
			"Content-Length: " + to_string(content.size()) + "\r\n"
			"Connection: close\r\n"
			"\r\n" + content);
}

bool deleteFile(std::string &filePath)
{
    if(!remove(filePath.c_str()))
        return (true);
    return (false);
}

void deleteRequest(std::string &uri, t_serverData *data)
{
    std::string code;
    std::string content;
	std::string filePath = check_location(uri, content, data->location, data);
    std::string response;
	std::string contentType = getContentType(uri);
    //if i have a location
    if(!filePath.empty())
    {
        //if I have an access file
        if(checkAccessFile(code, filePath))
        {
            content = readFile(filePath, data);
            deleteFile(filePath);
        }
    }
    else
    {
        filePath = data->path + uri;
        if(checkAccessFile(code, filePath))
        {
            content = readFile(filePath, data);
            deleteFile(filePath);
        }
    }

	std::cout << "the filePath is: " << filePath << " uri : " << uri << "and the code "<< code << std::endl;
    
	response = httpDeleteResponse(code, contentType, content);
    //send response
	if(send(data->sockfd, response.c_str(), response.size(), 0) < 0)
	{
		std::cout << strerror(errno) << std::endl;
		throw Response::ErrorSendingResponse(); 
	}
    close(data->sockfd);
}