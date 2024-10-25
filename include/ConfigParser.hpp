#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "Header.hpp"

/*
But : Charger et analyser les fichiers de configuration.
Rôle :
Lire les paramètres de configuration (port, racine du serveur, paramètres des serveurs virtuels).
Faciliter la configuration flexible du serveur.
*/
class Config
{
	int		_port;
	int		_server_root;
public:
	void	parseConfigFile(); // pour analyser le fichier de configuration et remplir les attributs.
};

# endif