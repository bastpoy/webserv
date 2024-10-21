#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib> // For exit() and EXIT_FAILURE
#include <iostream> // For cout
#include <unistd.h> // For close
#include <errno.h> // For errno
#include <string.h> // For strerror
#include <sstream> // For string streams

template <typename T>
std::string to_string(T value) {
  std::ostringstream oss;
  oss << value;
  return oss.str();
}

int main() {
  // Create a socket (IPv4, TCP)
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    std::cerr << "Failed to create socket. errno: " << errno << std::endl;
    exit(EXIT_FAILURE);
  }

  // Bind the socket to an address and port
  sockaddr_in sockaddr;
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_addr.s_addr = INADDR_ANY;
  sockaddr.sin_port = htons(9999); // htons is necessary to convert a number to network byte order
  if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
    std::cerr << "Failed to bind to port 9999. errno: " << strerror(errno) << std::endl;
    exit(EXIT_FAILURE);
  }

  // Start listening. Hold at most 10 connections in the queue
    if (listen(sockfd, 10) < 0) {
      std::cerr << "Failed to listen on socket. errno: " << strerror(errno) << std::endl;
      exit(EXIT_FAILURE);
    }
    std::cout << "Server is listening on port 9999..." << std::endl;

  while(true)
  {
    // Accept a connection
    socklen_t addrlen = sizeof(sockaddr);  
    int connection = accept(sockfd, (struct sockaddr*)&sockaddr, &addrlen);
    if (connection < 0) {
      std::cerr << "Failed to grab connection. errno: " << errno << std::endl;
      exit(EXIT_FAILURE);
    }
    std::cout << "Connection accepted." << std::endl;

    // Receive data from the connection
    char buffer[100];
    ssize_t bytesRead = recv(connection, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead < 0) {
      std::cerr << "Error reading data. errno: " << strerror(errno) << std::endl;
    } else {
      buffer[bytesRead] = '\0'; // Null-terminate the buffer
      std::cout << "Received message: " << buffer << std::endl;
    }

    // Send a response to the sockfd
    // std::string response = "Hello from server\n";
    // std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
    //                             "<html><body><h1>Hello, World!</h1></body></html>";

    std::string response_body = "<html><body><h1>Hello, World!</h1></body></html>";
    std::string response = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/html\r\n"
                           "Content-Length: " + to_string(response_body.size()) + "\r\n"
                           "Connection: close\r\n"
                           "\r\n" + response_body;
    std::cout << "\nResponse:\n" << response;
    send(connection, response.c_str(), response.size(), 0);
    close(connection);
    std::cout << "\n\n";
  }

  // Close the connections
  close(sockfd);

  return 0;
}