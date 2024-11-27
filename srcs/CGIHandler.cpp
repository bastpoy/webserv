#include "Header.hpp"

#define PATH_CGI_IN "./cgi/filein"
#define PATH_CGI_OUT "./cgi/fileout"
#define PATH_CGI_ERR "./cgi/fileerr"
#define TIME_OUT_CGI_MS 10000000


std::string	execute1(std::string uri, std::string &code) {
	pid_t pid;
	int pipefd[2];

	if (pipe(pipefd) == -1) {
		perror("pipe");
		close(pipefd[0]);
		close(pipefd[1]);
		return NULL;
	}

	if ((pid = fork()) == -1) {
		perror("fork");
		close(pipefd[0]);
		close(pipefd[1]);
		return NULL;
	}

	if (pid == 0) {
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[0]);
		close(pipefd[1]);

		setenv("REQUEST_METHOD", "GET", 1);
		setenv("SCRIPT_NAME", uri.c_str(), 1);

		execl("/usr/bin/python3", "python3", uri.c_str(), NULL);
		perror("execl");
		exit(1);
	} else {
		close(pipefd[1]);
		char buffer[1024];
		std::stringstream output;
		ssize_t bytes_read;

		while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
			buffer[bytes_read] = '\0';
			output << buffer;
		}

		close(pipefd[0]);

		// struct sigaction sa;
		// sa.sa_handler = SIG_DFL;
		// sigemptyset(&sa.sa_mask);
		// sa.sa_flags = 0;

		// sigaction(SIGCHLD, &sa, NULL);

		// if (waitpid(pid, NULL, WNOHANG) == -1)
		// {
		// 	perror("waitpid");
		// 	code = "500 Internal Server Error";
		// 	return NULL;
		// }
		
		// signal(SIGCHLD, SIG_IGN);

		code = "200 OK";
		return (output.str());
	}
}

std::string execute(std::string uri, std::string &code, t_serverData *data) {
	pid_t pid;
	int pipefd[2];

	if (pipe(pipefd) == -1) {
		perror("pipe");
		return "";
	}

	if ((pid = fork()) == -1) {
		perror("fork");
		close(pipefd[0]);
		close(pipefd[1]);
		return "";
	}

	if (pid == 0) {
		// Processus enfant
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[0]);
		close(pipefd[1]);

		setenv("REQUEST_METHOD", "GET", 1);
		setenv("SCRIPT_NAME", uri.c_str(), 1);
        // std::cerr << "cgi uri: " << uri << std::endl;
        char **script = (char **)malloc(sizeof(char*) * 3);
        script[0] = strdup("/usr/bin/python3");
        script[1] = strdup(uri.c_str());  // Creates a new C-string
        script[2] = NULL;
        // std::cerr << "rererererererer " << *script << " re" << std::endl;
        if(execve("/usr/bin/python3", script, NULL) < 0)
        {
            std::cerr << strerror(errno) << std::endl;
        }
		// execl("/usr/bin/python3", "python3", uri.c_str(), NULL);
		perror("execl");
		exit(1);
	} 
    else 
    {
		close(pipefd[1]);
		char buffer[1024];  
		std::stringstream output;
		ssize_t bytes_read;

		fd_set set;
		struct timeval timeout;

		FD_ZERO(&set);
		FD_SET(pipefd[0], &set);

		timeout.tv_sec = 5;  // Timeout de 10 secondes
		timeout.tv_usec = 0;
    
		int value = select(pipefd[0] + 1, &set, NULL, NULL, &timeout);
		if (value == -1) {
			perror("select");
			close(pipefd[0]);
			errorPage("500", data);
		}
        else if (value == 0) 
        {
            std::cerr << "here before error " << pipefd[0] << " " << pid <<  std::endl;
            if(kill(pid, SIGKILL) == -1)
            {
                std::cout << "error killing process: " << strerror(errno) << std::endl;
            }
			close(pipefd[0]);
            errorPage("504", data);
		}

		// Lecture des données
		int	total_bytes_read = 0;
		while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
			buffer[bytes_read] = '\0';
			output << buffer;
			if (total_bytes_read > ft_stoi(data->maxBody)) {
				close(pipefd[0]);
				errorPage("413", data);
			}
		}

		close(pipefd[0]);
        if(kill(pid, SIGKILL) == -1)
        {
            std::cout << "error killing process: " << strerror(errno) << std::endl;
        }

		code = "200 OK";
		return output.str();
	}
}





//tfreydi functions

pid_t    executeCGI(std::string uri, t_serverData *data)
{
    struct epoll_event client_event;


    int fd[2];
    //pipe my new script created
    if(pipe(fd) < 0)
    {
        std::cerr << "error creating pipe " << strerror(errno) << std::endl;
        errorPage("500", data);
    }
    client_event.events = EPOLLIN;
    client_event.data.fd = fd[0];
    //i add new file descriptor to my actual epoll instance
    if(epoll_ctl(3, EPOLL_CTL_ADD, fd[0], &client_event))
    {
        std::cerr << "error adding epoll ctl " << strerror(errno) << std::endl;
        errorPage("500", data);
    }

    int pid = fork();
    if (pid < 0)
    {
        std::cout << "Fork failed" << std::endl; //oh no !
        return pid;
        //this will be an error 500 !
    }
    else if (pid == 0)
    {
        char **script = (char **)malloc(sizeof(char*) * 3);
        
        script[0] = strdup("/usr/bin/python3");
        script[1] = strdup(uri.c_str());  // Creates a new C-string
        script[2] = NULL;
        if(dup2(fd[1], STDOUT_FILENO) < 0)
        {
            std::cerr << "Error dup inside CGI" << strerror(errno) << std::endl;
            errorPage("500", data);
        }
        //Modern dup    
        // std::freopen(PATH_CGI_IN, "r", stdin);
		// std::freopen(PATH_CGI_OUT, "w", stdout);
		// std::freopen(PATH_CGI_ERR, "w", stderr);
        execve("/usr/bin/python3", script, NULL);
        std::cerr << "failed to execve, path was : " << uri << std::endl;
        perror("execve");
        std::exit(EXIT_FAILURE);
    }
    return (pid);
}

pid_t    executeTimeOut()
{
    pid_t pidTimeOut = fork();
	if (pidTimeOut == -1) 
    {
		std::cout << "Fork failed" << std::endl; //oh no !
		return pidTimeOut;
	}
	else if (pidTimeOut == 0) 
    {
	    usleep(TIME_OUT_CGI_MS);
		std::exit(EXIT_SUCCESS);
	}
	return (pidTimeOut);
}

bool HandleCgiRequest(std::string uri, t_serverData *data)
{
    // pid_t first_child_pid;
    int   status;
    bool  cgi_success;

    //A LOT OF PARSING WILL HAPPEN HERE TO SPLIT PATH INTO EXE AND PARAMETERS
    //Does path have to take into account what root is defined as ?
    std::cout << "hi whats up cgi handler here path is |" << uri << "|" << std::endl;

    //Execute the fucking cgi;
    pid_t cgi_pid = executeCGI(uri, data); 
    // pid_t timeout_pid = executeTimeOut();

    pid_t RaceWinnerPid = waitpid(-1, &status, WUNTRACED);
    if (RaceWinnerPid == cgi_pid) 
    {
		std::cout << "process won" << std::endl;
        // if (kill(timeout_pid,SIGKILL) == -1) 
		// 	std::cerr << "Kill failed" << std::endl;
        cgi_success = true;
	}
	else 
    {
		std::cout << "timeout won" << std::endl;
        if (kill(cgi_pid, SIGKILL) == -1)
            std::cerr << "Kill failed" << std::endl;
        cgi_success = false;
	}
    std::cout << "Waiting for the Process !" << std::endl;
    waitpid(-1, NULL, WUNTRACED);
    std::cout << "Waited for the Process ! Returning bool of : " << cgi_success << std::endl;
    return cgi_success; //Returning void for now, I can potentially return the status of the process.

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
    return (buffer.str()); // sex
}

std::string    cgiProtocol(std::string uri, std::string &code, t_serverData *data)
{
    std::string response;
    
    if (HandleCgiRequest(uri, data) == false)
    {
        errorPage("504", data);
    }
    else
    {
        std::string cgi_output = fileToString(PATH_CGI_OUT);
        code = "200 OK";
        return(cgi_output);
    }
    return "";
}