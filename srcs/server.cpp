#include "Header.hpp"

bool isExtension(std::string path)
{
    if(path.find(".html") == std::string::npos &&
        path.find(".css") == std::string::npos &&
        path.find(".js") == std::string::npos &&
        path.find(".png") == std::string::npos &&
        path.find(".jpg") == std::string::npos &&
        path.find(".gif") == std::string::npos &&
        path.find(".ico") == std::string::npos)
        return (false);
    return (true);
}

void sendData(std::string &uri , std::string &contentType, t_serverData *data)
{
    std::string defaultPath = data->path;
    std::string filePath = defaultPath + uri; // Change this to your file path

    //if there is a index specify in the conf file and not extension in the path
    if(!data->index.empty() && !isExtension(filePath))
        filePath = filePath + data->index;
    // check if there is a location path
    // if there is check all the info inside to render files

    std::cout << "the path is: " << filePath <<  " defautl path: " << defaultPath << " uri: " << uri << std::endl;
    std::cout << "the content type: " << contentType << std::endl;
    //read the file content 
    std::string content = readFile(filePath);

    //return the response
    std::string response = "HTTP/1.1 200 OK \r\n"
                            "Content-Type: " + contentType + "\r\n"
                            "Content-Length: " + to_string(content.size()) + "\r\n"
                            "Connection: close\r\n"
                            "\r\n" + content;
    
    if(send(data->sockfd, response.c_str(), response.size(), 0) < 0)
    {
        std::cout << strerror(errno) << std::endl;
        throw Response::ErrorSendingResponse(); 
    }
}

std::string getContentType(const std::string &path) 
{
    std::map<std::string, std::string> contentTypes;

    contentTypes.insert(std::pair<std::string, std::string>(".html", "text/html"));
    contentTypes.insert(std::pair<std::string, std::string>(".css", "text/css"));
    contentTypes.insert(std::pair<std::string, std::string>(".js", "application/javascript"));
    contentTypes.insert(std::pair<std::string, std::string>(".png", "image/png"));
    contentTypes.insert(std::pair<std::string, std::string>(".jpg", "image/jpeg"));
    contentTypes.insert(std::pair<std::string, std::string>(".gif", "image/gif"));

    size_t dotPos = path.find_last_of(".");
    if (dotPos != std::string::npos) {
        std::string extension = path.substr(dotPos);
        if (contentTypes.find(extension) != contentTypes.end()) {
            return contentTypes[extension];
        }
    }
    return "text/html"; // Default content type
}

bool redirHeader(std::map<std::string, std::string>::iterator redir, int fd)
{
    std::string response = "HTTP/1.1 302 Found \r\n"
                            "Location: " + redir->second + "\r\n"
                            "Content-Type: text/html\r\n"
                            "Content-Length: 0 \r\n"
                            "Connection: keep-alive\r\n\r\n";

    std::cout << "the response:\n" << response << std::endl;
    if(send(fd, response.c_str(), response.size(), 0) < 0)
    {
        std::cout << strerror(errno) << std::endl;
        std::cout << "Error redirection: " << strerror(errno) << std::endl;
        return (false);
    }
    return(true);
}

bool redirectRequest(std::string buffer, t_serverData *data) 
{
    std::string firstLine = buffer.substr(0, buffer.find("\n"));
    std::string typeRequest =  firstLine.substr(0, buffer.find(" "));

    if(typeRequest == "GET")
    {
        //if i have a redirection 
        if(data->redir.size())
        {
            std::cout << "GET REDIRECTION " << data->port << " " << data->server_name << std::endl;
            return(redirHeader(data->redir.begin(), data->sockfd));
        }
        // else I retrieve the info if it exists
        else
        {
            std::string contentType;
            std::string path = buffer.substr(buffer.find('/') + 1, buffer.size() - buffer.find('/'));
            std::cout << "GET RESPONSE" << std::endl;
            
            //get the url of the request
            path = path.substr(0, path.find(' '));
            // get the type of the request file
            contentType = getContentType(path);
            // return the data to the client
            sendData(path, contentType, data);
        }
    }
    else
        std::cout << "404 not found" << std::endl; 
    return(false);
}