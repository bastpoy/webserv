#include "Header.hpp"

/**
 * @brief	This function will fill the data structure for the CGI.
 * @details	It will fill the data structure with the data from the server.
 * @param	importData	The data to import.
 * @param	cgi			The CGI structure.
 * @param	fdEpollLink	The map of file descriptors.
*/
struct epoll_event fillDataCgi(t_serverData *importData, t_cgi *cgi, std::map<int, t_serverData*> &fdEpollLink)
{
	t_serverData					*data = new t_serverData;
	struct epoll_event				client_event;
	std::pair<int, t_serverData*>	pair(data->sockfd, data);

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
	data->isDownload = importData->isDownload;

	client_event.events = EPOLLIN;
	client_event.data.ptr = static_cast<void*>(data);

	fdEpollLink.insert(pair);
	GlobalLinkedList::insert(data);
	return (client_event);
}

/**
 * @brief	This function will create a new CGI structure.
 * @param	fd				The file descriptor.
 * @param	pid				The process id.
 * @param	time			The time.
 * @param	parentSocket	The parent socket.
*/
t_cgi * new_cgi(int fd, int pid, time_t time, int parentSocket)
{
	t_cgi *newcgi = new t_cgi;
	newcgi->cgifd = fd;
	newcgi->cgipid = pid;
	newcgi->cgiTimeout = time;
	newcgi->parentsocket = parentSocket;
	return (newcgi);
}

void	executeCGI(std::string uri, t_serverData *&data, std::map<int, t_serverData*> &fdEpollLink)
{
	int			fd[2];
	std::string	extension = CGIExtension(uri);
	std::map<std::string, std::string>::const_iterator it = data->cgiPath.find(extension);

	std::cout << RED "extension is " << extension << RESET << std::endl;
	if (it == data->cgiPath.end())
	{
		std::cout << "Error : can't find extension " << extension << std::endl;
		errorPage("501", data);
	}
	if(pipe(fd) < 0)
	{
		std::cout << "error creating pipe " << strerror(errno) << std::endl;
		errorPage("500", data);
	}
	int pid = fork();
	if (pid < 0)
	{
		std::cout << "Fork failed" << std::endl; 
	}
	else if (pid == 0)
	{
		char **script = (char **)malloc(sizeof(char*) * 3);
		
		script[0] = strdup(it->second.c_str());
		script[1] = strdup(uri.c_str());
		script[2] = NULL;
		if(dup2(fd[1], STDOUT_FILENO) < 0)
		{
			std::cout << "Error dup inside CGI" << strerror(errno) << std::endl;
			errorPage("500", data);
		}
		close(fd[0]);
		close(fd[1]);
		execve(it->second.c_str(), script, NULL);
		perror("execve");
		std::exit(EXIT_FAILURE);
	}
	close(fd[1]);
	struct epoll_event client_event;

	t_cgi *cgi = new_cgi(fd[0], pid, time(NULL) + 5, data->sockfd);
	client_event = fillDataCgi(data, cgi, fdEpollLink);
	if(epoll_ctl(3, EPOLL_CTL_ADD, fd[0], &client_event) < 0)
	{
		std::cout << "error adding epoll ctl " << strerror(errno) << std::endl;
		errorPage("500", data);
	}
	data->isCgi = cgi;
}

std::string HandleCgiRequest(std::string uri, t_serverData *&data, std::map<int, t_serverData*> &fdEpollLink)
{
	std::cout << "hi whats up cgi handler here path is |" << uri << "|" << std::endl;

	executeCGI(uri, data, fdEpollLink);
	throw Response::responseOk();
	return "";
}

void check_timeout_cgi(t_serverData *info, std::map<int, t_serverData*> &fdEpollLink, struct epoll_event *events, int i, int epoll_fd)
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
				std::cout << RED "a cgi is TIMEOUT" << RESET << std::endl;
				std::string response = httpGetResponse("200 Ok", "text/html", readFile("./www/error/408.html", it->second), it->second, "");
				if(send(it->second->sockfd, response.c_str(), response.size(), 0) < 0)
				{
					std::cout << RED "error send main "<< errno << " " << strerror(errno) << RESET << std::endl;
					errorPage("500", info);
				}
				close(it->second->cgi->cgifd);
				//kill the pipe
				kill(it->second->cgi->cgipid, SIGKILL);
				int status;
				waitpid(it->second->cgi->cgipid, &status, 0);

				events[i].events = EPOLLIN;
				if(epoll_ctl(epoll_fd, EPOLL_CTL_MOD, it->second->sockfd, events) < 0)
				{
					std::cout << RED "Error epoll ctl catch: "<< errno << " " << strerror(errno) << RESET << std::endl;
					errorPage("500", it->second);
				}
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

void read_cgi(t_serverData *data, struct epoll_event *events, int i, int epoll_fd)
{
	// I read my cgi which is finish
	char buffer[4096];
	int bytes_read;

	std::cout << YELLOW "Reading cgi" << RESET << std::endl;
	bytes_read = read(data->cgi->cgifd, buffer, 4096);
	// std::cout << buffer << std::endl;
	if(bytes_read < 1)
	{
		std::cerr << RED "error reading the cgi: " << strerror(errno) << RESET << std::endl; 
	}
	//i put the content of the cgi response in the body
	data->body.append(buffer, bytes_read);
	//switching to epollout
	events[i].events = EPOLLOUT;
	if(epoll_ctl(epoll_fd, EPOLL_CTL_MOD, data->cgi->cgifd, events) < 0)
	{
		std::cout << RED "Error epoll ctl catch: "<< errno << " " << strerror(errno) << RESET << std::endl;
		errorPage("500", data);
	}
}
