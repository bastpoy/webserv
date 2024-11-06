#include "../includes/CGIHandler.hpp"

void	CGIHandler::execute(std::string uri, int fd) {
	pid_t pid;
	int pipefd[2];
	std::string	realpath;
	
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	realpath = cwd + uri;
	if (pipe(pipefd) == -1) {
		perror("pipe");
		return ;
	}

	if ((pid = fork()) == -1) {
		perror("fork");
		return ;
	}

	if (pid == 0) {
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[0]);

		setenv("REQUEST_METHOD", "GET", 1);
		setenv("SCRIPT_NAME", uri.c_str(), 1);

		execl("/usr/bin/python3", "python3", realpath.c_str(), NULL);
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
		waitpid(pid, NULL, 0);

		std::string response = "HTTP/1.1 200 OK\r\n";
		response += "Content-Type: text/html\r\n";
		response += "Content-Length: " + to_string(output.str().length()) + "\r\n";
		response += "\r\n";
		response += output.str(); 

		if(send(fd, response.c_str(), response.size(), 0) < 0)
		{
			std::cout << strerror(errno) << std::endl;
			throw Response::ErrorSendingResponse(); 
		}
	}
}