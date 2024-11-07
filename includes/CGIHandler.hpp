#ifndef CGI_HANDLER_HPP
# define CGI_HANDLER_HPP

#include "Header.hpp"

class CGIHandler
{
	public:
		static std::string	execute(std::string uri, std::string &code);
};

#endif