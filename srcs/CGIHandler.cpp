#include "../includes/CGIHandler.hpp"

std::string	CGIHandler::execute(const std::string& script_path, const std::map<std::string, std::string>& env)
{
	std::vector<char*> cgi_env;
	for (std::map<std::string, std::string>::const_iterator it = env.begin(); it != env.end(); ++it) {
		std::string env_var = it->first + "=" + it->second;
		cgi_env.push_back(strdup(env_var.c_str()));
	}
	cgi_env.push_back(NULL);

	int pipe_fd[2];
	if (pipe(pipe_fd) == -1)
		throw std::runtime_error("Failed to create pipe");
	pid_t pid = fork();
	if (pid == -1) {
		throw std::runtime_error("Failed to fork");
	} else if (pid == 0) {
		std::cout << "1" << std::endl;
		dup2(pipe_fd[1], STDOUT_FILENO);
		close(pipe_fd[0]);
		char *argv[] = {NULL};
		execve(script_path.c_str(), argv, &cgi_env[0]);
		exit(1);
	} else {
		close(pipe_fd[1]);

		std::cout << "2" << std::endl;
		std::stringstream output;
		char buffer[1024];
		ssize_t bytes_read;
		while ((bytes_read = read(pipe_fd[0], buffer, sizeof(buffer))) > 0)
			output.write(buffer, bytes_read);

		close(pipe_fd[0]);
		for (char* env_var : cgi_env)
			free(env_var);
		waitpid(pid, NULL, 0);

		return output.str();
	}
}

// std::string CGIHandler::execute(const std::string& path, const std::map<std::string, std::string>& env)
// {
// 	std::vector<char*> cgi_env;
// 	for (std::map<std::string, std::string>::const_iterator it = env.begin(); it != env.end(); ++it) {
// 		std::string env_var = it->first + "=" + it->second;
// 		cgi_env.push_back(strdup(env_var.c_str()));
// 	}
// 	cgi_env.push_back(NULL);

// 	int pipe_fd[2];
// 	if (pipe(pipe_fd) == -1)
// 		throw std::runtime_error("Failed to create pipe");

// 	pid_t pid = fork();
// 	if (pid == -1)
// 		throw std::runtime_error("Failed to fork");
// 	else if (pid == 0)
// 	{
// 		dup2(pipe_fd[1], STDOUT_FILENO);
// 		close(pipe_fd[0]);

// 		char* argv[] = { nullptr };
// 		execve(path.c_str(), argv, &cgi_env[0]);
// 		std::cout << YELLOW << "path: " << path << RESET << std::endl;
// 		exit(1);
// 	} else {
// 		close(pipe_fd[1]);
// 		std::stringstream output;
// 		char buffer[1024];
// 		ssize_t bytes_read;

// 		while ((bytes_read = read(pipe_fd[0], buffer, sizeof(buffer))) > 0)
// 		{
// 			std::cout << RED << "buffer: " << buffer << RESET << std::endl;
// 			output.write(buffer, bytes_read);
// 		}

// 		close(pipe_fd[0]);
// 		for (size_t i = 0; i < cgi_env.size(); ++i)
// 			free(cgi_env[i]);
// 		waitpid(pid, 0, 0);

// 		std::string result = output.str();
// 		printf("output: %s", result.c_str());
// 		// printf("output: %s", output);
// 		std::cout << RED << "result: " << result << RESET << std::endl;
// 		if (result.find("Content-Type: ")) {
// 			std::cerr << "CGI script did not return Content-Type header." << std::endl;
// 		}

// 		return result; // Retourne la réponse du script CGI
// 	}
// }