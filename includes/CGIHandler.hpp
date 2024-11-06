#ifndef CGI_HANDLER_HPP
# define CGI_HANDLER_HPP

#include "Header.hpp"

class CGIHandler
{
	public:
		static void	execute(std::string uri, int connection);
};

#endif