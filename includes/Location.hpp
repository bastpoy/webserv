#ifndef LOCATION_HPP
# define LOCATION_HPP

#include "Header.hpp"

class Location
{
	private:
		std::string					path;
		std::string					index;
		std::string					maxBody;
		std::string					autoindex;
		std::map<int,std::string>	redir;
		std::map<int, std::string>	errorPage;

	public:
		// Canonical form
		Location();
		~Location();
		// location(const location &other);
		// location	&operator=(const location &other);

		// Setter
		void	setPath(std::string path);
		void	setIndex(std::string index);
		void	setMaxBody(std::string maxBody);
		void	setAutoIndex(std::string autoIndex);
		void	setRedir(int code, std::string path);
		void	setErrorPage(int errCode, std::string errorPage);

		// Getter
		std::string					getPath() const;
		std::string					getIndex() const;
		std::string					getMaxBody() const;
		std::string					getAutoIndex() const;
		std::map<int,std::string>	&getRedir();
		std::map<int,std::string>	&getErrorPage();

		// Fill
		void	fillPath(std::string line);
		void	fillIndex(std::string line);
		void	fillMaxBody(std::string line);
		void	fillAutoIndex(std::string line);
		void	fillRedir(std::string line, Server *server);
		void	fillErrorPage(std::string line, Server *server);

		// Debug
		void	printConfig();
		
};

#endif /* LOCATION_HPP */