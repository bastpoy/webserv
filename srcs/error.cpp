#include "Header.hpp"

//Error function and close epoll fd
void errorCloseEpollFd(int &epoll_fd, int errCode)
{
	if(errCode != 7) // if its different from epoll_create
		close(epoll_fd);
	if(errCode == 1) // epoll waiting Error function
		std::cerr << "Error epoll waiting: ";
	else if(errCode == 2) // Error creating socket
		std::cerr << "Error to create socket: ";
	else if(errCode == 3) // Error setsocketopt
		std::cerr << "Error adding option to socket: ";
	else if(errCode == 4) // epoll ctl
		std::cerr << "Error controlling socket: ";
	else if(errCode == 5) // Error recv
		std::cerr << "Error receiving data: ";
	else if(errCode == 6) // error accept
		std::cerr << "Error accepting new client: ";
	else if(errCode == 7) // error accept
		std::cerr << "Error creating epoll instance: ";
	std::cout << strerror(errno) << std::endl;
	throw Response::Error();
}

void contentTooLarge(std::string size, t_serverData *data)
{
	//send json response to the client
	std::string jsonContent = 
	"{\n\t\"error\": \"Upload failed\",\n\t\"message\" : \"Maximum allowed upload size is "+ size + "bytes\"" + "\n}";
	std::cout << jsonContent << std::endl;

	//header response
	std::string response = "HTTP/1.1 413 Content Too Large\r\n"
							"Content-Type: application/json\r\n"
							"Content-Length: " + to_string(jsonContent.size()) + "\r\n"
							"\r\n" + jsonContent;

	//send response
	if(send(data->sockfd, response.c_str(), response.size(), 0) < 0)
	{
		std::cout << strerror(errno) << std::endl;
		throw Response::ErrorSendingResponse(); 
	}
}

void errorPage(std::string error, t_serverData *data)
{
	if (error == "400")
		Response::sendResponse("400 Bad Request", "text/html", readFile("./www/error/error400.html"), data);
	else if (error == "403")
		Response::sendResponse("403 Forbidden", "text/html", readFile("./www/error/error403.html"), data);
	else if (error == "404")
		Response::sendResponse("404 Not Found", "text/html", readFile("./www/error/error404.html"), data);
	else if (error == "500")
		Response::sendResponse("500 Internal Server Error", "text/html", readFile("./www/error/error500.html"), data);
}