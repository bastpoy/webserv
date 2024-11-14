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
        errorPage("404", data);
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
            // if i have a redirection
            if(it->getRedir().size())
            {
                //i redirec
                std::cout << "inside location redirection" << std::endl;
                redirRequest(it->getRedir().begin(), data->sockfd);
                //and leave
                throw Response::Error();
            }
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
                    errorPage("404", data);
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
                        errorPage("404", data);
                }
            }
		}
		it++;
	}
	return ("");
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

void checkAccessFile(std::string &code, std::string &filePath, t_serverData *data)
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

void getRequest(std::string &uri, t_serverData *data)
{
	std::vector<Location>location = data->location;
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
			errorPage("403", data);
			throw Response::Error();
		}
        //if i have an extension
		if(isExtension(uri))
		{
			filePath = data->path + uri;
            //if its a python file
			if (filePath.find(".py") != std::string::npos)
			{
				std::cout << BLUE "It's a CGI " RESET << filePath <<std::endl; // Debug
				checkAccessFile(code, filePath, data);
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
        //if i have an autoindex
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

//parse the get request header and return the response with getrequest
void parseAndGetRequest(std::string buffer, t_serverData *data)
{
    //get the url of the request
    std::string path = buffer.substr(buffer.find('/') + 1, buffer.size() - buffer.find('/'));
    path = path.substr(0, path.find(' '));

    std::cout << "GET RESPONSE " << path <<  std::endl;

    if(path.find("favicon.ico") != std::string::npos)
    {
        notFoundFavicon(data);
        return;
    }
    //if i have a ? inside my url which represent filtering
    else if(path.find("?") != std::string::npos)
        errorPage("501", data);
    //if i have a redirection to delete page i modify it in the displaydeletepage
    else if(path == "pages/delete/delete.html")
        displayDeletePage(path, data);
    // return the data to the client
    else
        getRequest(path, data);
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
    close(fd);
}
