#ifndef CGI_HANDLER_HPP
# define CGI_HANDLER_HPP

#include "Header.hpp"

class CGIHandler
{
	public:
		static std::string	execute(const std::string& script_path, const std::map<std::string, std::string>& env);
};

#endif