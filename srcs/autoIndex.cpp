#include "Header.hpp"

bool isDirectory(const std::string& path)
{
	struct stat statbuf;
    std::string full = "./www/" + path;
	if (stat(full.c_str(), &statbuf) != 0)
    {
        std::cout << "return false " << path << std::endl;
        return false;
    }
    std::cout << "true " << path << " and full " << full << std::endl;
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

std::string generateAutoIndexPage(const std::string directory, const std::vector<std::string>& files)
{
	std::string html;
	// std::cout << "the url : " << directory << std::endl;
	html = "\n<html>\n<head>\n\t<title>Index of " + directory + "</title>\n</head>\n<body>\n";
	html += "<h1>Index of " + directory + "</h1><ul>\n";

	// std::cout << MAGENTA "directory: " << directory << RESET << std::endl;

	for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it)
    {
        std::string relativePath = directory + *it;

        if(relativePath.at(0) != '/')
            relativePath.insert(0, 1, '/');
        html += "\t<li><a href=\"" + relativePath + "\">" + *it + (isDirectory(relativePath) ? "/" : "") + "</a></li>\n";
		// std::cout << MAGENTA "it: " << *it << RESET << std::endl;
	}
	html += "</ul></body></html>";
	return html;
}
