#ifndef VIRTUALHOST_HPP
# define VIRTUALHOST_HPP

# include "Header.hpp"

/*
But : Gérer plusieurs configurations de serveur virtuel (plusieurs noms de domaines sur le même serveur).
Rôle :
Permettre à un serveur unique de gérer plusieurs configurations basées sur le nom de domaine ou l’adresse IP.
*/

/*
Gestion des hôtes virtuels (multi-domaine)
Représente un hôte virtuel, avec son propre ensemble de règles de configuration (par exemple, pour chaque domaine ou sous-domaine).
Permet de servir plusieurs sites web ou applications sur le même serveur.
*/
class VirtualHost
{
	private:
		std::string						_domain_name;
		std::string						_root_directory;
		std::string						_error_pages;
		std::map<std::string, Location>	_locations; // Si besoin
	public:
		void	matchRequest(); // pour vérifier si la requête correspond au virtual host.
		// or/and
		const Location& matchLocation(const std::string& path) const; // pour vérifier si la requête correspond à une location.
};

# endif