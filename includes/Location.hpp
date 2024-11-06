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
		Location();
		~Location();
		// Location(const Location &other);
		// Location	&operator=(const Location &other);

		// Setter
		void	setPath(std::string path);
		void    setRoot(std::string root);
		void	setIndex(std::string index);
		void	setMaxBody(std::string maxBody);
		void	setAutoIndex(std::string autoIndex);
		void	setRedir(int code, std::string path);
		void	setErrorPage(int errCode, std::string errorPage);
		// Getter
		std::string					getPath() const;
		std::string					getRoot() const;
		std::string					getIndex() const;
		std::string					getMaxBody() const;
		std::string					getAutoIndex() const;
		std::map<int,std::string>	&getRedir();
		std::map<int,std::string>	&getErrorPage();

		// Fill
		void	fillPath(std::string line);
		void    fillRoot(std::string line);
		void	fillIndex(std::string line);
		void	fillMaxBody(std::string line);
		void	fillAutoIndex(std::string line);
		void	fillRedir(std::string line, Server *server);
		void	fillErrorPage(std::string line, Server *server);

		// Debug
		void	printConfig();
		
};

#endif /* LOCATION_HPP */