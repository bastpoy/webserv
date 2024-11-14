#include "../includes/CGIHandler.hpp"

std::string	CGIHandler::execute(std::string uri, std::string &code) {
	pid_t pid;
	int pipefd[2];

	if (pipe(pipefd) == -1) {
		perror("pipe");
		return NULL;
	}

	if ((pid = fork()) == -1) {
		perror("fork");
		return NULL;
	}

	if (pid == 0) {
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[0]);

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
		waitpid(pid, NULL, 0);

		code = "200 OK";
		return (output.str());
	}
}