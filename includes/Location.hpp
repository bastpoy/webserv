#ifndef LOCATION_HPP
# define LOCATION_HPP

#include "Header.hpp"

class Server;

class Location
{
	private:
		std::string					_path;
		std::string					_root;
		std::string					_index;
		std::string					_maxBody;
		std::string					_autoindex;
		std::map<int,std::string>	_redir;
		std::map<int, std::string>	_errorPage;

	public:
		// Canonical form
		Location(void);
		~Location(void);

		// Setter
		void	setPath(std::string path);
		void	setRoot(std::string root);
		void	setIndex(std::string index);
		void	setMaxBody(std::string maxBody);
		void	setAutoIndex(std::string autoIndex);
		void	setRedir(int code, std::string path);
		void	setErrorPage(int errCode, std::string errorPage);

		// Getter
		std::string					getPath(void) const;
		std::string					getRoot(void) const;
		std::string					getIndex(void) const;
		std::string					getMaxBody(void) const;
		std::string					getAutoIndex(void) const;
		std::map<int,std::string>	&getRedir(void);
		std::map<int,std::string>	&getErrorPage(void);

		// Fill
		void	fillPath(std::string line);
		void	fillRoot(std::string line);
		void	fillIndex(std::string line);
		void	fillMaxBody(std::string line);
		void	fillAutoIndex(std::string line);
		void	fillRedir(std::string line);
		void	fillErrorPage(std::string line);

		// Debug
		void	checkNotEmptys(void);
		void	printConfig(void);
		
};

#endif /* LOCATION_HPP */