#include "Header.hpp"

bool isDirectory(const std::string& path)
{
	struct stat statbuf;
	if (stat(path.c_str(), &statbuf) != 0)
		return false;
	return S_ISDIR(statbuf.st_mode);
}

std::vector<std::string> listDirectory(const std::string& directory)
{
	std::vector<std::string> files;
	DIR* dir = opendir(directory.c_str());
	if (dir != NULL)
	{
		struct dirent* entry;
		while ((entry = readdir(dir)) != NULL)
			files.push_back(entry->d_name);
		closedir(dir);
	}
	return files;
}

std::string generateAutoIndexPage(const std::string& directory, const std::vector<std::string>& files)
{
	std::string html;
	
	
	html = "\n<html>\n<head>\n\t<title>Index of " + directory + "</title>\n</head>\n<body>\n";
	html += "<h1>Index of " + directory + "</h1><ul>\n";

	std::cout << MAGENTA "directory: " << directory << RESET << std::endl;

	for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it)
	{
		if (*it == ".")
			html += "\t<li><a href=\"\">./</a></li>\n";
		else if (*it == "..")
			html += "\t<li><a href=\"../\">../</a></li>\n";
		else
			html += "\t<li><a href=\"" + directory + *it + "\">" + *it + (isDirectory(directory + *it) ? "/" : "") + "</a></li>\n";
		std::cout << MAGENTA "it: " << *it << RESET << std::endl;
	}

	html += "</ul></body></html>";
	return html;
}

std::string handleAutoIndex(const std::string& path)
{
	if (isDirectory(path))
	{
		std::vector<std::string> files = listDirectory(path);
		return generateAutoIndexPage(path, files);  // Contenu de la page HTML de l'autoindex
	}
	// Sinon, gérer la demande normalement (serveur de fichier, etc.)
	return "";
}
