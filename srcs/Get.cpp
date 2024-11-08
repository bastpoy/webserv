#include "Header.hpp"

std::string check_location(std::string &filePath, std::vector<Location> &location, t_serverData *data)
{
    std::vector<Location>::iterator it = location.begin();
    if(it == location.end())
        return("");

    // it->getPath => pathLocation
    // it->getIndex => indexLocation
    // it->getRoot => rootLocation
    // rootServer => rootServer
    // indexServer => indexServer
    // uri => path to the current uri

    //modify locationPath to remove the first '/' 
    it->setPath(it->getPath().substr(1, it->getPath().size() - 1));
    std::string finalPath;
    //iterate throught my different server location
    while(it != location.end())
    {
        // if the path is equal to the location path
        if(!it->getPath().empty() && filePath == it->getPath())
        {
            std::cout << "location path: " << it->getPath() << std::endl;
            //I check if I have a root directory
            if(!it->getRoot().empty())
            {
                //add root to the path
                finalPath += it->getRoot() + it->getPath();
            }
            //if no location root
            else
            {
                if(!data->path.empty())
                    finalPath += data->path + it->getPath();
                else
                    finalPath += it->getPath();
            }
            // if i have a file to return inside location
            if(!it->getIndex().empty())
            {
                //path is equal to serverRoot
                finalPath += it->getIndex();
            }
            //else i return the server file
            else
                finalPath += data->index;
            std::cout << "finalPath: " << finalPath << std::endl;
            return(finalPath);
        }
        it++;
    }
    return ("");
}

std::string getContentType(std::string &path) 
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
    //if i dont have exetension i add backslash
    else
    {
        //if size of path = 0 or no '/' at the end
        if(path.size() == 0 || path.at(path.size() - 1) != '/')
            path += "/";
        
    }
    return "text/html"; // Default content type
}

std::string httpGetResponse(std::string code, std::string contentType, std::string content)
{
    //make the header response
    return ("HTTP/1.1 " + code + " \r\n"
            "Content-Type: " + contentType + "\r\n"
            "Content-Length: " + to_string(content.size()) + "\r\n"
            "Connection: close\r\n"
            "\r\n" + content);
}

void checkAccessFile(std::string &code, std::string &filePath)
{
    // check if i can access the current ressource request 
	if(access(filePath.c_str(), F_OK) != 0)
	{
		std::cout << filePath <<RED ": Fichier introuvable\n" RESET;
		code = "404 Not Found";
		filePath = "./www/error/error404.html";
	}
	else if (access(filePath.c_str(), R_OK) != 0)
	{
		std::cout << filePath <<YELLOW ": Accès refusé (pas de droits de lecture)\n" RESET;
		code = "403 Forbidden";
		filePath = "./www/error/error403.html";
	}
	else
	{
		code = "200 OK";
	}
}

void getRequest(std::string &uri, t_serverData *data)
{
    bool autoindex = true;
    std::vector<Location>location = data->location;
    //get the contentType
    std::string contentType = getContentType(uri);
    std::string filePath; // Change this to your file path
    std::string locationPath = check_location(uri, data->location, data);
    //check first if i have a location
    if(!locationPath.empty())
        filePath = locationPath;
    //if not check inside my server block
    else
    {
        std::cout << "no location match" << std::endl;
        // if no root inside my server
        if(data->path.empty())
        {
            forbidden(data);
            throw Response::Error();
        }
        //if a file inside my uri
        if(isExtension(uri))
        {
            // I dont do the download for now i dont know how to do it
            // if i have a file i serve it
            filePath = data->path + uri;
        }
        // if an index inside my server
        else if (!data->index.empty())
        {
            filePath = data->path + uri + data->index;
        }
        //if i have an autoindex 
        else if(!data->autoIndex.empty() && data->autoIndex == "on")
        {
            filePath = data->path + uri;
            autoindex = true;
            std::cout << "i have an autoindex" << std::endl;
        }
        else
        {
            forbidden(data);
            throw Response::Error();
        }
    }
    std::cout << "the path is: " << filePath << " uri : " << uri << std::endl;
    std::string code;

    //check acces of filePath
    checkAccessFile(code, filePath);

    //read the file content 
    std::string content = readFile(filePath);
    // get the type of the request file
    std::string response = httpGetResponse(code, contentType, content);

    //send response
    if(send(data->sockfd, response.c_str(), response.size(), 0) < 0)
    {
        std::cout << strerror(errno) << std::endl;
        throw Response::ErrorSendingResponse(); 
    }
}

void redirRequest(std::map<std::string, std::string>::iterator redir, int fd)
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
    }
}
