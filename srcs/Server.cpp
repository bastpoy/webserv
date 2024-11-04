#include "Header.hpp"


std::string check_location(std::string &filePath, std::vector<Location> &location, t_serverData *data)
{
    std::vector<Location>::iterator it = location.begin();

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

void sendData(std::string &uri , std::string &contentType, t_serverData *data)
{
    std::vector<Location>location = data->location;
    //root de server
    std::string defaultPath = data->path;
    std::string filePath; // Change this to your file path
    std::string locationPath;

    //if there is a index specify in the server and not extension in the path
    if(!data->index.empty() && !isExtension(uri))
        filePath = defaultPath + data->index;
    else
    {
        filePath = defaultPath + uri;
    }
    // check if there is a location path
    locationPath = check_location(uri, data->location, data);
    if(!locationPath.empty())
    {
        std::cout << "a location\n";
        filePath = locationPath;
    }

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
void getFileContent2(std::string &uri , std::string &contentType, int connection)
{
	std::string	content;
	std::string	code = "200 OK";
	std::string	filePath = "." + uri;

	if(access(filePath.c_str(), F_OK) != 0)
	{
		std::cout << uri <<RED ": Fichier introuvable\n" RESET;
		code = "404 Not Found";
		uri = "/www/error/error404.html";
		contentType = "text/html";
	}
	else if (access(filePath.c_str(), R_OK) != 0)
	{
		std::cout << uri <<YELLOW ": Accès refusé (pas de droits de lecture)\n" RESET;
		code = "403 Forbidden";
		uri = "/www/error/error403.html";
		contentType = "text/html";
	}
	else
	{
		code = "200 OK";

	}
	filePath = "." + uri; // Change this to your file path
	
	// content = readFile(filePath);

	std::ifstream file(filePath.c_str(), std::ios::binary);
	if(!file.is_open())
	{
		std::cout << filePath << ": ";
		throw Response::ErrorOpeningFile();
	}
	content = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());


	std::cout << "contenu: " << content << std::endl;
	std::string response = "HTTP/1.1 " + code + " \r\n"
						// "Content-Type: text/html\r\n"
						"Content-Type: " + contentType + "\r\n"
						"Content-Length: " + to_string(content.size()) + "\r\n"
						"Connection: close\r\n"
						"\r\n" + content;
	if(send(connection, response.c_str(), response.size(), 0) < 0)
	{
		std::cout << strerror(errno) << std::endl;
		throw Response::ErrorSendingResponse(); 
	}
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
    //si je n'ai pas d'extensions donc pas de fichiers jer ajoute un backslach
    else
    {
        if(path.size() == 0 || path.at(path.size() - 1) != '/')
            path += "/";
        
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
        // else I respond 
        else
        {
            std::cout << "GET RESPONSE" << std::endl;
            //get the url of the request

            std::string path = buffer.substr(buffer.find('/') + 1, buffer.size() - buffer.find('/'));
            path = path.substr(0, path.find(' '));
            
            // get the type of the request file
            std::string contentType = getContentType(path);
            // return the data to the client
            sendData(path, contentType, data);
        }
    }
    else
        std::cout << "404 not found" << std::endl; 
    return(false);
}
