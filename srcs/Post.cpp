#include "Header.hpp"

void insertValue(std::string temp, std::map<std::string, std::string> &values)
{
	size_t pos2 = temp.find_first_of('=');

	if(pos2 != std::string::npos)
	{
		// take the content before and after '='
		std::pair<std::string, std::string> pair(temp.substr(0, pos2 - 1), temp.substr(pos2 + 1, temp.size() - 1));
		values.insert(pair);
	}
	else
		throw Response::ErrorBodyPostRequest();
}

void parsePostBody(std::string &body)
{
	std::map<std::string, std::string> values;
	std::string temp;
	std::string key;
	std::string value;
	size_t pos1 =  body.find_first_of('&');

	while(pos1 != std::string::npos)
	{
		temp = body.substr(0, pos1);

		//insert key pair values in values
		insertValue(temp, values);
		//update body
		body = body.substr(pos1 + 1);
		//search another key value pair
		pos1 = body.find_first_of('&');
	}
	insertValue(body, values);
}