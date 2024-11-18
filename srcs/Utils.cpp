#include "Header.hpp"

int	ft_stoi(std::string str)
{
	int			i;
	long long	atoi;
	int			sign;

	i = 0;
	atoi = 0;
	sign = 1;
	while (str[i] == 32 || (str[i] >= 9 && str[i] <= 13))
		i++;
	if (str[i] == '+' || str[i] == '-')
	{
		if (str[i] == '-')
			sign *= -1;
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
		atoi = atoi * 10 + (str[i++] - '0');
	return ((int)atoi * sign);
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
		errorPage("404", data);
	}
	return std::string(
		std::istreambuf_iterator<char>(file),
		std::istreambuf_iterator<char>()
	);
}
