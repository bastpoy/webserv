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
		std::cout << filePath << ": " RED "Fichier introuvable\n" RESET << access(filePath.c_str(), F_OK) << std::endl;
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

void getRequest(std::string uri, t_serverData *data)
{
	// bool autoindex = true;
	std::vector<Location>location = data->location;
	//get the contentType
	std::string contentType = getContentType(uri);
	// std::string filePath = check_location(uri, data->location, data);
    std::string filePath;
	std::string	content;
	std::string code;

    // std::cout << "uri " << uri << std::endl;
	//check first if i have a location
	if(filePath.empty())
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

			// std::cout << "The data->path\t:\t" YELLOW << data->path << RESET "" << std::endl; 
			// std::cout << "The uri\t:\t\t" YELLOW << uri << RESET "" << std::endl; 
			// std::cout << "The filePath\t:\t" YELLOW << filePath << RESET "" << std::endl; 

			if (filePath.find(".py") != std::string::npos)
			{
				std::cout << BLUE "It's a CGI" RESET << std::endl; // Debug
				checkAccessFile(code, filePath);
				content = CGIHandler::execute(uri.c_str(), code);
			}
            else
                content = readFile(filePath);
		}
		// if an index inside my server
		else if (!data->index.empty())
		{
			filePath = data->path + uri + data->index;
            content = readFile(filePath);
		}
		//if i have an autoindex 
		else if(!data->autoindex.empty() && data->autoindex == "on")
		{
			filePath = data->path + uri;

			std::vector<std::string> files = listDirectory(filePath);
			content = generateAutoIndexPage(uri, files);
		}
		else
		{
			forbidden(data);
			throw Response::Error();
		}
	}
	std::cout << "the filePath is: " << filePath << " uri : " << uri << std::endl;

	//check acces of filePath
	checkAccessFile(code, filePath);

	// get the type of the request file
	std::string response = httpGetResponse(code, contentType, content);

	//send response
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
		checkAccessFile(code, filePath);
		content = CGIHandler::execute(filePath.c_str(), code);
	}
	else
	{
		std::cout << BLUE "It's a file" RESET << std::endl; // Debug
		checkAccessFile(code, filePath);
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
