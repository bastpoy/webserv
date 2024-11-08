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
	std::string html = "<html><head><title>Index of " + directory + "</title></head><body>";
	html += "<h1>Index of " + directory + "</h1><ul>";

	std::string dirName = directory;
	size_t pos = directory.find("./www");
	if (pos != std::string::npos) {
		dirName = directory.substr(pos + 5);
	}
	std::cout << MAGENTA "dirName" << dirName << RESET << std::endl;
	for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it)
	{
		// Ignore les répertoires "." et ".."
		if (*it != "." && *it != "..")
		{
			html += "<li><a href=\"" + directory + *it + "\">" + *it + "</a></li>";
		}
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
