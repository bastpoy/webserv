#include "Header.hpp"

#define TIME_OUT_CGI_MS 10000000


struct epoll_event fillDataCgi(t_serverData *importData, t_cgi *cgi, std::map<int, t_serverData*> &fdEpollLink)
{
	t_serverData *data = new t_serverData;

	data->sockfd = importData->sockfd;
	data->port = importData->port;
	data->server_name = importData->server_name;
	data->path = importData->path;
	data->maxBody = importData->maxBody;
	data->autoindex = importData->autoindex;
	data->index = importData->index;
	data->errorPage = importData->errorPage;
	data->cgiPath = importData->cgiPath;
	data->redir = importData->redir;
	data->location = importData->location;
    data->buffer = "";
    data->header = "";
    data->body = "";
    data->cgi = cgi;
    // data->requestAllow.push_back("GET");

	struct epoll_event client_event;

	client_event.events = EPOLLIN;
    client_event.data.fd = importData->sockfd;
	client_event.data.ptr = static_cast<void*>(data);

    //add new elements to the map structur
    std::pair<int, t_serverData*> pair(data->sockfd, data);
    fdEpollLink.insert(pair);
    GlobalLinkedList::insert(data);
	return(client_event);
}

t_cgi * new_cgi(int fd, int pid, time_t time, int parentSocket)
{
    t_cgi *newcgi = new t_cgi;
    newcgi->cgifd = fd;
    newcgi->cgipid = pid;
    newcgi->cgiTimeout = time;
    newcgi->parentsocket = parentSocket;
    return (newcgi);
}

pid_t    executeCGI(std::string uri, t_serverData *data, std::map<int, t_serverData*> &fdEpollLink)
{
    int fd[2];
    std::string extension = CGIExtension(uri);
    std::map<std::string, std::string>::const_iterator it = data->cgiPath.find(extension);
    if (it == data->cgiPath.end())
    {
        std::cerr << "Error : can't find extension " << extension << std::endl;
        // free(script); // Libérer la mémoire
        errorPage("501", data);
        throw Response::Error();
        // std::exit(EXIT_FAILURE);
    }
    //pipe my new script created
    if(pipe(fd) < 0)
    {
        std::cerr << "error creating pipe " << strerror(errno) << std::endl;
        errorPage("500", data);
    }

    int pid = fork();
    if (pid < 0)
    {
        std::cout << "Fork failed" << std::endl; 
        return pid;
        //this will be an error 500 !
    }
    else if (pid == 0)
    {
        char **script = (char **)malloc(sizeof(char*) * 3);
        
        // Préparer les arguments pour execve
        script[0] = strdup(it->second.c_str());
        script[1] = strdup(uri.c_str());  // Creates a new C-string
        script[2] = NULL;
        if(dup2(fd[1], STDOUT_FILENO) < 0)
        {
            std::cerr << "Error dup inside CGI" << strerror(errno) << std::endl;
            errorPage("500", data);
        }
        close(fd[0]);
		close(fd[1]);
        execve(it->second.c_str(), script, NULL);
        std::cerr << "failed to execve, path was : " << uri << std::endl;
        std::cout << MAGENTA << "it: " << it->first << RESET << std::endl;
        perror("execve");
        std::exit(EXIT_FAILURE);
    }
    close(fd[1]);
    // creating an instance of a cgi and adding it to epoll instance;
    struct epoll_event client_event;

    t_cgi *cgi = new_cgi(fd[0], pid, time(NULL) + 5, data->sockfd);
    //create a new client
    client_event = fillDataCgi(data, cgi, fdEpollLink);
    // deleting the previous fd which is my fd client
    // data->fdEpollLink->erase(data->sockfd);
    if(epoll_ctl(3, EPOLL_CTL_DEL, data->sockfd, NULL) < 0)
    {
        std::cerr << "error deleting epoll ctl " << strerror(errno) << std::endl;
        errorPage("500", data);
    }
    //I add new file descriptor to my actual epoll instance
    if(epoll_ctl(3, EPOLL_CTL_ADD, fd[0], &client_event) < 0)
    {
        std::cerr << "error adding epoll ctl " << strerror(errno) << std::endl;
        errorPage("500", data);
    }
    //adding my cgi to my current socket;
    data->cgi = new_cgi(fd[0], pid, cgi->cgiTimeout, data->sockfd);
    return (pid);
}

std::string HandleCgiRequest(std::string uri, t_serverData *data, std::map<int, t_serverData*> &fdEpollLink)
{
    std::cout << "hi whats up cgi handler here path is |" << uri << "|" << std::endl;

    executeCGI(uri, data, fdEpollLink); 
    throw Response::responseOk();
    return "";
}

std::string fileToString(const char *filePath)
{
    std::ifstream inputFile(filePath);

    if (!inputFile.is_open())
    {
        throw std::runtime_error("Failed to open the file: " + std::string(filePath));
    } // love

    std::stringstream buffer;
    buffer << inputFile.rdbuf(); //gets all content of the file and puts it into buffer;
    return (buffer.str());
}

void check_timeout_cgi(t_serverData *info, std::map<int, t_serverData*> &fdEpollLink)
{
    if(info)
    {
        if(info->cgi == NULL)
        {
            std::map<int, t_serverData*>::iterator it = fdEpollLink.begin();
            //iterate through my corresponse map between fd and data struct
            while (it != fdEpollLink.end()) 
            {
                //if i have a cgi inside my struct
                if(it->second->cgi)
                {
                    //if my cgi is timeout
                    if(it->second->cgi->cgiTimeout < time(NULL))
                    {
                        std::cout << "a cgi is TIMEOUT" << std::endl;
                        std::string response = httpGetResponse("200 Ok", "text/html", readFile("./www/error/error408.html", it->second), it->second);
                        if(send(it->second->sockfd, response.c_str(), response.size(), 0) < 0)
                        {
                            std::cout << RED "error send main "<< errno << " " << strerror(errno) << RESET << std::endl;
                        }
                        close(it->second->cgi->cgifd);
                        close(it->second->sockfd);
                        delete it->second->cgi;
                        it->second->cgi = NULL;
                        std::map<int, t_serverData*>::iterator toErase = it;
                        it++;
                        fdEpollLink.erase(toErase);
                        continue;
                    }
                }
                it++;
            }
        }
    }
}

void read_cgi(t_serverData *data, struct epoll_event *events, int i, int epoll_fd)
{
	//i read my cgi which is finish
	char buffer[4096];
	int bytes_read;

	std::cout << YELLOW "before reading cgi" << RESET << std::endl;
	bytes_read = read(data->cgi->cgifd, buffer, 4096);
	if(bytes_read < 1)
	{
		std::cerr << RED "error reading the cgi: " << strerror(errno) << RESET << std::endl; 
	}
	std::cout << GREEN "reading from the cgi fd" << RESET << std::endl;
	//i put the content of the cgi response in the body
	data->body.append(buffer, bytes_read);
	//switching to epollout
	events[i].events = EPOLLOUT;
	if(epoll_ctl(epoll_fd, EPOLL_CTL_MOD, data->sockfd, events) < 0)
    {
        std::cout << RED "Error epoll ctl catch: "<< errno << " " << strerror(errno) << RESET << std::endl;
    }
}