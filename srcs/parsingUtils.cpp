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

// void	maxBodyParsing(std::string caracter, std::string &size)
// {
// 	size_t pos;
	
// 	if(size.find(caracter) != std::string::npos)
// 	{
// 		pos = size.find(caracter);
// 		//compare the position of the actual caracter
// 		if(size.size() - 1 != pos)
// 			throw Response::ErrorMaxBody();
// 		if(caracter == "k" || caracter == "K")
// 			size.replace(pos, 1, "000");
// 		else
// 			size.replace(pos, 1, "000000");
// 	}
// }