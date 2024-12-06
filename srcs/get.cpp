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
	it->setPath1(it->getPath().substr(1, it->getPath().size() - 1));
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
                redirRequest(it->getRedir().begin()->second, data->sockfd, data);
                //and leave
                throw Response::Error();
            }
			//if file in my request
			if(isExtension(uri))
			{
                std::cout << "inside location extension" << std::endl;
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
                std::cout << "inside location no file" << std::endl;
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

void checkAccessFile(std::string &code, std::string &filePath, t_serverData *data)
{
	// check if i can access the current ressource request 
	if(access(filePath.c_str(), F_OK) != 0)
    {
        std::cout << "here" << std::endl;
		errorPage("404", data);
    }
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

void process_extension(std::string &filePath, std::string &code, std::string uri, std::string buffer, std::string &content, Cookie &cookie, t_serverData *data, std::map<int, t_serverData*> &fdEpollLink)
{
    // if no root inside my server
    if(data->path.empty())
    {
        errorPage("403", data);
    }
    //if i have an extension
    if(isExtension(uri) || is_cgi_extension(uri))
    {
        filePath = data->path + uri;
        //if its a python file
        if (is_cgi_extension(filePath))
        {
            std::cout << BLUE "It's a CGI " RESET << filePath << std::endl; // Debug
            checkAccessFile(code, filePath, data);
            // content = execute(filePath.c_str(), code, data);
            content = HandleCgiRequest(filePath.c_str(), data, fdEpollLink);
        }
        //if i am at a connexion page and if i have cookies
        else if(filePath  == "./www/pages/cookie/connexion.html" && check_cookie_validity(cookie, get_cookie_id(buffer)))
        {
            std::cout << "no nead to reconnect cause user already exist\n";
            redirRequest("/", data->sockfd, data);
            throw Response::responseOk();
        }
        else
            content = readFile(filePath, data);
    }
}

void getRequest(std::string &uri, t_serverData *data, Cookie &cookie, std::string buffer, std::map<int, t_serverData*> &fdEpollLink)
{
	std::vector<Location>location = data->location;
	std::string	content;
	std::string code;
	std::string contentType = getContentType(uri);
	//check if I have a location block that match the query
	std::string filePath = check_location(uri, content, data->location, data);

	//check if i dont have a location
	if(filePath.empty())
	{
		// if no root inside my server
		if(data->path.empty())
			errorPage("403", data);
        // if i have a file to download
        else if(isExtensionDownload(uri))
        {
            filePath= data->path + uri;
            content = readFile(filePath, data);
        }
        // if i have a file to download or uri
		else if(isExtension(uri) || is_cgi_extension(uri))
            process_extension(filePath, code, uri, buffer, content, cookie, data, fdEpollLink);
        //if i make a deconnexion
        else if(uri == "pages/deconnexion/")
        {
            std::string id = get_cookie_id(buffer);
            if(!id.empty() && check_cookie_validity(cookie, id))
                cookie.remove_session_id(id);
            redirRequest("/", data->sockfd, data);
            throw Response::responseOk();
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
	// std::cout << YELLOW "the filePath is: " << filePath << " uri : " << uri << RESET << std::endl;

    //check if i am a directory and if i can enter inside
	if (isDirectory(filePath))
		checkAccessDir(code, filePath, data);
    //idem for the file
	checkAccessFile(code, filePath, data);
	std::string response = httpGetResponse(code, contentType, content, data);
    // std::cout << MAGENTA "handling request\n" << response << RESET << std::endl;
	if(send(data->sockfd, response.c_str(), response.size(), 0) < 0)
	{
		std::cout << strerror(errno) << std::endl;
		throw Response::ErrorSendingResponse();
	}
}

//parse the get request header and return the response with getrequest
void parseAndGetRequest(std::string buffer, t_serverData *data, Cookie &cookie, std::map<int, t_serverData*> &fdEpollLink)
{
    //get the url of the request
    std::string path = buffer.substr(buffer.find('/') + 1, buffer.size() - buffer.find('/'));
    path = path.substr(0, path.find(' '));

    std::cout << "GET RESPONSE " << path <<  std::endl;
    if(path.find("favicon.ico") != std::string::npos)
        return notFoundFavicon(data);
    //if i have a ? inside my url which represent filtering
    else if(path.find("?") != std::string::npos)
        errorPage("501", data);
    //if i have a redirection to delete page i modify it in the displaydeletepage
    else if(path == "pages/delete/delete.html")
        displayDeletePage(path, data);
    // return the data to the client
    else
        getRequest(path, data, cookie, buffer, fdEpollLink);
}

