#pragma comment(lib, "lib_json.lib")

#include "MapParser.h"

namespace XGHJ
{
	int MapParser::readjson(std::string jsonname)
	{
		std::ifstream ifs;
		ifs.open(jsonname);
		if (!ifs.is_open())
		{
			std::cout<<"cannot open file"<<std::endl;
			return -2;
		}

		Json::Reader reader;
		Json::Value root;
		if (!reader.parse(ifs, root, false))
		{
			std::cout<<"cannot parse json file"<<std::endl;
			return -1;
		}

		std::string name = root["name"].asString();
		int age = root["age"].asInt();

		std::cout<<name<<std::endl;
		std::cout<<age<<std::endl;

		return 0;
	}

}



