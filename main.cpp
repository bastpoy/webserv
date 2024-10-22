#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib> // For exit() and EXIT_FAILURE
#include <iostream> // For cout
#include <unistd.h> // For close
#include <errno.h> // For errno
#include <string.h> // For strerror
#include <sstream> // For string streams
#include <fstream>      // std::ifstream
#include <stdexcept>
#include <map>

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
    std::ifstream file(path.c_str());
    if(!file.is_open())
    {
        throw std::runtime_error("error opening file:" + path);
    }
    while(std::getline(file, content))
    {
        finalContent = finalContent + content;
    }
    return (finalContent);
}

void getFileContent(std::string &content, std::string &uri)
{
    std::string defaultPath = "./website";
    std::string filePath = defaultPath + uri; // Change this to your file path
    
    try 
    {
        content = readFile(filePath);
    } 
    catch (const std::exception& e) 
    {
        std::cerr << "Error reading file: " << e.what() << std::endl;
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
    if (sockfd == -1) {
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
        if (bytesRead < 0) 
        {
            std::cerr << "Error reading data. errno: " << strerror(errno) << std::endl;
            return (errno);
        } 
        else {
            buffer[bytesRead] = '\0'; // Null-terminate the buffer
            // std::cout << "Received message: " << buffer << std::endl;
        }
        std::string path = buffer;
        std::string content;
        std::string contentType;

        // je recupere le path de chaque requete http
        if(!path.empty())
        {
            //je recupere l'uri
            path = path.substr(path.find('/'), path.size() - path.find('/'));
            path = path.substr(0, path.find(' '));
            std::cout << "the path after: " << path << std::endl;
            //je recupere le type du fichier
            contentType = getContentType(path);
        }
        else
            contentType = "application/octet-stream";

        // je recupere le contenu du fichier
        getFileContent(content, path);
        std::cout << "the content type: " << contentType << std::endl;
        std::string response = "HTTP/1.1 200 OK \r\n"
                                // "Content-Type: text/html\r\n"
                                "Content-Type: " + contentType + ";\r\n"
                                "Content-Length: " + to_string(content.size()) + "\r\n"
                                "Connection: close\r\n"
                                "\r\n" + content;
        send(connection, response.c_str(), response.size(), 0);
        close(connection);
        std::cout << "\n\n";
    }
    close(sockfd);

    return 0;
}