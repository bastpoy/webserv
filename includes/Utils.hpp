#ifndef UTILS_CPP
#define UTILS_CPP

#include "Header.hpp"

bool	ft_stoi(const std::string &str, int &result);
std::vector<std::string> ft_split(const std::string& str, char delimiter);
std::string readFile(std::string path);
bool isExtension(std::string path);

template <typename T>
std::string to_string(T value) {
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

#endif