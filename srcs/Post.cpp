#include "Header.hpp"

void replaceSpecialCharacter(std::string &value)
{
    if(value.find("%40") != std::string::npos)
    {
        size_t pos = value.find("%40");
        value.replace(pos, 3, "@");
    }
}

void insertValue(std::string temp, std::map<std::string, std::string> &values)
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
        throw Response::ErrorBodyPostRequest();
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
    putDb(values);
}

void translateJson()
{
    std::string line;
    int once = 0;
    //open keyvalue file where all the data is store;
    std::ifstream inFile("./www/keyvalue.txt");
    if(!inFile.is_open())
    {
        std::cout << "error opening the file for data " << strerror(errno) << std::endl;
        throw Response::Error();
    }
    // open the json file where I will translate the value
    int jsonFile = open("./www/keyvalue.json", O_WRONLY | O_CREAT, 0644);
    if(jsonFile < 0)
    {
        inFile.close();
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

// get the content for uploading files

int getContentLength(std::string header)
{
    //function to retrieve the content-length
    std::string content = "Content-Length: ";
    size_t pos = header.find(content);

    if(pos == std::string::npos)
    {
        throw Response::ErrorBodyPostRequest();
    }
    std::string size = header.substr(pos + content.size(), header.size());
    pos = size.find("\n");
    size = size.substr(0, pos);
    return(atoi(size.c_str()));
}

std::string getFileName(std::string body)
{
    //function to retrieve the filename to upload
    std::string file = "filename=\"";
    size_t pos = body.find(file);
    if(pos == std::string::npos)
    {
        throw Response::ErrorBodyPostRequest();
    }
    std::string fileName = body.substr(pos + file.size(), body.size());
    pos = fileName.find("\"");
    fileName = fileName.substr(0, pos);
    std::cout << fileName << std::endl;
    return (fileName);
}
