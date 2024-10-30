#include "../includes/CGIHandler.hpp"

static std::string	execute(const std::string& script_path, const std::map<std::string, std::string>& env)
{
	// Create the CGI environment
	std::vector<char*> cgi_env;
	for (const auto& [key, value] : env) {
		std::string env_var = key + "=" + value;
		cgi_env.push_back(strdup(env_var.c_str()));
	}
	cgi_env.push_back(NULL);

	// Execute the CGI script
	int pipe_fd[2];
	if (pipe(pipe_fd) == -1)
		throw std::runtime_error("Failed to create pipe");

	pid_t pid = fork();
	if (pid == -1) {
		throw std::runtime_error("Failed to fork");
	} else if (pid == 0)
	{
		// Child process
		dup2(pipe_fd[1], STDOUT_FILENO);
		close(pipe_fd[0]);
		execve(script_path.c_str(), NULL, cgi_env.data());
		exit(1);
	} else
	{
		close(pipe_fd[1]);

		// Read the output from the CGI script
		std::stringstream output;
		char buffer[1024];
		ssize_t bytes_read;
		while ((bytes_read = read(pipe_fd[0], buffer, sizeof(buffer))) > 0)
			output.write(buffer, bytes_read);

		// Clean up
		close(pipe_fd[0]);
		for (char* env_var : cgi_env)
			free(env_var);
		waitpid(pid, NULL, 0);

		return output.str();
	}
}