#include "httpConfig.hpp"
#include "serverConfig.hpp"
#include "locationConfig.hpp"

int main()
{
    httpConfig config;

    try
    {
        config.parseConfig();
        config.printConfig();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return (0);
}