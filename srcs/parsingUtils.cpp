#include <Header.hpp>

void	checkLocationPath(Location &location, std::vector<Location> &locations)
{
	//check if path is empty
	// if (location.getPath().empty())
	// 	throw Response::ConfigurationFileLocationPath();
	for (std::vector<Location>::iterator it = locations.begin(); it != locations.end(); it++)
	{
		if (it->getPath() == location.getPath())
			throw Response::ConfigurationFileLocationPath();
	}
}
