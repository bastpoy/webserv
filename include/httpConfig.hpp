#ifndef HTTPCONFIG_HPP
#define HTTPCONFIG_HPP

#include "serverConfig.hpp"
#include "resHeader.hpp"
#include "locationConfig.hpp"

class serverConfig;
class httpConfig
{
	public:

		//basic
		httpConfig();
		~httpConfig();

		//getter
		std::vector<serverConfig> &getServer();
		
		//setter
		void addServer(serverConfig &server);
		
		//parsing
		void parseConfig();
		void getServerAttributs(std::ifstream& file, serverConfig &server);

		//other
		void printConfig();

	private:
		std::vector<serverConfig> server;
};

#endif