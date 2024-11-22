#include "Header.hpp"

/* ================ */
/*	CANONICAL FORMS	*/
/* ================ */

Location::Location()
{
	// std::cout << GREEN "Creating a Location configuration" RESET << std::endl;
}

Location::~Location()
{
	// std::cout << RED "Destroying a Location configuration" RESET << std::endl;
}

/* ================ */
/*		SETTER		*/
/* ================ */

void	Location::setPath(std::string path)
{
	path.erase(std::remove(path.begin(), path.end(), ' '), path.end());
	this->_path = path;
}

void	Location::setRoot(std::string root)
{
	root.erase(std::remove(root.begin(), root.end(), ' '), root.end());
	this->_root = root;
}

void	Location::setIndex(std::string index)
{
	index.erase(std::remove(index.begin(), index.end(), ' '), index.end());
	this->_index = index;
}

void	Location::setMaxBody(std::string maxBody)
{
	maxBody.erase(std::remove(maxBody.begin(), maxBody.end(), ' '), maxBody.end());
	this->_maxBody = maxBody;
}

void	Location::setAutoIndex(std::string autoIndex)
{
	autoIndex.erase(std::remove(autoIndex.begin(), autoIndex.end(), ' '), autoIndex.end());
	this->_autoindex = autoIndex;
}

void	Location::setRedir(std::string code, std::string domain)
{
	domain.erase(std::remove(domain.begin(), domain.end(), ' '), domain.end());
    //if i have already a redirection i delete it and replace it
    if(this->getRedir().size())
        this->_redir.erase(this->_redir.begin());
    //add the new redirection
    this->_redir.insert(std::make_pair(code, domain));
}

void	Location::setErrorPage(std::string code, std::string errorFile)
{
	errorFile.erase(std::remove(errorFile.begin(), errorFile.end(), ' '), errorFile.end());
	this->_errorPage.insert(std::make_pair(code, errorFile));
}

/* ================ */
/*		GETTER		*/
/* ================ */

std::string Location::getPath() const
{
	return (this->_path);
}

std::string Location::getRoot() const
{
	return(this->_root);
}

std::string Location::getIndex() const
{
	return (this->_index);
}

std::string Location::getMaxBody() const
{
	return (this->_maxBody);
}

std::string Location::getAutoIndex() const
{
	return (this->_autoindex);
}

std::map<std::string,std::string> &Location::getRedir()
{
	return (this->_redir);
}

std::map<std::string,std::string> &Location::getErrorPage()
{
	return (this->_errorPage);
}

/* ================ */
/*		FILL		*/
/* ================ */

void	Location::fillPath(std::string line)
{
	size_t pos = line.find("location ") + strlen("location ");
	// if(line.find(" {") == std::string::npos)
	// 	throw Response::ConfigurationFileLocation(); 
	size_t pos2 = line.find(" {");
	this->setPath(line.substr(pos, pos2 - pos));
	if(this->getPath().at(this->getPath().size() - 1) != '/')
	{
		this->setPath(this->getPath() + "/");
	}
	// std::cout << "the path:\t\t" YELLOW << this->getPath() << RESET << std::endl;
}

void Location::fillRoot(std::string line)
{
	size_t pos = line.find("root ");
	this->setRoot(line.substr(pos + strlen("root"), line.length() - (pos + strlen("root"))));
	if(this->getRoot().at(this->getRoot().size() - 1) != '/')
		this->setRoot(this->getRoot() + "/");
	if(this->getRoot().at(0) != '.')
		this->setRoot("." + this->getRoot());
	// std::cout << "the root:\t\t" YELLOW << this->getRoot() << RESET << std::endl;
}

void	Location::fillIndex(std::string line)
{
	size_t pos = line.find("index");
	this->setIndex(line.substr(pos + strlen("index"), line.length() - (pos + strlen("index"))));
	// std::cout << "the index is:\t\t" YELLOW << this->getIndex() << RESET << std::endl;
}

void	Location::fillMaxBody(std::string line)
{
	size_t pos = line.find("client_max_body_size");
	this->setMaxBody(line.substr(pos + strlen("client_max_body_size"), line.length() - (pos + strlen("client_max_body_size"))));
	// std::cout << "the maxBody is: " << this->getMaxBody() << std::endl;
}

void	Location::fillAutoIndex(std::string line)
{
	size_t pos = line.find("autoindex");
	this->setAutoIndex(line.substr(pos + strlen("autoindex"), line.length() - (pos + strlen("autoindex"))));
	// std::cout << "The auto index is " << this->getAutoIndex() << std::endl;
}

void	Location::fillRedir(std::string line, Server *server)
{
	size_t pos = line.find("return ");
    if(pos == std::string::npos)
    {
        throw Response::ConfigurationFileLocation();
    }
	std::string code = line.substr(pos + strlen("return "), 3);
	std::string domain = line.substr(pos + strlen("return ") + 4, line.length() - (pos + strlen("return")));
	this->setRedir(code, domain);

	(void)server;
	//print
	// std::map<std::string, std::string>::iterator it = server->getRedir().begin();
	// std::cout << "the code is: " << it->first << "\t the domain is: " << it->second <<  std::endl;
}

void	Location::fillErrorPage(std::string line, Server *server)
{
	size_t pos = line.find("error_page ");
	std::string code = line.substr(pos + strlen("error_page "), 3).c_str();
	std::string domain = line.substr(pos + strlen("error_page ") + 3, line.length());
	this->setErrorPage(code, domain);

	(void)server;
	//print
	// std::map<int, std::string>::iterator it = server->getErrorPage().begin();
	// std::cout << "the errorCode is:\t" YELLOW << it->first << RESET " (" YELLOW << it->second << RESET ")" << std::endl;
}

/* ================ */
/*		DEBUG		*/
/* ================ */

void	Location::printConfig()
{
	if(!this->getPath().empty())
		std::cout << "\tPath\t\t" YELLOW << this->getPath() << RESET << std::endl;
	if(!this->getAutoIndex().empty())
		std::cout << "\tautoindex\t" YELLOW << this->getAutoIndex() << RESET << std::endl;
	if(!this->getIndex().empty())
		std::cout << "\tindex\t\t" YELLOW << this->getIndex() << RESET << std::endl;
	if(!this->getMaxBody().empty())
		std::cout << "\tclient_max_body_size\t\t" YELLOW << this->getMaxBody() << RESET << std::endl;
	if(this->getErrorPage().size()) 
		std::cout << "\terror_page\t" YELLOW << this->getErrorPage().begin()->first << " " << this->getErrorPage().begin()->second << RESET << std::endl;
	if(this->getRedir().size()) 
		std::cout << "\treturn\t\t" YELLOW << this->getRedir().begin()->first << " " << this->getRedir().begin()->second << RESET << std::endl;
	std::cout << std::endl;
}
