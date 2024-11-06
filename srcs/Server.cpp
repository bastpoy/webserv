#include "Header.hpp"

//------SERVER FUNCTIONS-------

//Fill data epoll with server iterator
struct epoll_event fillEpoolDataIterator(int sockfd, std::vector<Server>::iterator itbeg)
{
    t_serverData *data = new t_serverData;
    struct epoll_event event;
    
    data->sockfd = sockfd;
    data->port = itbeg->getPort();
    data->server_name = itbeg->getServerName();
    data->path = itbeg->getPath();
    data->maxBody = itbeg->getMaxBody();
    data->index = itbeg->getIndex();
    data->errorPage = itbeg->getErrorPage();
    data->redir = itbeg->getRedir();
    data->location = itbeg->getLocation();

    event.events = EPOLLIN; // Monitor for input events
    //I stock the info server on the event ptr data
    event.data.ptr = static_cast<void*>(data);

    return (event);
}

//Fill data epoll with info from the new client
struct epoll_event fillEpoolDataInfo(int &client_fd, t_serverData *info)
{
    t_serverData *data = new t_serverData;

    data->sockfd = client_fd;
    data->port = info->port;
    data->server_name = info->server_name;
    data->path = info->path;
    data->maxBody = info->maxBody;
    data->index = info->index;
    data->errorPage = info->errorPage;
    data->redir = info->redir;
    data->location = info->location;

    struct epoll_event client_event;

    client_event.events = EPOLLIN;
    client_event.data.ptr = static_cast<void*>(data);

    return(client_event);
}

//IP managing, Binding, Listening
void setupSocket(int &sockfd, struct sockaddr_in &addr, std::vector<Server>::iterator itbeg)
{
    //translate my string ip address into a network adress
    if(inet_pton(AF_INET, itbeg->getServerName().c_str(), &addr.sin_addr.s_addr) < 0)
    {
        std::cout << "wrong IP address: " << strerror(errno) << std::endl;
        throw Response::Error();
    }

    //bind my socket with the current fill sockaddr_in
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(sockaddr)) < 0) 
    {
        std::cout << "BIND: "<< strerror(errno) << " ";
        throw Response::Error();
    }

    //listen on the current socket created
    if (listen(sockfd, 10) < 0)
    {
        std::cout << strerror(errno) << " ";
        throw Response::Error();
    }
}

// Configuration network
void Server::configuringNetwork(std::vector<Server>::iterator &itbeg, ConfigParser &config, int &epoll_fd)
{
	while(itbeg != config.getServers().end())
	{
		//creation addrinfo struc to stock my addrinfo informations
		struct sockaddr_in addr;
		
		//I create my socket
		int sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd == -1)
            errorCloseEpollFd(epoll_fd, 2);

        //add properties to allow the socket to be reusable even if it is in time wait
        int opt = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
            errorCloseEpollFd(epoll_fd, 3);
        
        std::cout << itbeg->getPort() << "-" << itbeg->getServerName() << "-" << std::endl;

		//fill my sockaddr_in addr with the result of getaddrinfo
		addr.sin_family = AF_INET;
		addr.sin_port = htons(atoi(itbeg->getPort().c_str()));

        setupSocket(sockfd, addr, itbeg);

        //create a new instance of epool_event to stock info        
        struct epoll_event event = fillEpoolDataIterator(sockfd, itbeg);

        //add the epoll event to my epoll_fd instance
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &event) == -1)
            errorCloseEpollFd(epoll_fd, 4);
        
        //add every socket to my set container
        this->setSocketFd(sockfd);
		itbeg++;
	}    
}

int acceptConnection(int &fd, int &epoll_fd, struct sockaddr_in &client_addr)
{
    socklen_t client_addr_len = sizeof(client_addr);
    //accept the connection
    int client_fd = accept(fd, (struct sockaddr*)&client_addr, &client_addr_len);
    if (client_fd == -1)
        errorCloseEpollFd(epoll_fd, 6);
    return(client_fd);
}

//reading the request http data
std::string readingData(int &fd)
{
    char buffer[1024];
    ssize_t bytes_read = recv(fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_read == -1)
        errorCloseEpollFd(fd, 5);
    else if (bytes_read == 0) 
    {
        // Connection closed by the client
        std::cout << "Client disconnected: " << fd << std::endl;
        close(fd);
        return ("");
    }

    // Null-terminate the buffer to make it a valid C-string
    std::cout << "bytes read is equal: " << bytes_read << std::endl;
    buffer[bytes_read] = '\0';
    return(buffer);
}

bool read_full_body(int client_socket, std::string &body, int content_length) {
    int total_read = body.size();
    const int buffer_size = 1024;   // Use a large buffer if desired
    char buffer[buffer_size];

    while (total_read < content_length) {
        // Calculate how much more data we need to read
        int bytes_to_read = std::min(content_length - total_read, buffer_size);
        
        // Read data into the buffer
        int bytes_read = recv(client_socket, buffer, bytes_to_read, 0);
        
        if (bytes_read < 0) 
        {
            std::cout << "Error reading from socket: " << strerror(errno) << std::endl;
            throw Response::Error();
        } 
        else if (bytes_read == 0) 
        {
            std::cerr << "Connection closed by the client." << std::endl;
            break; // Connection closed
        }

        // Append the read data to the body string
        body.append(buffer, bytes_read);
        total_read += bytes_read;

        // Optional: Output how much data has been read so far for debugging
        // std::cout << "Read " << bytes_read << " bytes, total read: " << total_read << "/" << content_length << std::endl;
    }

    // Check if we read the expected content length
    return total_read == content_length;
}

bool redirectRequest(std::string buffer, t_serverData *data) 
{
    std::string firstLine = buffer.substr(0, buffer.find("\n"));
    std::string typeRequest =  firstLine.substr(0, buffer.find(" "));

    if(typeRequest == "GET")
    {
        //if i have a redirection 
        if(data->redir.size())
        {
            std::cout << "GET REDIRECTION " << data->port << " " << data->server_name << std::endl;
            return(redirHeader(data->redir.begin(), data->sockfd));
        }
        // else I respond 
        else
        {
            std::cout << "GET RESPONSE" << std::endl;
            //get the url of the request
        
            std::string path = buffer.substr(buffer.find('/') + 1, buffer.size() - buffer.find('/'));
            path = path.substr(0, path.find(' '));
            
            // return the data to the client
            getRequest(path, data);
        }
    }
    else if(typeRequest == "POST")
    {
        std::cout << "\nPOST REQUEST\n" << std::endl;
        //search the body in the header
        size_t pos = buffer.find("\r\n\r\n");
        // If i have a body in the post request
        if(pos != std::string::npos)
        {
            // retrieve the body and remove the \r\n\r\n before by adding + 4
            std::string body = buffer.substr(pos + 4, buffer.size());
            std::string header = buffer.substr(0, pos);
            // If I have an upload
            if(header.find("multipart/form-data") != std::string::npos)
            {
                std::cout << "UPLOADING FILE" << std::endl;
                //get the size of the file to upload
                std::cout << header << " and \n\n" << body << std::endl;
                int size = getContentLength(header);
                std::string fileName = getFileName(body);
                fileName = "./www/upload/" + fileName;
                //read all the data of the upload file
                read_full_body(data->sockfd, body, size);
                std::ofstream output(fileName.c_str());
                if(!output.is_open())
                {
                    throw Response::ErrorOpeningFile();
                }
                //put the download data inside a file
                output << body;
                output.close();
                sendPostData("201 Created", "application/json", "", data);
            }
            // If i have a form
            else
            {
                std::string file = "./www/keyvalue.json";
                parsePostBody(body);
                //put all the data from the body inside a file
                translateJson();
                //send response POST
                sendPostData("201 Created", "application/json", readFile(file), data);
            }
        }
        // error post body
        else
        {
            throw Response::ErrorBodyPostRequest();
        }
    }
    else if(typeRequest == "DELETE")
    {}
    else
        std::cout << "404 not found" << std::endl;
    return(false);
}

// Fill from ServerAddr
void Server::createListenAddr(ConfigParser &config)
{
	std::vector<Server>::iterator itbeg = config.getServers().begin();

    //creating the poll
    int epoll_fd = epoll_create(10);
    if (epoll_fd == -1)
        errorCloseEpollFd(epoll_fd, 7);

    //iterate through every server and retrieve information
    this->configuringNetwork(itbeg, config, epoll_fd);

    // EPOLL STARTING
    struct epoll_event events[MAX_EVENTS];

    std::cout << "\nWaiting for connection...\n";

    while (true) {
        //epollwait return a number corresponding to all the files descriptor
        int num_fds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (num_fds == -1) 
            errorCloseEpollFd(epoll_fd, 1);
        // I iterate through each fd
        for (int i = 0; i < num_fds; ++i) 
        {
            t_serverData *info = static_cast<t_serverData*>(events[i].data.ptr);
            int fd = info->sockfd;

            // check if my fd is equal to a socket for handcheck
            if(this->socketfd.find(fd) != this->socketfd.end())
            {
                // listen to add new fd to my epoll structure
                struct sockaddr_in client_addr;
                //new fd_client for communication
                int client_fd = acceptConnection(fd, epoll_fd, client_addr);
                // add new fd to my epoll instance
                struct epoll_event client_event = fillEpoolDataInfo(client_fd, info);
                // add the new fd to be control by my epoll instance
                if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event) == -1)
                    errorCloseEpollFd(epoll_fd, 4);
                std::cout << "\n-----New connection established-----\n";
            }
            //Connection already etablish 
            else
            {
                // check for modification inside the socket
                if(events[i].events & EPOLLIN)
                {
                    std::cout << "\nReading data...\n";
                    //read data
                    std::string path = readingData(fd);
                    if (path.empty())
                        continue;
                    // std::cout << path << std::endl;
                    //response request
                    if(redirectRequest(path, info))
                        continue;
                    close(fd);
                }
            }
        }
    }
}


