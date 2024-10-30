#include "serverAddr.hpp"

serverAddr::~serverAddr()
{}

//getter

//setter
void serverAddr::setSocketfd(int socketfd)
{
    this->socketfd.insert(socketfd);
}

template <typename T>
std::string to_string(T value) {
  std::ostringstream oss;
  oss << value;
  return oss.str();
}

std::string readFile(std::string &path)
{
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

void serverAddr::createListenAddr(httpConfig &config)
{
    std::vector<serverConfig>::iterator itbeg = config.getServer().begin();
    
    //creating the poll
    int epoll_fd = epoll_create(10);
    if (epoll_fd == -1) {
        std::cerr << "Failed to create epoll instance: " << strerror(errno) << std::endl;
        throw resHeader::Error();
    }
    while(itbeg != config.getServer().end())
    {
        //creation addrinfo struc to stock my addrinfo informations
        struct sockaddr_in addr;
        
        //I create my socket
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) {
            std::cout << "Failed to create socket: " << strerror(errno) << std::endl;
            throw resHeader::ErrorCreatingSocket();
        }

        //add properties to allow the socket to be reusable even if it is in time wait
        int opt = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
            std::cerr << "Failed to set SO_REUSEADDR" << std::endl;
            throw resHeader::Error();
        }

        //print address and port
        std::cout << itbeg->getPort() << "-" << itbeg->getServerName() << "-" << std::endl;

        //fill my sockaddr_in addr with the result of getaddrinfo
        addr.sin_family = AF_INET;
        addr.sin_port = htons(atoi(itbeg->getPort().c_str()));
        if(inet_pton(AF_INET, itbeg->getServerName().c_str(), &addr.sin_addr.s_addr) < 0)
        {
            std::cout << "INET_PTON: " << strerror(errno) << std::endl;
            throw resHeader::Error();
        }

        //bind my socket with the current fill sockaddr_in
        if (bind(sockfd, (struct sockaddr*)&addr, sizeof(sockaddr)) < 0) 
        {
            std::cout << "BIND: "<< strerror(errno) << " ";
            throw resHeader::Error();
        }

        //listen on the current socket created
        if (listen(sockfd, 10) < 0) 
        {
            std::cout << "LISTEN: " << strerror(errno) << " ";
            throw resHeader::Error();
        }


        //create an epoll event for every socketfd
        struct epoll_event event;
        event.events = EPOLLIN; // Monitor for input events
        event.data.fd = sockfd;
        //je stocke les infos du serveurs dans un ptr
        // event.data.ptr = static_cast<void*>(&config.getServer());

        //add the epoll event to my epoll_fd instance
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &event) == -1) {
            std::cerr << "Failed to add socket to epoll: " << strerror(errno) << std::endl;
            throw resHeader::Error();
        }
        
        //add every socket to my set container
        this->setSocketfd(sockfd);

        itbeg++;
    }
    const int MAX_EVENTS = 20;
    struct epoll_event events[MAX_EVENTS];

    std::cout << "\nWaiting for connection...\n";

    while (true) {
        //epollwait return a number corresponding to all the files descriptor
        int num_fds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (num_fds == -1) {
            std::cerr << "epoll_wait error: " << strerror(errno) << std::endl;
            close(epoll_fd);
            throw resHeader::Error();
        }
        // I iterate through each fd
        for (int i = 0; i < num_fds; ++i) 
        {
            int fd = events[i].data.fd;
            // check if my fd is equal to a socket for handcheck
            if(this->socketfd.find(fd) != this->socketfd.end())
            {
                // listen to add new fd to my epoll structure
                struct sockaddr_in client_addr;
                socklen_t client_addr_len = sizeof(client_addr);
                
                //accept the connection
                int client_fd = accept(fd, (struct sockaddr*)&client_addr, &client_addr_len);
                if (client_fd == -1) {
                    std::cerr << "accept() failed: " << strerror(errno) << std::endl;
                    close(epoll_fd);
                    throw resHeader::Error();                    
                }
                // add new fd to my epoll instance
                struct epoll_event client_event;
                client_event.events = EPOLLIN;
                client_event.data.fd = client_fd;
                // client_event.data.ptr = events[i].data.ptr;
                if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event) == -1)
                {
                    std::cerr << "Failed to add socket to epoll: " << strerror(errno) << std::endl;
                    close(epoll_fd);
                    throw resHeader::Error();
                }
                std::cout << "\n-----New connection established-----\n";
            }
            //Connection already etablish 
            else
            {
                // check for modification inside the socket
                if(events[i].events & EPOLLIN)
                {
                    // serverConfig* info = static_cast<serverConfig*>(events[i].data.ptr);

                    char buffer[1024];
                    std::cout << "\nReading data...\n";
                    ssize_t bytes_read = recv(fd, buffer, sizeof(buffer) - 1, 0);
                    if (bytes_read == -1) {
                        std::cerr << "recv() failed: " << strerror(errno) << std::endl;
                        close(fd);
                        continue;
                    } else if (bytes_read == 0) {
                        // Connection closed by the client
                        std::cout << "Client disconnected: " << fd << std::endl;
                        close(fd);
                        continue;
                    }

                    // Null-terminate the buffer to make it a valid C-string
                    buffer[bytes_read] = '\0';
                    std::cout << "Received data from socket " << fd << ": " << buffer << std::endl;

                    //formulate response
                    std::string path = buffer;
                    std::string content;
                    std::string contentType;

                    // get http request
                    if(!path.empty())
                    {
                        //get uri
                        path = path.substr(path.find('/'), path.size() - path.find('/'));
                        path = path.substr(0, path.find(' '));
                        std::cout << "the uri: " << path << std::endl;
                        //get the type file
                        contentType = getContentType(path);
                        std::cout << "the content type is: " << contentType << std::endl;
                    }
                    else
                        contentType = "text/html";

                    // (void)info;
                    
                    // std::cout << "le pointeur vaut: " << events[i].data.ptr << std::endl; 
                    // if(info->getRedir().begin()->first) 
                    //     std::cout << "return " << info->getRedir().begin()->first << " " << info->getRedir().begin()->second << std::endl;

                    //get the file content
                    try
                    {
                        getFileContent(path, contentType, fd);
                    }
                    catch(const std::exception& e)
                    {
                        std::cout << e.what() << std::endl;
                    }
                    std::cout << "\n\n";
                    close(fd);
                }
            }
        }
    }

}
