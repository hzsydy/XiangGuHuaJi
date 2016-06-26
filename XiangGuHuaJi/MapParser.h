#ifndef __XIANGGUHUAJI_MAPPARSER_H_
#define __XIANGGUHUAJI_MAPPARSER_H_

#include <fstream>
#include <iostream>
#include <cassert>
#include "json/json.h"

namespace XGHJ
{
	class MapParser
	{
	public:
		MapParser(){}
		int readjson(std::string jsonname);
	protected:

	};

}

#endif