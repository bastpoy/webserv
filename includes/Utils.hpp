#ifndef UTILS_CPP
#define UTILS_CPP

#include "Header.hpp"

std::string readFile(std::string path, t_serverData *data);
bool isExtension(std::string path);

template <typename T>
std::string to_string(T value) {
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

#endif