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

//function to display the html page of delete it is custom because i had th efile from the ./www/upload directory, the location where to add and remove files
void displayDeletePage(std::string path, t_serverData *data)
{
    std::string filePath = data->path + path;
    std::string content = readFile(filePath, data);
    std::string contentType = getContentType(filePath);
    std::string pathToUpload = "./www/upload";
    std::string html;
    std::string response;

    //list all the files inside the directory
    std::vector<std::string> files = listDirectory(pathToUpload);
    std::vector<std::string>::iterator it = files.begin();
    //iterate through my file 
    while(it != files.end())
    {
        //add them to the html page
        if(*it != "." && *it != "..")
            html += "\t<li>" + *it + "</li>\n";
        it++;
    }
    html += "</body>\n</html>";
    content += html;
    std::cout << content << std::endl;
    //render the full html page
    response = httpGetResponse("200 Ok", contentType, content);
    if(send(data->sockfd, response.c_str(), response.size(), 0) < 0)
	{
		std::cout << strerror(errno) << std::endl;
		throw Response::ErrorSendingResponse(); 
	}
    close(data->sockfd);
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
        checkAccessFile(code, filePath, data);
        content = readFile(filePath, data);
        deleteFile(filePath);
    }
    else
    {
        filePath = data->path + uri;
        checkAccessFile(code, filePath, data);
        content = readFile(filePath, data);
        deleteFile(filePath);
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

void parseAndDeleteRequest(std::string buffer, t_serverData *data)
{
    std::cout << "DELETE RESPONSE" << std::endl;

    //get the url of the request
    std::string path = buffer.substr(buffer.find('/') + 1, buffer.size() - buffer.find('/'));
    path = path.substr(0, path.find(' '));
    if(path.find("favicon.ico") != std::string::npos)
        close(data->sockfd);
    //if i have a ? inside my url which represent filtering
    else if(path.find("?") != std::string::npos)
        errorPage("501", data);
    // return the data to the client
    deleteRequest(path, data);
    close(data->sockfd);    
}