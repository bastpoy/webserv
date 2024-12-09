#ifndef UTILS_CPP
#define UTILS_CPP

#include "Header.hpp"

bool						ft_stoi(const std::string &str, int &result);
std::vector<std::string>	ft_split(const std::string& str, char delimiter);
std::string					readFile(std::string path, t_serverData *data);
std::string					read_error_file(std::string path, t_serverData *data);
bool						isExtension(std::string path);
bool						isExtensionDownload(std::string path);
void						truncate_file(std::string &file, t_serverData *data);
bool						is_keep_alive(std::string &header);
bool						request_allowed(std::string typeRequest, t_serverData *data);
std::string					CGIExtension(std::string path);
bool						is_cgi_extension(std::string path);

template <typename T>
std::string to_string(T value) {
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

#endif