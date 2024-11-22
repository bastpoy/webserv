#include "Header.hpp"

// std::string	CGIHandler::execute(std::string uri, std::string &code) {
// 	pid_t pid;
// 	int pipefd[2];

// 	if (pipe(pipefd) == -1) {
// 		perror("pipe");
// 		close(pipefd[0]);
// 		close(pipefd[1]);
// 		return NULL;
// 	}

// 	if ((pid = fork()) == -1) {
// 		perror("fork");
// 		close(pipefd[0]);
// 		close(pipefd[1]);
// 		return NULL;
// 	}

// 	if (pid == 0) {
// 		dup2(pipefd[1], STDOUT_FILENO);
// 		close(pipefd[0]);
// 		close(pipefd[1]);

// 		setenv("REQUEST_METHOD", "GET", 1);
// 		setenv("SCRIPT_NAME", uri.c_str(), 1);

// 		execl("/usr/bin/python3", "python3", uri.c_str(), NULL);
// 		perror("execl");
// 		exit(1);
// 	} else {
// 		close(pipefd[1]);
// 		char buffer[1024];
// 		std::stringstream output;
// 		ssize_t bytes_read;

// 		while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
// 			buffer[bytes_read] = '\0';
// 			output << buffer;
// 		}

// 		close(pipefd[0]);

// 		// struct sigaction sa;
// 		// sa.sa_handler = SIG_DFL;
// 		// sigemptyset(&sa.sa_mask);
// 		// sa.sa_flags = 0;

// 		// sigaction(SIGCHLD, &sa, NULL);

// 		// if (waitpid(pid, NULL, WNOHANG) == -1)
// 		// {
// 		// 	perror("waitpid");
// 		// 	code = "500 Internal Server Error";
// 		// 	return NULL;
// 		// }
		
// 		// signal(SIGCHLD, SIG_IGN);

// 		code = "200 OK";
// 		return (output.str());
// 	}
// }

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
        
		execl("/usr/bin/python3", "python3", uri.c_str(), NULL);
		perror("execl");
		exit(1);
	} else {
		// Processus parent
		close(pipefd[1]);
		char buffer[1024];
		std::stringstream output;
		ssize_t bytes_read;

		fd_set set;
		struct timeval timeout;

		FD_ZERO(&set);
		FD_SET(pipefd[0], &set);

		timeout.tv_sec = 10;  // Timeout de 10 secondes
		timeout.tv_usec = 0;

		int value = select(pipefd[0] + 1, &set, NULL, NULL, &timeout);
		if (value == -1) {
			perror("select");
			close(pipefd[0]);
			errorPage("500", data);
		} else if (value == 0) {
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

		code = "200 OK";
		return output.str();
	}
}