#ifndef UTILS_CPP
#define UTILS_CPP

#include "Header.hpp"

std::string readFile(std::string path, t_serverData *data);
std::string read_error_file(std::string path, t_serverData *data);
bool isExtension(std::string path);
bool isExtensionDownload(std::string path);
void truncate_file(std::string &file);
bool is_keep_alive(std::string &header);

template <typename T>
std::string to_string(T value) {
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

#endif