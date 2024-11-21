#include "Header.hpp"


int	ft_stoi(const std::string &str)
{
	int result = 0;
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
		throw std::invalid_argument("Invalid argument: contains non-digit characters");
	}

	return isNegative ? -result : result;
}

std::vector<std::string> ft_split(const std::string& str, char delimiter)
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

bool isExtension(std::string path)
{
	//return false if there is not extension
	if(path.find(".html") == std::string::npos &&
		path.find(".css") == std::string::npos &&
		path.find(".js") == std::string::npos &&
		path.find(".png") == std::string::npos &&
		path.find(".jpg") == std::string::npos &&
		path.find(".gif") == std::string::npos &&
		path.find(".ico") == std::string::npos &&
		path.find(".py") == std::string::npos)
		return (false);
	return (true);
}

std::string readFile(std::string path)
{
	std::ifstream file(path.c_str(), std::ios::binary);
	if(!file.is_open())
	{
		// std::cout << path << ": ";
		throw Response::ErrorOpeningFile(path);
	}
	return std::string(
		std::istreambuf_iterator<char>(file),
		std::istreambuf_iterator<char>()
	);
}