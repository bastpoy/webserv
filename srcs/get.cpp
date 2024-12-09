#include "Header.hpp"

std::string pathLocation(std::string &content, std::string &uri, std::vector<Location>::iterator it, t_serverData *data, std::string root)
{
	std::string filePath;
	if(!it->getIndex().empty())
	{
		std::cout << "Index in my location" << std::endl;
		filePath = root + uri + it->getIndex();
		content = readFile(filePath, data);
		return filePath;
	}
	else if(!it->getAutoIndex().empty() && it->getAutoIndex() == "on")
	{
		filePath = root + uri;
		std::cout << "filepath: " << filePath << std::endl;
		std::vector<std::string> files = listDirectory(filePath);
		content = generateAutoIndexPage(uri, files);
		return root + uri;
	}
	else if(!data->index.empty())
	{
		filePath = root + uri + data->index;
		content = readFile(filePath, data);
		return filePath;
	}
	else if(!data->autoindex.empty() && data->autoindex == "on")
	{
		filePath = root + uri;
		std::vector<std::string> files = listDirectory(filePath);
		content = generateAutoIndexPage(uri, files);
		return (filePath);
	}
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

	it->setPath(it->getPath().substr(1, it->getPath().size() - 1));
	while(it != location.end())
	{
		if(!it->getPath().empty() && uri.find(it->getPath()) != std::string::npos)
		{
			if(it->getRedir().size())
			{
				std::cout << "inside location redirection" << std::endl;
				redirRequest(it->getRedir().begin()->second, data->sockfd, data);
				throw Response::Error();
			}
			if(isExtension(uri))
			{
				if(!it->getRoot().empty())
				{
					content = readFile(it->getRoot() + uri, data);
					return it->getRoot() + uri;
				}
				else if(!data->path.empty())
				{
					content = readFile(data->path + uri, data);
					return data->path + uri;
				}
				else
					errorPage("404", data);
			}
			else
			{
				if(!it->getRoot().empty())
					return pathLocation(content, uri, it, data, it->getRoot());
				else
				{
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
	if(data->path.empty())
	{
		errorPage("403", data);
	}
	if(isExtension(uri) || is_cgi_extension(uri))
	{
		filePath = data->path + uri;
		if (is_cgi_extension(filePath))
		{
			std::cout << BLUE "It's a CGI " RESET << filePath << std::endl; // Debug
			checkAccessFile(code, filePath, data);
			content = HandleCgiRequest(filePath.c_str(), data, fdEpollLink);
		}
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
	std::string filePath = check_location(uri, content, data->location, data);

	if(filePath.empty())
	{
		if(data->path.empty())
			errorPage("403", data);
		else if(isExtensionDownload(uri))
		{
			filePath= data->path + uri;
			content = readFile(filePath, data);
		}
		else if(isExtension(uri) || is_cgi_extension(uri))
			process_extension(filePath, code, uri, buffer, content, cookie, data, fdEpollLink);
		else if(uri == "pages/deconnexion/")
		{
			std::string id = get_cookie_id(buffer);
			if(!id.empty() && check_cookie_validity(cookie, id))
				cookie.remove_session_id(id);
			redirRequest("/", data->sockfd, data);
			throw Response::responseOk();
		}
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
	if (isDirectory(filePath))
		checkAccessDir(code, filePath, data);
	checkAccessFile(code, filePath, data);
	std::string response = httpGetResponse(code, contentType, content, data);
	if(send(data->sockfd, response.c_str(), response.size(), 0) < 0)
	{
		std::cout << strerror(errno) << std::endl;
		throw Response::ErrorSendingResponse();
	}
}

void parseAndGetRequest(std::string buffer, t_serverData *data, Cookie &cookie, std::map<int, t_serverData*> &fdEpollLink)
{
	std::string path = buffer.substr(buffer.find('/') + 1, buffer.size() - buffer.find('/'));
	path = path.substr(0, path.find(' '));

	std::cout << "GET RESPONSE " << path <<  std::endl;
	if(path.find("favicon.ico") != std::string::npos)
		return notFoundFavicon(data);
	else if(path.find("?") != std::string::npos)
		errorPage("501", data);
	else if(path == "pages/delete/delete.html")
		displayDeletePage(path, data);
	else
		getRequest(path, data, cookie, buffer, fdEpollLink);
}

