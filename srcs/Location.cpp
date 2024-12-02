#include "Header.hpp"

/* ================ */
/*	CANONICAL FORMS	*/
/* ================ */

Location::Location() {}

Location::~Location() {}

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

void	Location::setCgiPath(std::string language, std::string path)
{
	this->_cgi_path.insert(std::make_pair(language, path));
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

std::map<std::string,std::string> &Location::getCgiPath()
{
	return (this->_cgi_path);
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
	size_t pos = line.find("location") + strlen("location");

	size_t pos2 = line.find("{");
	std::string path = line.substr(pos, pos2 - pos);

	path.erase(std::remove(path.begin(), path.end(), ' '), path.end());
	path.erase(std::remove(path.begin(), path.end(), '\t'), path.end());

	if (path.empty())
		throw Response::ConfigurationFileLocation("path is empty");

	this->setPath(path);
	if (this->getPath().at(this->getPath().size() - 1) != '/')
		this->setPath(this->getPath() + "/");
}

void Location::fillRoot(std::string line)
{
	size_t pos = line.find("root ");
	this->setRoot(line.substr(pos + strlen("root"), line.length() - (pos + strlen("root"))));
	if(this->getRoot().at(this->getRoot().size() - 1) != '/')
		this->setRoot(this->getRoot() + "/");
	if(this->getRoot().at(0) != '.')
		this->setRoot("." + this->getRoot());
}

void	Location::fillIndex(std::string line)
{
	size_t pos = line.find("index");
	this->setIndex(line.substr(pos + strlen("index"), line.length() - (pos + strlen("index"))));
}

void	Location::fillMaxBody(std::string line)
{
	size_t pos = line.find("client_max_body_size");
	this->setMaxBody(line.substr(pos + strlen("client_max_body_size"), line.length() - (pos + strlen("client_max_body_size"))));
}

void	Location::fillAutoIndex(std::string line)
{
	size_t pos = line.find("autoindex");
	this->setAutoIndex(line.substr(pos + strlen("autoindex"), line.length() - (pos + strlen("autoindex"))));
}

void	Location::fillRedir(std::string line)
{
	size_t pos = line.find("return ");
    if(pos == std::string::npos)
    {
        throw Response::ConfigurationFileLocation("'return' keyword not found");
    }
	std::string code = line.substr(pos + strlen("return "), 3);
	std::string domain = line.substr(pos + strlen("return ") + 4, line.length() - (pos + strlen("return")));
	this->setRedir(code, domain);
}

void	Location::fillErrorPage(std::string line)
{
	size_t pos = line.find("error_page");
	line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
	std::string code = line.substr(pos + strlen("error_page"), 3).c_str();
	std::string domain = line.substr(pos + strlen("error_page") + 3, line.length());
	this->setErrorPage(code, domain);
}

void	Location::fillCgiPath(std::string line)
{
	size_t pos = line.find("cgi_path");
	std::string language = line.substr(pos + strlen("cgi_path"), 3).c_str();
	std::string path = line.substr(pos + strlen("cgi_path") + 3, line.length());
	this->setCgiPath(language, path);

	//print
	// std::map<int, std::string>::iterator it = server->getErrorPage().begin();
	// std::cout << "the errorCode is:\t" YELLOW << it->first << RESET " (" YELLOW << it->second << RESET ")" << std::endl;
}

/* ================ */
/*		DEBUG		*/
/* ================ */

void Location::checkNotEmptys(void)
{
	if (this->getRoot().empty()
		&& this->getIndex().empty()
		&& this->getMaxBody().empty()
		&& this->getAutoIndex().empty()
		&& this->getRedir().empty()
		&& this->getErrorPage().empty())
		throw Response::ConfigurationFileLocation("Empty location");
}

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
	if(this->getCgiPath().size()) 
		std::cout << "\tcgi_path\t" YELLOW << this->getCgiPath().begin()->first << " " << this->getCgiPath().begin()->second << RESET << std::endl;
	if(this->getRedir().size()) 
		std::cout << "\treturn\t\t" YELLOW << this->getRedir().begin()->first << " " << this->getRedir().begin()->second << RESET << std::endl;
	std::cout << std::endl;
}
