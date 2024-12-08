#include "Header.hpp"

void	SIGINT_handler(int signal)
{
	(void)signal;
    GlobalLinkedList::cleanup();
	std::cout << "\nCTRL+C" << std::endl;
}

void	SIGQUIT_handler(int signal)
{
	(void)signal;
    GlobalLinkedList::cleanup();
	std::cout << "\nCTRL+\\" << std::endl;
}

void	SIGTERM_handler(int signal)
{
	switch (signal){
		case SIGINT:
		{
			std::cout << "\nCTRL+D" << std::endl;
		}
	}
}

void	SIGCHLD_handler(int signal)
{
	int pid;
	int status;

	(void)signal;
	while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
	{
		if (WIFEXITED(status))
			std::cout << "1 Child process " << pid << " terminated with status: " << WEXITSTATUS(status) << std::endl;
		if (WIFSIGNALED(status))
			std::cout << "2 Child process " << pid << " terminated with status: " << WEXITSTATUS(status) << std::endl;
		if (WIFSTOPPED(status))
			std::cout << "3 Child process " << pid << " terminated with status: " << WEXITSTATUS(status) << std::endl;
	}
}

void	configureSignals()
{
	std::signal(SIGINT, SIGINT_handler);
	std::signal(SIGTERM, SIGTERM_handler);
	std::signal(SIGQUIT, SIGQUIT_handler);
	// std::signal(SIGCHLD, SIGCHLD_handler);
}