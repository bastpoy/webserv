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
    if (dir == NULL) {
        // Fallback to the previous method if /proc is not available
        int maxFd = sysconf(_SC_OPEN_MAX);
        for (int fd = 0; fd < maxFd; ++fd) {
            close(fd);
        }
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') 
            continue;  // Skip . and .. directories

        int fd = atoi(entry->d_name);
        if (fd > 2) {  // Avoid closing stdin, stdout, stderr
            close(fd);
        }
    }
    
    closedir(dir);
}

//Error function and close epoll fd
void errorCloseEpollFd(int &epoll_fd, int errCode)
{
	if(errCode != 7) // if its different from epoll_create
		close(epoll_fd);
	if(errCode == 1) // epoll waiting Error function
		std::cout << "Error epoll waiting: ";
	else if(errCode == 2) // Error creating socket
		std::cout << "Error to create socket: ";
	else if(errCode == 3) // Error setsocketopt
		std::cout << "Error adding option to socket: ";
	else if(errCode == 4) // epoll ctl
		std::cout << "Error controlling socket: ";
	else if(errCode == 5) // Error recv
		std::cout << "Error receiving data: ";
	else if(errCode == 6) // error accept
		std::cout << "Error accepting new client: ";
	else if(errCode == 7) // error accept
		std::cout << "Error creating epoll instance: ";
    closeAllFileDescriptors();
	std::cout << errno << " " <<strerror(errno) << std::endl;
	throw Response::Error();
}

void forbidden(t_serverData *data)
{
	std::string contentFile = readFile("./www/error/error403.html", data);

	std::string response = "HTTP/1.1 403 Forbiden\r\n"
							"Content-Type: text/html\r\n"
							"Content-Length: " + to_string(contentFile.size()) + "\r\n"
							"\r\n" + contentFile;
	//send response
	if(send(data->sockfd, response.c_str(), response.size(), 0) < 0)
	{
		std::cout << strerror(errno) << std::endl;
		throw Response::ErrorSendingResponse(); 
	}
    close(data->sockfd);
    throw Response::Error();
}

void notFound(t_serverData *data)
{
	std::string contentFile = readFile("./www/error/error404.html", data);

	std::string response = "HTTP/1.1 404 Not Found\r\n"
							"Content-Type: text/html\r\n"
                            "Connection: keep-alive\r\n"
							"Content-Length: " + to_string(contentFile.size()) + "\r\n"
							"\r\n" + contentFile;
	//send response
	if(send(data->sockfd, response.c_str(), response.size(), 0) < 0)
	{
		std::cout << strerror(errno) << std::endl;
		throw Response::ErrorSendingResponse(); 
	}
    close(data->sockfd);
    throw Response::Error();
}

void notFoundFavicon(t_serverData *data)
{
	std::string contentFile = readFile("./www/error/error404.html", data);

	std::string response = "HTTP/1.1 404 Not Found\r\n"
							"Content-Type: text/html\r\n"
							"Content-Length: " + to_string(contentFile.size()) + "\r\n"
                            "Connection: keep-alive\r\n"
							"\r\n" + contentFile;
	//send response
	if(send(data->sockfd, response.c_str(), response.size(), 0) < 0)
	{
		std::cout << strerror(errno) << std::endl;
		throw Response::ErrorSendingResponse(); 
	}
    // close(data->sockfd);
    throw Response::Error();
}
void contentTooLarge(t_serverData *data)
{
	//send json response to the client
	std::string jsonContent = 
	"{\n\t\"error\": \"Upload failed\",\n\t\"message\" : \"Maximum allowed upload size is "+ data->maxBody + "bytes\"" + "\n}";
	std::cout << jsonContent << std::endl;

	//header response
	std::string response = "HTTP/1.1 413 Request Entity Too Large\r\n"
							"Content-Type: application/json\r\n"
							"Content-Length: " + to_string(jsonContent.size()) + "\r\n"
							"\r\n" + jsonContent;

	//send response
	if(send(data->sockfd, response.c_str(), response.size(), 0) < 0)
	{
		std::cout << strerror(errno) << std::endl;
		throw Response::ErrorSendingResponse(); 
	}
	throw Response::Error();
}

void choose_error_page(std::string error, t_serverData *data, std::string path)
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
    //check if i have an error page in my actual conf file inside a possible location
    std::vector<Location>::iterator it1 = data->location.begin();
    //if i have location
    if(it1 != data->location.end())
    {
        //modify locationPath to remove the first '/' 
        it1->setPath(it1->getPath().substr(1, it1->getPath().size() - 1));
        //get the uri
        std::string uri = data->buffer.substr(data->buffer.find('/') + 1, data->buffer.size() - data->buffer.find('/'));
        uri = uri.substr(0, uri.find(' '));
        //iterate throught my different server location
        while(it1 != data->location.end())
        {
            // if the path matchs the location path
            if(!it1->getPath().empty() && uri.find(it1->getPath()) != std::string::npos)
            {
                std::cout << RED << "match location" << RESET << std::endl;
                //check if i have an error page that match
                if(it1->getErrorPage().find(error) != it1->getErrorPage().end())
                {
                    //put the path of the file
                    std::string second = it1->getErrorPage().find(error)->second;
                    std::string path;
                    //check if i have a root inside my location
                    if(!it1->getRoot().empty())
                        path = it1->getRoot() + second;
                    //if no root
                    else
                        path = data->path + second;
                    std::cout << RED "LOCATION ERROR MATCH" << RESET << std::endl;
                    choose_error_page(error, data, path);
                }
            }
            it1++;
        }
    }

    // check if i have an error page in my actual conf file inside the server
    if(data->errorPage.size())
    {
        //check if i have an error page
        std::map<std::string, std::string>::iterator it = data->errorPage.find(error);
        if(it != data->errorPage.end())
        {
            std::string path = data->path + it->second;
            std::cout << "the path error_code: " << path << std::endl;
            choose_error_page(error, data, path);
        }
    }
    // if i dont have error specify i return basic error page
	if (error == "400")
		Response::sendResponse("400 Bad Request", "text/html", read_error_file("./www/error/error400.html", data), data);
	else if (error == "403")
		Response::sendResponse("403 Forbidden", "text/html", read_error_file("./www/error/error403.html", data), data);
	else if (error == "404")
		Response::sendResponse("404 Not Found", "text/html", read_error_file("./www/error/error404.html", data), data);
	else if (error == "405")
		Response::sendResponse("405 Method Not Allowed", "text/html", read_error_file("./www/error/error405.html", data), data);
    else if (error == "413")
        Response::sendResponse("413 Content Too Large", "text/html", read_error_file("./www/error/error413.html", data), data);
	else if (error == "413")
		contentTooLarge(data);
	else if (error == "500")
		Response::sendResponse("500 Internal Server Error", "text/html", read_error_file("./www/error/error500.html", data), data);
	else if (error == "501")
		Response::sendResponse("501 Not Implemented", "text/html", readFile("./www/error/error501.html", data), data);
	else if (error == "504")
		Response::sendResponse("504 Gateway Timeout", "text/html", readFile("./www/error/error504.html", data), data);
}
