#include "Header.hpp"

void closeAllFileDescriptors() 
{
	int i = 3;
	while(i < 1024)
	{
		close(i);
		i++;
	}
	return;
	DIR *dir = opendir("/proc/self/fd");
	if (dir == NULL)
	{
		int maxFd = sysconf(_SC_OPEN_MAX);
		for (int fd = 0; fd < maxFd; ++fd)
			close(fd);
		return;
	}

	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL) {
		if (entry->d_name[0] == '.') 
			continue;
		int fd = atoi(entry->d_name);
		if (fd > 2)
			close(fd);
	}
	
	closedir(dir);
}

void errorCloseEpollFd(int &epoll_fd, int errCode)
{
	if(errCode != 7)
		close(epoll_fd);
	if(errCode == 1)
		std::cout << "Error epoll waiting: ";
	else if(errCode == 2)
		std::cout << "Error to create socket: ";
	else if(errCode == 3)
		std::cout << "Error adding option to socket: ";
	else if(errCode == 4)
		std::cout << "Error controlling socket: ";
	else if(errCode == 5)
		std::cout << "Error receiving data: ";
	else if(errCode == 6)
		std::cout << "Error accepting new client: ";
	else if(errCode == 7)
		std::cout << "Error creating epoll instance: ";
	closeAllFileDescriptors();
	std::cout << errno << " " <<strerror(errno) << std::endl;
	throw Response::Error();
}

void	notFoundFavicon(t_serverData *data)
{
	std::string contentFile = readFile("./www/error/404.html", data);

	std::string response = "HTTP/1.1 404 Not Found\r\n"
							"Content-Type: text/html\r\n"
							"Content-Length: " + to_string(contentFile.size()) + "\r\n"
							"Connection: keep-alive\r\n"
							"\r\n" + contentFile;
	if(send(data->sockfd, response.c_str(), response.size(), 0) < 0)
	{
		std::cout << strerror(errno) << std::endl;
		throw Response::ErrorSendingResponse(); 
	}
	throw Response::Error();
}
static void	contentTooLarge(t_serverData *data)
{
	std::string jsonContent = 
	"{\n\t\"error\": \"Upload failed\",\n\t\"message\" : \"Maximum allowed upload size is "+ data->maxBody + "bytes\"" + "\n}";
	std::cout << jsonContent << std::endl;

	std::string response = "HTTP/1.1 413 Request Entity Too Large\r\n"
							"Content-Type: application/json\r\n"
							"Content-Length: " + to_string(jsonContent.size()) + "\r\n"
							"\r\n" + jsonContent;

	if(send(data->sockfd, response.c_str(), response.size(), 0) < 0)
	{
		std::cout << strerror(errno) << std::endl;
		throw Response::ErrorSendingResponse(); 
	}
	throw Response::Error();
}

static void	choose_error_page(std::string error, t_serverData *data, std::string path)
{
	if (error == "400")
		Response::sendResponse("400 Bad Request", "text/html", read_error_file(path, data), data);
	else if (error == "403")
		Response::sendResponse("403 Forbidden", "text/html", read_error_file(path, data), data);
	else if (error == "404")
		Response::sendResponse("404 Not Found", "text/html", read_error_file(path, data), data);
	else if (error == "405")
		Response::sendResponse("405 Method Not Allowed", "text/html", read_error_file(path, data), data);
	else if (error == "413")
		Response::sendResponse("413 Content Too Large", "text/html", read_error_file(path, data), data);
	else if (error == "500")
		Response::sendResponse("500 Internal Server Error", "text/html", read_error_file(path, data), data);
	else if (error == "501")
		Response::sendResponse("501 Not Implemented", "text/html", read_error_file(path, data), data);
}

void errorPage(std::string error, t_serverData *data)
{
	std::vector<Location>::iterator it1 = data->location.begin();
	if(it1 != data->location.end())
	{
		it1->setPath(it1->getPath().substr(1, it1->getPath().size() - 1));
		std::string uri = data->buffer.substr(data->buffer.find('/') + 1, data->buffer.size() - data->buffer.find('/'));
		uri = uri.substr(0, uri.find(' '));
		while(it1 != data->location.end())
		{
			if(!it1->getPath().empty() && uri.find(it1->getPath()) != std::string::npos)
			{
				std::cout << RED << "match location" << RESET << std::endl;
				if(it1->getErrorPage().find(error) != it1->getErrorPage().end())
				{
					std::string second = it1->getErrorPage().find(error)->second;
					std::string path;
					if(!it1->getRoot().empty())
						path = it1->getRoot() + second;
					else
						path = data->path + second;
					std::cout << RED "LOCATION ERROR MATCH" << RESET << std::endl;
					choose_error_page(error, data, path);
				}
			}
			it1++;
		}
	}

	if(data->errorPage.size())
	{
		std::map<std::string, std::string>::iterator it = data->errorPage.find(error);
		if(it != data->errorPage.end())
		{
			std::string path = data->path + it->second;
			std::cout << "the path error_code: " << path << std::endl;
			choose_error_page(error, data, path);
		}
	}
	if (error == "400")
		Response::sendResponse("400 Bad Request", "text/html", read_error_file("./www/error/400.html", data), data);
	else if (error == "403")
		Response::sendResponse("403 Forbidden", "text/html", read_error_file("./www/error/403.html", data), data);
	else if (error == "404")
		Response::sendResponse("404 Not Found", "text/html", read_error_file("./www/error/404.html", data), data);
	else if (error == "405")
		Response::sendResponse("405 Method Not Allowed", "text/html", read_error_file("./www/error/405.html", data), data);
	else if (error == "413")
		Response::sendResponse("413 Content Too Large", "text/html", read_error_file("./www/error/413.html", data), data);
	else if (error == "413")
		contentTooLarge(data);
	else if (error == "500")
		Response::sendResponse("500 Internal Server Error", "text/html", read_error_file("./www/error/500.html", data), data);
	else if (error == "501")
		Response::sendResponse("501 Not Implemented", "text/html", readFile("./www/error/501.html", data), data);
	else if (error == "504")
		Response::sendResponse("504 Gateway Timeout", "text/html", readFile("./www/error/504.html", data), data);
}
