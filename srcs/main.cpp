#include "httpConfig.hpp"
#include "serverConfig.hpp"
#include "locationConfig.hpp"
#include "serverAddr.hpp"

int main()
{
    httpConfig config;
    serverAddr listenAddress;

    try
    {
        config.parseConfig();
        config.printConfig();
        listenAddress.createListenAddr(config);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return (0);
}