#include "Header.hpp"

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
	std::cout << strerror(errno) << std::endl;
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

void errorPage(std::string error, t_serverData *data)
{
	if (error == "400")
		Response::sendResponse("400 Bad Request", "text/html", readFile("./www/error/error400.html", data), data);
	else if (error == "403")
		Response::sendResponse("403 Forbidden", "text/html", readFile("./www/error/error403.html", data), data);
	else if (error == "404")
		Response::sendResponse("404 Not Found", "text/html", readFile("./www/error/error404.html", data), data);
	else if (error == "413")
		contentTooLarge(data);
	else if (error == "500")
		Response::sendResponse("500 Internal Server Error", "text/html", readFile("./www/error/error500.html", data), data);
	else if (error == "501")
		Response::sendResponse("501 Not Implemented", "text/html", readFile("./www/error/error501.html", data), data);
	else if (error == "504")
		Response::sendResponse("504 Gateway Timeout", "text/html", readFile("./www/error/error504.html", data), data);
}
