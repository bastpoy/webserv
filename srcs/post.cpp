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

void putDb(std::map<std::string, std::string> values)
{
	std::map<std::string, std::string>::iterator it = values.begin();

	int fd = open("./www/keyvalue.txt", O_WRONLY | O_APPEND | O_CREAT, 0644);
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

void parsePostBody(std::string &body, t_serverData *data)
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
	putDb(values);
}

void translateJson(t_serverData *data)
{
	std::string line;
	int once = 0;
	//open keyvalue file where all the data is store;
	std::ifstream inFile("./www/keyvalue.txt");
	if(!inFile.is_open())
	{
		errorPage("400", data);
		std::cout << "error opening the file for data " << strerror(errno) << std::endl;
	}
	// open the json file where I will translate the value
	int jsonFile = open("./www/keyvalue.json", O_WRONLY | O_CREAT, 0644);
	if(jsonFile < 0)
	{
		inFile.close();
		errorPage("400", data);
		std::cout << strerror(errno) << std::endl;
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
			//test if its the last bloc
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

void sendPostData(std::string code , std::string contentType, std::string content, t_serverData *data)
{
	//build the http header response
	std::string response = "HTTP/1.1 " + code + " \r\n"
							"Content-Type: " + contentType + "\r\n"
							"Content-Length: " + to_string(content.size()) + "\r\n"
							"\r\n" + content;
	//send response
	if(send(data->sockfd, response.c_str(), response.size(), 0) < 0)
	{
		std::cout << strerror(errno) << std::endl;
		throw Response::ErrorSendingResponse(); 
	}
}

int getContentLength(std::string header, t_serverData *data)
{
	//function to retrieve the content-length
	std::string content = "Content-Length: ";
	size_t pos = header.find(content);

	if(pos == std::string::npos)
	{
		errorPage("400", data);
		throw Response::ErrorBodyPostRequest();
	}
	//get the maxbody
	std::string size = header.substr(pos + content.size(), header.size());
	pos = size.find("\n");
	size = size.substr(0, pos);

	int max_body = atoi(data->maxBody.c_str());
	int intSize = atoi(size.c_str());
	//if the request size is superior to the max_body return an error
	if(intSize > max_body)
	{
		contentTooLarge(data->maxBody, data);
		throw Response::Error();
	}
	return(intSize);
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

bool read_full_body(t_serverData *data, std::string &body, int content_length) {
	// read the full body of the upload content
	int total_read = body.size();
	const int buffer_size = 1024;   // Use a large buffer if desired
	char buffer[buffer_size];

	while (total_read < content_length) {
		// Calculate how much more data we need to read
		int bytes_to_read = std::min(content_length - total_read, buffer_size);
		
		// Read data into the buffer
		int bytes_read = recv(data->sockfd, buffer, bytes_to_read, 0);
		
		if (bytes_read < 0) 
		{
			std::cout << "Error reading from socket: " << strerror(errno) << std::endl;
			errorPage("400", data);
		} 
		else if (bytes_read == 0) 
		{
			std::cerr << "Connection closed by the client." << std::endl;
			break; // Connection closed
		}

		// Append the read data to the body string
		body.append(buffer, bytes_read);
		total_read += bytes_read;
	}

	// Check if we read the expected content length
	return total_read == content_length;
}

void postRequest(std::string buffer, t_serverData *data)
{
	std::cout << "\nPOST REQUEST\n" << std::endl;
	//search the body in the header
	size_t pos = buffer.find("\r\n\r\n");
	// If i have a body in the post request
	if(pos != std::string::npos)
	{
		// retrieve the body and remove the \r\n\r\n before by adding + 4
		std::string body = buffer.substr(pos + 4, buffer.size());
		std::string header = buffer.substr(0, pos);
		// If I have an upload
		if(header.find("multipart/form-data") != std::string::npos)
		{
			std::string file = "./www/pages/post/post.html";
			std::cout << "UPLOADING FILE" << std::endl;
			//get the size of the file to upload
			int size = getContentLength(header, data);
			std::string fileName = getFileName(body, data);
			fileName = "./www/upload/" + fileName;
			//read all the data of the upload file
			read_full_body(data, body, size);
			std::ofstream output(fileName.c_str());
			if(!output.is_open())
			{
				errorPage("500", data);
				throw Response::ErrorOpeningFile();
			}
			//put the download data inside a file
			output << body;
			output.close();
			sendPostData("201 Created", "text/html", readFile(file, data), data);
		}
		// If i have a form
		else
		{
			std::string file = "./www/keyvalue.json";
			std::cout << "POSTING DATA" << std::endl;
			//parse the actual body of the post request
			parsePostBody(body, data);
			//put all the data from the body inside a file
			translateJson(data);
			//send response POST
			sendPostData("201 Created", "application/json", readFile(file, data), data);
		}
	}
	// error post body
	else
        errorPage("500" , data);
}