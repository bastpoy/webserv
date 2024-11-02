#include "Header.hpp"

std::string readFile(std::string &path)
{
    std::ifstream file(path.c_str(), std::ios::binary);
    if(!file.is_open())
    {
        std::cout << path << ": ";
        throw Response::ErrorOpeningFile();
    }
    return std::string(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );
}