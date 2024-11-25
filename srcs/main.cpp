#include "Header.hpp"

int main(int ac, char **av)
{

	try
	{
		// Vérifier si un fichier de configuration a été passé en argument
		if (ac < 2)
			throw std::runtime_error("Usage: <program_name> <config_file>");

		ConfigParser	config(av[1]); // Initialiser ConfigParser avec le fichier de configuration fourni
		Server			listenAddress;

		config.parseConfig(config.getServers());
		if (config.getServers().empty())
			throw std::runtime_error("No server configuration found in the file");
		config.printConfig();
		listenAddress.createListenAddr(config);
	}
	catch (const std::exception& e)
	{
		std::cerr << RED << "Error: " << e.what() << RESET << std::endl;
	}
	return (0);
}
