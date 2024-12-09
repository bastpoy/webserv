#include "Header.hpp"

void replaceSpecialCharacter(std::string &value)
{
	if(value.find("%40") != std::string::npos)
	{
		size_t pos = value.find("%40");
		value.replace(pos, 3, "@");
	}
	while(value.find("+") != std::string::npos)
	{
		size_t pos = value.find("+");
		value.replace(pos, 1, " ");
	}
}

void insertValue(std::string temp, std::map<std::string, std::string> &values, t_serverData *data)
{
	size_t pos2 = temp.find_first_of('=');

	if(pos2 != std::string::npos)
	{
		// take the content before and after '='
		std::string key = temp.substr(0, pos2);
		std::string value = temp.substr(pos2 + 1, temp.size() - 1);
		replaceSpecialCharacter(value);
		std::pair<std::string, std::string> pair(key, value);
		values.insert(pair);
	}
	else
	{
		errorPage("400", data);
		throw Response::ErrorBodyPostRequest();
	}
}

void putFormData(std::map<std::string, std::string> values)
{
	std::map<std::string, std::string>::iterator it = values.begin();

	int fd = open("./www/data/form/keyvalue.txt", O_WRONLY | O_APPEND | O_CREAT, 0644);
	if(fd < 0)
	{
		std::cout << "Error during opening file:" << strerror(errno) << std::endl;
		throw Response::Error();
	}
	while(it != values.end())
	{
		write(fd, "\"", 1);
		write(fd, it->first.c_str(), it->first.size());
		write(fd, "\" : \"", 5);
		write(fd, it->second.c_str(), it->second.size());
		it++;
		if(it != values.end())
			write(fd, "\",\n", 3);
		else
			write(fd, "\"\n", 2);
	}
	close(fd);
}

void parsePostBody(std::string &body, t_serverData *data, Cookie &cookie)
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
		insertValue(temp, values, data);
		//update body
		body = body.substr(pos1 + 1);
		//search another key value pair
		pos1 = body.find_first_of('&');
	}
	insertValue(body, values, data);
    
    if(values.find("password") != values.end())
    {
        std::string file = "./www/pages/post/post.html";
        std::string id = newSessionCookie(values, cookie, data);
        httpPostResponse("201 Created", "text/html", readFile(file, data), data, cookie, id);
        Response::responseOk();
    }
    else
        putFormData(values);
}

void translateJson(t_serverData *data)
{
	std::string line;
	int once = 0;
	//open keyvalue file where all the data is store;
	std::ifstream inFile("./www/data/form/keyvalue.txt");
	if(!inFile.is_open())
	{
		errorPage("400", data);
		std::cout << "error opening the file for data " << strerror(errno) << std::endl;
	}
	// open the json file where I will translate the value
	int jsonFile = open("./www/data/form/keyvalue.json", O_WRONLY | O_CREAT, 0644);
	if(jsonFile < 0)
	{
		inFile.close();
		errorPage("400", data);
		throw Response::Error();
	}
	//parse my keyvalue file into keyvalue.json file
	while(std::getline(inFile, line))
	{
		if(!once)
		{
			write(jsonFile, "{\n\t\"entries\": [\n", 16);
			once = 1;
		}
		if(line.find("\"email\"") != std::string::npos)
		{
			write(jsonFile, "\t{\n\t\t", 5);
			write(jsonFile, line.c_str(), line.size());
			write(jsonFile, "\n\t\t", 3);
			std::getline(inFile, line);
		}
		if(line.find("\"message\"") != std::string::npos)
		{
			write(jsonFile, line.c_str(), line.size());
			write(jsonFile, "\n\t\t", 3);
			std::getline(inFile, line);
		}
		if(line.find("\"name\"") != std::string::npos)
		{
			write(jsonFile, line.c_str(), line.size());
			write(jsonFile, "\n\t}", 3);
			//test if its the last bloc1
			if(std::getline(inFile, line))
			{
				write(jsonFile, ",\n", 2);
				write(jsonFile, "\t{\n\t\t", 5);
				write(jsonFile, line.c_str(), line.size());
				write(jsonFile, "\n\t\t", 3);
			}
			//end of file
			else
			{
				write(jsonFile, "\n\t]\n}", 5);
				break;
			}
		}
	}
	close(jsonFile);
	inFile.close();
}

std::string getFileName(std::string body, t_serverData *data)
{
	//function to retrieve the filename to upload
	std::string file = "filename=\"";
	size_t pos = body.find(file);
	if(pos == std::string::npos)
	{
		errorPage("400", data);
		throw Response::ErrorBodyPostRequest();
	}
	std::string fileName = body.substr(pos + file.size(), body.size());
	pos = fileName.find("\"");
	fileName = fileName.substr(0, pos);
	return (fileName);
}


void postRequest(t_serverData *data, Cookie &cookie)
{
	std::cout << "\nPOST REQUEST\n" << std::endl;
	//search the body in the header
	size_t pos = data->buffer.find("\r\n\r\n");
	// If i have a body in the post request
	if(pos != std::string::npos)
	{
		// If I have an upload
		if(data->header.find("multipart/form-data") != std::string::npos)
		{
			std::string file = "./www/pages/post/post.html";
			std::cout << "UPLOADING FILE" << std::endl;
			//get the size of the file to upload
			// int size = getContentLength(data->header, data);
			std::string fileName = getFileName(data->body, data);
			fileName = "./www/upload/" + fileName;
			//read all the data of the upload file
			std::ofstream output(fileName.c_str(), std::ios::binary);
			if(!output.is_open())
			{
				errorPage("500", data);
				throw Response::ErrorOpeningFile(NULL);
			}
            //truncate the beginning and the end of my file and remove boundaries
            truncate_file(data->body, data);
			//put the download data inside a file
            output.write(data->body.c_str(), data->body.size());
            
            // std::cout << body << std::endl;
            std::string id = get_cookie_id(data->buffer);
			httpPostResponse("201 Created", "text/html", readFile(file, data), data, cookie, id);
		}
		// If i have a form
		else
		{
			std::string file = "./www/data/form/keyvalue.json";
			std::cout << "POSTING DATA" << std::endl;
			//parse the actual body of the post request
			parsePostBody(data->body, data, cookie);
			//put all the data from the body inside a file
			translateJson(data);
			//send responseiPOST
            std::string id = get_cookie_id(data->buffer);
			httpPostResponse("201 Created", "application/json", readFile(file, data), data, cookie, id);
		}
	}
	// error post body
	else
		errorPage("500" , data);
}
