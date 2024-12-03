#include "Header.hpp"

std::string	CGIExtensions[] = {
	".py",
	".php",
	".pl",
	".sh",
	".rb",
	".tcl",
	".js"
};

int	nbCGIExtensions = 7;

std::string CGIExtension(std::string path)
{
	//return false if there is not extension
	int	i = 0;

	while (i < nbCGIExtensions)
	{
		size_t	pos = path.find(CGIExtensions[i]);
		std::string extension = &path[pos];
		if (pos != std::string::npos)
			return extension;
		i++;
	}
	return ("");
}

bool	ft_stoi(const std::string &str, int &result)
{
	bool isNegative = false;
	size_t i = 0;

	while (i < str.length() && (str[i] == ' ' || str[i] == '\t')) {
		i++;
	}
	if (i < str.length() && (str[i] == '-' || str[i] == '+')) {
		if (str[i] == '-') {
			isNegative = true;
		}
		i++;
	}
	while (i < str.length() && str[i] >= '0' && str[i] <= '9') {
		result = result * 10 + (str[i] - '0');
		i++;
	}
	if (i < str.length() && (str[i] < '0' || str[i] > '9')) {
		return false;
	}

	result = isNegative ? -result : result;
	return true;
}

std::vector<std::string>	ft_split(const std::string& str, char delimiter)
{
	std::vector<std::string> result;
	std::string tmp;

	for (std::string::size_type i = 0; i < str.size(); ++i) {
		if (str[i] == delimiter) {
			result.push_back(tmp);
			tmp.clear();
		} else {
			tmp += str[i];
		}
	}

	// Ajouter la dernière partie (si elle existe)
	if (!tmp.empty()) {
		result.push_back(tmp);
	}

	return result;
}

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
        return (header.size());
		// errorPage("400", data);
		// throw Response::ErrorBodyPostRequest();
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
        errorPage("413", data);
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

std::string readFile(std::string filePath, t_serverData *data)
{
    std::ifstream inputFile(filePath.c_str(), std::ios::binary);

    if (!inputFile.is_open())
    {
        errorPage("404", data);
    } // love

    std::stringstream buffer;
    buffer << inputFile.rdbuf(); //gets all content of the file and puts it into buffer;
    return (buffer.str());
}

// std::string readFile(std::string path, t_serverData *data)
// {
// 	std::ifstream file(path.c_str(), std::ios::binary);
// 	if(!file.is_open())
// 	{
//         errorPage("404", data);
// 	}
// 	return std::string(
// 		std::istreambuf_iterator<char>(file),
// 		std::istreambuf_iterator<char>()
// 	);
// }

std::string read_error_file(std::string path, t_serverData *data)
{
    //idem that readfile but return an error without looking for a file in case of impossible reading
	std::ifstream file(path.c_str(), std::ios::binary);
	if(!file.is_open())
	{
        Response::sendResponse("500", "text/html", "<h1>500 Internal Server Error</h1>", data);
	}
	return std::string(
		std::istreambuf_iterator<char>(file),
		std::istreambuf_iterator<char>()
	);
}

bool is_keep_alive(std::string &header)
{
    if(header.find("keep-alive") != std::string::npos)
    {
        return (true);
    }
    return (false);
}

bool request_allowed(std::string typeRequest, t_serverData *data)
{
    std::vector<std::string>::iterator it = std::find(data->requestAllow.begin(), data->requestAllow.end(), typeRequest);
    if(it != data->requestAllow.end() || !data->requestAllow.size())
        return(true);
    errorPage("403", data);
    return (false);
}

void truncate_file(std::string &file, t_serverData *data)
{
    // get the boundary 
    size_t pos = data->header.find("boundary=");
    std::string boundary = data->header.substr(pos + 9, data->header.size() - pos);
    pos = boundary.find("\r\n");
    boundary = boundary.substr(0, pos);
    std::cout << RED << "boundary: " << boundary << RESET  << std::endl;
    // erase the line
    pos = file.find("\n");
    file.erase(0, pos + 1);
    pos = file.find("\n");
    file.erase(0, pos + 1);
    pos = file.find("\n");
    file.erase(0, pos + 1);
    pos = file.find("\n");
    file.erase(0, pos + 1);
    pos = file.find(boundary + "--");
    if(pos != std::string::npos)
    {
        file.erase(pos - 1, (boundary + "--").size());
    }
}
