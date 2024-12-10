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

	struct epoll_event client_event;

	client_event.events = EPOLLIN;
	client_event.data.fd = importData->sockfd;
	client_event.data.ptr = static_cast<void*>(data);

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
		errorPage("501", data);
		throw Response::Error();
	}
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
	}
	else if (pid == 0)
	{
		char **script = (char **)malloc(sizeof(char*) * 3);
		
		script[0] = strdup(it->second.c_str());
		script[1] = strdup(uri.c_str());
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
	struct epoll_event client_event;

	t_cgi *cgi = new_cgi(fd[0], pid, time(NULL) + 5, data->sockfd);
	client_event = fillDataCgi(data, cgi, fdEpollLink);
	if(epoll_ctl(3, EPOLL_CTL_DEL, data->sockfd, NULL) < 0)
	{
		std::cerr << "error deleting epoll ctl " << strerror(errno) << std::endl;
		errorPage("500", data);
	}
	if(epoll_ctl(3, EPOLL_CTL_ADD, fd[0], &client_event) < 0)
	{
		std::cerr << "error adding epoll ctl " << strerror(errno) << std::endl;
		errorPage("500", data);
	}
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
	}

	std::stringstream buffer;
	buffer << inputFile.rdbuf();
	return (buffer.str());
}
