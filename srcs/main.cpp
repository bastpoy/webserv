#include "resHeader.hpp"
#include <sys/epoll.h>

template <typename T>
std::string to_string(T value) {
  std::ostringstream oss;
  oss << value;
  return oss.str();
}

std::string readFile(std::string &path)
{
	std::string content;
	std::string finalContent;
	std::ifstream file(path.c_str(), std::ios::binary);
	if(!file.is_open())
	{
		throw resHeader::ErrorOpeningFile();
	}
	return std::string(
		std::istreambuf_iterator<char>(file),
		std::istreambuf_iterator<char>()
	);
}

void getFileContent(std::string &uri , std::string &contentType, int connection)
{
	std::string defaultPath = "./website";
	std::string filePath = defaultPath + uri; // Change this to your file path
	std::string content;

	//read the file content 
	content = readFile(filePath);
	//return the response
	std::cout << "the content type: " << contentType << std::endl;
	std::string response = "HTTP/1.1 200 OK \r\n"
							// "Content-Type: text/html\r\n"
							"Content-Type: " + contentType + "\r\n"
							"Content-Length: " + to_string(content.size()) + "\r\n"
							"Connection: close\r\n"
							"\r\n" + content;
	if(send(connection, response.c_str(), response.size(), 0) < 0)
	{
		std::cout << strerror(errno) << std::endl;
		throw resHeader::ErrorSendingResponse(); 
	}
}

std::string getContentType(const std::string &path) 
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
	return "application/octet-stream"; // Default content type
}


int main() {
	// Create a socket (IPv4, TCP)
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		std::cerr << "Failed to create socket. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}

	// Bind the socket to an address and port
	sockaddr_in sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	sockaddr.sin_port = htons(9999); // htons is necessary to convert a number to network byte order
	if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) 
	{
		std::cerr << "Failed to bind to port 9999. errno: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}

	// Start listening. Hold at most 10 connections in the queue
	if (listen(sockfd, 10) < 0) 
	{
		std::cerr << "Failed to listen on socket. errno: " << strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << "Server is listening on port 9999..." << std::endl;


	//Ozan

	// Creating an epoll instance
	int epoll_fd = epoll_create1(0);
	if (epoll_fd == -1)
	{
		std::cerr << "Failed to create epoll file descriptor" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Add listening-socket to epoll
	epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = sockfd;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &event);

	//Stocking elements
	epoll_event events[10];

	//End Ozan


	//Ozan 2 (while)
	while (true)
	{
		int	n = epoll_wait(epoll_fd, events, 10, -1);

		for (int i = 0; i < n; i++)
		{
			if (events[i].data.fd == sockfd)
			{
				// New Connection
				int		connection = accept(sockfd, nullptr, nullptr);

				event.events = EPOLLIN;
				event.data.fd = connection;
				epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connection, &event);
			}
			else
			{
				// Read the request
				int		connection = events[i].data.fd;
				char	buffer[1024];
				ssize_t	bytesRead = recv(connection, buffer, sizeof(buffer) - 1, 0);

				buffer[bytesRead] = '\0';
				std::string path = buffer;
				path = path.substr(path.find('/'), path.size() - path.find('/'));
				path = path.substr(0, path.find(' '));

				std::string contentType = getContentType(path);
				try
				{
					getFileContent(path, contentType, connection);
				}
				catch(const std::exception& e)
				{
					std::cout << e.what() << std::endl;
				}
				close(connection); //Close connection when his done
				

			}
		}
	}


	// while(true)
	// {
	// 	// Accept a connection
	// 	socklen_t addrlen = sizeof(sockaddr);  
	// 	int connection = accept(sockfd, (struct sockaddr*)&sockaddr, &addrlen);
	// 	if (connection < 0) {
	// 		std::cerr << "Failed to grab connection. errno: " << errno << std::endl;
	// 		exit(EXIT_FAILURE);
	// 	}
	// 	std::cout << "Connection accepted." << std::endl;

	// 	// Receive data from the connection
	// 	char buffer[1024];
	// 	ssize_t bytesRead = recv(connection, buffer, sizeof(buffer) - 1, 0);
	// 	if (bytesRead < 0) 
	// 	{
	// 		std::cerr << "Error reading data. errno: " << strerror(errno) << std::endl;
	// 		return (errno);
	// 	} 
	// 	buffer[bytesRead] = '\0'; // Null-terminate the buffer

	// 	std::string path = buffer;
	// 	std::string content;
	// 	std::string contentType;

	// 	// get http request
	// 	if(!path.empty())
	// 	{
	// 		//get uri
	// 		std::cout << "-----the path before-----\n: " << path;
	// 		std::cout << "\n\n";
	// 		path = path.substr(path.find('/'), path.size() - path.find('/'));
	// 		path = path.substr(0, path.find(' '));
	// 		std::cout << "the path after: " << path << std::endl;
	// 		//get the type file
	// 		contentType = getContentType(path);
	// 	}
	// 	else
	// 		contentType = "/index.html";

	// 	//get the file content
	// 	try
	// 	{
	// 		getFileContent(path, contentType, connection);
	// 	}
	// 	catch(const std::exception& e)
	// 	{
	// 		std::cout << e.what() << std::endl;
	// 	}
	// 	close(connection);
	// 	std::cout << "\n\n";
	// }
	// close(sockfd);

	return 0;
}