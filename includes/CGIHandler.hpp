#ifndef CGI_HANDLER_HPP
# define CGI_HANDLER_HPP

#include "Header.hpp"

class CGIHandler
{
	public:
		static void	execute(const std::string script_path, int connection);
};

#endif