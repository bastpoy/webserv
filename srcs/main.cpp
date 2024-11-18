#include "Header.hpp"


t_serverData	*singleton_data(t_serverData *data)
{
	static t_serverData *data_todeliver;

	if (data)
		data_todeliver = data;
	return (data_todeliver);
}

int main(int ac, char **av)
{

	try
	{
		configureSignals();
		// Vérifier si un fichier de configuration a été passé en argument
		if (ac < 2)
			throw std::runtime_error("Usage: <program_name> <config_file>");

		ConfigParser	config(av[1]); // Initialiser ConfigParser avec le fichier de configuration fourni
		Server			listenAddress;

		config.parseConfig();
		config.printConfig();
		listenAddress.createListenAddr(config);
	}
	catch (const std::exception& e)
	{
		std::cerr << RED << "Error: " << e.what() << RESET << std::endl;
	}
	return (0);
}
