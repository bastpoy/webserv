#include "Header.hpp"

std::string pathLocation(std::string &content, std::string &uri, std::vector<Location>::iterator it, t_serverData *data, std::string root)
{
    std::string filePath;
    //if index in location
    if(!it->getIndex().empty())
    {
        std::cout << "Index in my location" << std::endl;
        filePath = root + uri + it->getIndex();
        content = readFile(filePath, data);
        return filePath;
    }
    //if I have an autoindex in my location
    else if(!it->getAutoIndex().empty() && it->getAutoIndex() == "on")
    {
        filePath = root + uri;
        std::cout << "filepath: " << filePath << std::endl;
        std::vector<std::string> files = listDirectory(filePath);
        content = generateAutoIndexPage(uri, files);
        return root + uri;
    }
    //if index in my server
    else if(!data->index.empty())
    {
        filePath = root + uri + data->index;
        content = readFile(filePath, data);
        return filePath;
    }
    // if autoindex in my server
    else if(!data->autoindex.empty() && data->autoindex == "on")
    {
        filePath = root + uri;
        std::vector<std::string> files = listDirectory(filePath);
        content = generateAutoIndexPage(uri, files);
        return (filePath);
    }
    //if none of that return a 404 not found
    else
    {
        notFound(data);
        throw Response::Error();
    }
}

std::string check_location(std::string &uri, std::string &content, std::vector<Location> &location, t_serverData *data)
{
	std::vector<Location>::iterator it = location.begin();
	if(it == location.end())
		return("");

	//modify locationPath to remove the first '/' 
	it->setPath(it->getPath().substr(1, it->getPath().size() - 1));
	//iterate throught my different server location
	while(it != location.end())
	{
		// if the path matchs the location path
		if(!it->getPath().empty() && uri.find(it->getPath()) != std::string::npos)
		{
			//if file in my request
            if(isExtension(uri))
            {
                // if i have a root in my location
                if(!it->getRoot().empty())
                {
                    content = readFile(it->getRoot() + uri, data);
                    return it->getRoot() + uri;
                }
                // if i have a root inside my server block
                else if(!data->path.empty())
                {
                    content = readFile(data->path + uri, data);
                    return data->path + uri;
                }
                //if no root in my server block
                else
                    notFound(data);
            }
            // if no file in URI
            else
            {
                // if i have a root in the location
                if(!it->getRoot().empty())
                    return pathLocation(content, uri, it, data, it->getRoot());
                else
                {
                    //if i have a root in my server
                    if(!data->path.empty())
                        return pathLocation(content, uri, it, data, data->path);
                    else
                        forbidden(data);
                }
            }
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
	contentTypes.insert(std::pair<std::string, std::string>(".svg", "image/svg+xml"));
	contentTypes.insert(std::pair<std::string, std::string>(".webp", "image/webp"));
	contentTypes.insert(std::pair<std::string, std::string>(".ico", "image/x-icon"));
    contentTypes.insert(std::pair<std::string, std::string>(".pdf", "application/pdf"));
    contentTypes.insert(std::pair<std::string, std::string>(".mp3", "video/mpeg"));
    contentTypes.insert(std::pair<std::string, std::string>(".mp4", "video/mp4"));
    contentTypes.insert(std::pair<std::string, std::string>(".webm", "video/webm"));
    contentTypes.insert(std::pair<std::string, std::string>(".ogg", "video/ogg"));
    contentTypes.insert(std::pair<std::string, std::string>(".doc", "application/msword"));
    contentTypes.insert(std::pair<std::string, std::string>(".docx", "application/msword"));
    contentTypes.insert(std::pair<std::string, std::string>(".xls", "application/vnd.ms-excel"));
    contentTypes.insert(std::pair<std::string, std::string>(".xlsx", "application/vnd.ms-excel"));
    contentTypes.insert(std::pair<std::string, std::string>(".ppt", "application/vnd.ms-powerpoint"));
    contentTypes.insert(std::pair<std::string, std::string>(".pptx", "application/vnd.ms-powerpoint"));
    contentTypes.insert(std::pair<std::string, std::string>(".ppt", "application/vnd.ms-powerpoint"));
    contentTypes.insert(std::pair<std::string, std::string>(".zip", "application/zip"));
    contentTypes.insert(std::pair<std::string, std::string>(".rar", "application/vnd.rar"));
    contentTypes.insert(std::pair<std::string, std::string>(".tar", "application/x-tar"));
    contentTypes.insert(std::pair<std::string, std::string>(".gz", "application/gzip"));
    contentTypes.insert(std::pair<std::string, std::string>(".7z", "application/x-7z-compressed"));
    contentTypes.insert(std::pair<std::string, std::string>(".txt", "text/plain"));
    contentTypes.insert(std::pair<std::string, std::string>(".xml", "application/xml"));
    contentTypes.insert(std::pair<std::string, std::string>(".json", "application/json"));
    contentTypes.insert(std::pair<std::string, std::string>(".csv", "text/csv"));

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

std::string httpGetResponseDownload(std::string code, std::string contentType, std::string content)
{
	//make the header response
	return ("HTTP/1.1 " + code + " \r\n"
			"Content-Type: " + contentType + "\r\n"
			"Content-Length: " + to_string(content.size()) + "\r\n"
            "Content-Disposition: attachment\r\n"
            // ; filename=\"" + file + "\"
			"Connection: close\r\n"
			"\r\n" + content);
}

bool checkAccessFile(std::string &code, std::string &filePath)
{
	// check if i can access the current ressource request 
	if(access(filePath.c_str(), F_OK) != 0)
		errorPage("404", data);
	else if (access(filePath.c_str(), R_OK) != 0)
		errorPage("403", data);
	else
		code = "200 OK";
}

void checkAccessDir(std::string &code, std::string &dirPath, t_serverData *data)
{
	// check if i can access the current ressource request
	struct stat	pathStat;
	if(stat(dirPath.c_str(), &pathStat) != 0)
		errorPage("404", data);
	else if (!S_ISDIR(pathStat.st_mode))
		errorPage("404", data);
	else if (access(dirPath.c_str(), R_OK) != 0)
		errorPage("403", data);
	else
		code = "200 OK";
}

// void getRequest(std::string &uri, t_serverData *data)
// {
// 	std::vector<Location>location = data->location;
// 	//get the contentType
// 	std::string contentType = getContentType(uri);
// 	//root de server
// 	std::string defaultPath = data->path + uri;
// 	std::string filePath; // Change this to your file path
// 	std::string locationPath;
// 	std::string code;

// 	//if there is a index specify in the server and not extension in the path
// 	if(!data->index.empty() && !isExtension(uri))
// 		filePath = defaultPath + data->index;
// 	else
// 		filePath = defaultPath;
// 	// check if there is a location path
// 	locationPath = check_location(uri, data->location, data);
// 	if(!locationPath.empty())
// 		filePath = locationPath;
// 	if (filePath.find(".py") != std::string::npos)
// 		return (CGIHandler::execute(("/www/" + uri).c_str(), data->sockfd));
// 	//check acces of filePath
// 	checkAccessFile(code, filePath);
// 	std::cout << "the path is: " << filePath <<  " default path: " << defaultPath << " uri: " << uri << std::endl;
// 	std::cout << "the content type: " << contentType << std::endl;
// 	//read the file content 
// 	std::string content = readFile(filePath);
// 	// get the type of the request file
// 	std::string response = httpHeaderResponse(code, contentType, content);

// 	//send response
// 	if(send(data->sockfd, response.c_str(), response.size(), 0) < 0)
// 	{
// 		std::cout << strerror(errno) << std::endl;
// 		throw Response::ErrorSendingResponse(); 
// 	}
// }

void getRequest(std::string &uri, t_serverData *data)
{
	std::vector<Location>location = data->location;
    std::string response;
	std::string	content;
	std::string code;
	std::string contentType = getContentType(uri);
    //check if I have a location block that match the query
	std::string filePath = check_location(uri, content, data->location, data);
    bool download = false;

	//check if i dont have a location
	if(filePath.empty())
	{
		// if no root inside my server
		if(data->path.empty())
		{
			forbidden(data);
			errorPage("403", data);
			throw Response::Error();
		}
		if(isExtension(uri))
		{
			filePath = data->path + uri;

			if (filePath.find(".py") != std::string::npos)
			{
				std::cout << BLUE "It's a CGI" RESET << std::endl; // Debug
				checkAccessFile(code, filePath);
				content = CGIHandler::execute(uri.c_str(), code);
			}
            else
                content = readFile(filePath, data);
		}
        // if i have a file to download
        else if(isExtensionDownload(uri))
        {
            filePath= data->path + uri;
            download = true;
            content = readFile(filePath, data);
        }
		// if an index inside my server
		else if (!data->index.empty())
		{
			filePath = data->path + uri + data->index;
            content = readFile(filePath, data);
		}
		else if(!data->autoindex.empty() && data->autoindex == "on")
		{
			filePath = data->path + uri;

			std::vector<std::string> files = listDirectory(filePath);
			content = generateAutoIndexPage(uri, files);
		}
		else
			errorPage("403", data);
	}
	std::cout << "the filePath is: " << filePath << " uri : " << uri << std::endl;

	if (isDirectory(filePath))
		checkAccessDir(code, filePath, data);
	checkAccessFile(code, filePath, data);
	std::string response = httpGetResponse(code, contentType, content);

	if(send(data->sockfd, response.c_str(), response.size(), 0) < 0)
	{
		std::cout << strerror(errno) << std::endl;
		throw Response::ErrorSendingResponse(); 
	}
}

//-------
void getRequest2amandine(std::string &uri, t_serverData *data)
{
	std::string				curDir	=	"./";
	
	std::string				defaultPath	=	uri.find("www") == std::string::npos ? uri: "./" + uri;		// Root of server
	std::string				filePath;								// Change this to your file path
	std::string				locationPath;
	std::string				code;
	std::string				content;								// Html Content
	std::string				response;								// Header + Html Content

	//if there is a index specify in the server and not extension in the path
	if(!data->index.empty() && !isExtension(uri) && data->autoindex == "off")
		filePath = defaultPath + data->index;
	else
		filePath = defaultPath;

	// check if there is a location path
	locationPath = check_location(uri, data->location, data);
	if(!locationPath.empty())
		filePath = locationPath;

	// Debug
	std::cout << "The data->path\t:\t" YELLOW << data->path << RESET "" << std::endl; 
	std::cout << "The filePath\t:\t" YELLOW << filePath << RESET "" << std::endl; 
	std::cout << "The defaultPath\t:\t" YELLOW << defaultPath << RESET  ""<< std::endl; 
	std::cout << "The uri\t:\t\t" YELLOW << uri << RESET "" << std::endl; 
	std::cout << "The autoindex\t:\t" YELLOW << (data->autoindex == "on" ? "true" : "off") << RESET << std::endl;
	std::cout << "The isDirectory\t:\t" YELLOW << isDirectory(("./" + uri + "/")) << RESET << std::endl;

	if (isDirectory(("./" + uri)) && data->autoindex == "on")// Auto Index
	{
		std::cout << BLUE "It's a directory" RESET << std::endl; // Debug
		std::vector<std::string> files = listDirectory(("./" + uri));
		std::string directory = "./" + std::string(uri[0] != '\0' ? uri + "/" : "");
		content = generateAutoIndexPage(directory, files);  // Contenu de la page HTML de l'autoindex
		std::cout << "The content\t:\t" YELLOW << content << RESET << std::endl;
	}
	else if (filePath.find(".py") != std::string::npos)
	{
		std::cout << BLUE "It's a CGI" RESET << std::endl; // Debug
		checkAccessFile(code, filePath, data);
		content = CGIHandler::execute(filePath.c_str(), code);
	}
	else
	{
		std::cout << BLUE "It's a file" RESET << std::endl; // Debug
		checkAccessFile(code, filePath, data);
		//read the file content
		content = readFile(filePath);
		// get the type of the request file
	}

	response = httpGetResponse(code, getContentType(uri), content);
	// Debug
	std::cout << "\nThe filePath :\t" YELLOW << filePath << RESET "\nDefault path :\t" YELLOW << defaultPath << RESET "\nUri :\t\t" YELLOW << uri << RESET "" << std::endl; 
	std::cout << "\nThe content type: " << getContentType(uri) << RESET << std::endl;
	//check acces of filePath
	checkAccessFile(code, filePath);

	// get the type of the request file
    if(!download)
	    response = httpGetResponse(code, contentType, content);
    // if its a download file not the same request
    else
	    response = httpGetResponseDownload(code, contentType, content);
	//send response
	if(send(data->sockfd, response.c_str(), response.size(), 0) < 0)
	{
		std::cout << strerror(errno) << std::endl;
		throw Response::ErrorSendingResponse(); 
	}
    std::cout << "response send\n";
    close(data->sockfd);
}

void redirRequest(std::map<std::string, std::string>::iterator redir, int fd)
{
	std::string response = "HTTP/1.1 302 Found \r\n"
							"Location: " + redir->second + "\r\n"
							"Content-Type: text/html\r\n"
							"Content-Length: 0 \r\n"
							"Connection: keep-alive\r\n\r\n";

	if(send(fd, response.c_str(), response.size(), 0) < 0)
	{
		std::cout << strerror(errno) << std::endl;
		std::cout << "Error redirection: " << strerror(errno) << std::endl;
	}
}