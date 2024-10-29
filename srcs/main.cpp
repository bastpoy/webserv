#include "httpConfig.hpp"
#include "serverConfig.hpp"
#include "locationConfig.hpp"
#include "serverAddr.hpp"

int main()
{
    try
    {
        httpConfig *config = new httpConfig;
        serverAddr listenAddress;

        config->parseConfig();
        config->printConfig();
        std::cout << "test" << std::endl;
        listenAddress.createListenAddr(config);
    }
    catch(const std::exception& e)
    {
        std::cout << "test1" << std::endl;
        std::cerr << e.what() << '\n';
    }
    std::cout << "re" << std::endl;
    return (0);
}