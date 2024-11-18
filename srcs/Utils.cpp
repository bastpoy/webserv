#include "Header.hpp"

std::string getContentType(std::string &path) 
{
	std::map<std::string, std::string> contentTypes;

	contentTypes.insert(std::pair<std::string, std::string>(".html", "text/html"));
	contentTypes.insert(std::pair<std::string, std::string>(".css", "text/css"));
	contentTypes.insert(std::pair<std::string, std::string>(".js", "application/javascript"));
	contentTypes.insert(std::pair<std::string, std::string>(".png", "image/png"));
	contentTypes.insert(std::pair<std::string, std::string>(".jpg", "image/jpeg"));
	contentTypes.insert(std::pair<std::string, std::string>(".gif", "image/gif"));
	contentTypes.insert(std::pair<std::string, std::string>(".svg", "image/svg+xml"));
	contentTypes.insert(std::pair<std::string, std::string>(".webp", "image/webp"));
	contentTypes.insert(std::pair<std::string, std::string>(".ico", "image/x-icon"));
    contentTypes.insert(std::pair<std::string, std::string>(".pdf", "application/pdf"));
    contentTypes.insert(std::pair<std::string, std::string>(".mp3", "video/mpeg"));
    contentTypes.insert(std::pair<std::string, std::string>(".mp4", "video/mp4"));
    contentTypes.insert(std::pair<std::string, std::string>(".webm", "video/webm"));
    contentTypes.insert(std::pair<std::string, std::string>(".ogg", "video/ogg"));
    contentTypes.insert(std::pair<std::string, std::string>(".doc", "application/msword"));
    contentTypes.insert(std::pair<std::string, std::string>(".docx", "application/msword"));
    contentTypes.insert(std::pair<std::string, std::string>(".xls", "application/vnd.ms-excel"));
    contentTypes.insert(std::pair<std::string, std::string>(".xlsx", "application/vnd.ms-excel"));
    contentTypes.insert(std::pair<std::string, std::string>(".ppt", "application/vnd.ms-powerpoint"));
    contentTypes.insert(std::pair<std::string, std::string>(".pptx", "application/vnd.ms-powerpoint"));
    contentTypes.insert(std::pair<std::string, std::string>(".ppt", "application/vnd.ms-powerpoint"));
    contentTypes.insert(std::pair<std::string, std::string>(".zip", "application/zip"));
    contentTypes.insert(std::pair<std::string, std::string>(".rar", "application/vnd.rar"));
    contentTypes.insert(std::pair<std::string, std::string>(".tar", "application/x-tar"));
    contentTypes.insert(std::pair<std::string, std::string>(".gz", "application/gzip"));
    contentTypes.insert(std::pair<std::string, std::string>(".7z", "application/x-7z-compressed"));
    contentTypes.insert(std::pair<std::string, std::string>(".txt", "text/plain"));
    contentTypes.insert(std::pair<std::string, std::string>(".xml", "application/xml"));
    contentTypes.insert(std::pair<std::string, std::string>(".json", "application/json"));
    contentTypes.insert(std::pair<std::string, std::string>(".csv", "text/csv"));
    contentTypes.insert(std::pair<std::string, std::string>(".py", "text/html"));

	size_t dotPos = path.find_last_of(".");
	if (dotPos != std::string::npos) {
		std::string extension = path.substr(dotPos);
		if (contentTypes.find(extension) != contentTypes.end()) {
			return contentTypes[extension];
		}
	}
	//if i dont have exetension i add backslash
	else
	{
		//if size of path = 0 or no '/' at the end
		if(path.size() == 0 || path.at(path.size() - 1) != '/')
			path += "/";
	}
	return "text/html"; // Default content type
}

int getContentLength(std::string header, t_serverData *data)
{
	//function to retrieve the content-length
	std::string content = "Content-Length: ";
	size_t pos = header.find(content);

	if(pos == std::string::npos)
	{
		errorPage("400", data);
		throw Response::ErrorBodyPostRequest();
	}
	//get the maxbody
	std::string size = header.substr(pos + content.size(), header.size());
	pos = size.find("\n");
	size = size.substr(0, pos);

	int max_body = atoi(data->maxBody.c_str());
	int intSize = atoi(size.c_str());
	//if the request size is superior to the max_body return an error
	if(intSize > max_body)
	{
		contentTooLarge(data->maxBody, data);
		throw Response::Error();
	}
	return(intSize);
}

bool isExtension(std::string path)
{
	//return false if there is not extension
	if(path.find(".html") == std::string::npos &&
		path.find(".css") == std::string::npos &&
		path.find(".js") == std::string::npos &&
		path.find(".png") == std::string::npos &&
		path.find(".jpg") == std::string::npos &&
		path.find(".gif") == std::string::npos &&
        path.find(".svg") == std::string::npos &&
        path.find(".webp") == std::string::npos &&
		path.find(".ico") == std::string::npos &&
		path.find(".py") == std::string::npos &&
        path.find(".pdf") == std::string::npos &&
        path.find(".mp3") == std::string::npos &&
        path.find(".mp4") == std::string::npos &&
        path.find(".webm") == std::string::npos &&
        path.find(".ogg") == std::string::npos)
		return (false);
	return (true);
}

bool isExtensionDownload(std::string path)
{
	//return false if there is not extension
	if(path.find(".doc") == std::string::npos &&
		path.find(".docx") == std::string::npos &&
		path.find(".xls") == std::string::npos &&
		path.find(".xlsx") == std::string::npos &&
		path.find(".ppt") == std::string::npos &&
		path.find(".pptx") == std::string::npos &&
        path.find(".zip") == std::string::npos &&
        path.find(".rar") == std::string::npos &&
		path.find(".tar") == std::string::npos &&
		path.find(".gz") == std::string::npos &&
        path.find(".7z") == std::string::npos &&
        path.find(".txt") == std::string::npos &&
        path.find(".xml") == std::string::npos &&
        path.find(".json") == std::string::npos &&
        path.find(".csv") == std::string::npos)
		return (false);
	return (true);
}

std::string readFile(std::string path, t_serverData *data)
{
	std::ifstream file(path.c_str(), std::ios::binary);
	if(!file.is_open())
	{
		std::cout << path << ": ";
        notFound(data);
	}
	return std::string(
		std::istreambuf_iterator<char>(file),
		std::istreambuf_iterator<char>()
	);
}
