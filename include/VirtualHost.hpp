#ifndef VIRTUALHOST_HPP
# define VIRTUALHOST_HPP

# include "Header.hpp"

/*
But : Gérer plusieurs configurations de serveur virtuel (plusieurs noms de domaines sur le même serveur).
Rôle :
Permettre à un serveur unique de gérer plusieurs configurations basées sur le nom de domaine ou l’adresse IP.
*/
class VirtualHost
{
	private:
		std::string	_domain_name;
		std::string	_root_directory;
		std::string	_error_pages;
	public:
		void	matchRequest(); // pour vérifier si la requête correspond au virtual host.
};

# endif