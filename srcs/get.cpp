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
	std::string contentType = getContentType(uri);
	std::string filePath = check_location(uri, data->location, data);
	std::string	content;
	std::string code;

	if(filePath.empty())
	{
		std::cout << "no location match" << std::endl;
		if(data->path.empty())
		{
			errorPage("403", data);
			throw Response::Error();
		}
		if(isExtension(uri))
		{
			filePath = data->path + uri;
			std::cout << "The data->path\t:\t" YELLOW << data->path << RESET "" << std::endl;
			std::cout << "The uri\t:\t\t" YELLOW << uri << RESET "" << std::endl;
			std::cout << "The filePath\t:\t" YELLOW << filePath << RESET "" << std::endl;
			checkAccessFile(code, filePath, data);
			if (filePath.find(".py") != std::string::npos)
				content = CGIHandler::execute(filePath.c_str(), code);
			else
				content = readFile(filePath);
		}
		else if (!data->index.empty())
		{
			filePath = data->path + uri + data->index;
			content = readFile(filePath);
		}
		else if(!data->autoindex.empty() && data->autoindex == "on")
		{
			filePath = data->path + uri;
			std::cout << "i have an autoindex" << std::endl;
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
