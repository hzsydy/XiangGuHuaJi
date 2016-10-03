/* XiangGuHuaJi 2016, basemap.h
 *
 */

#ifndef _XIANGGUHUAJI_MAP_H__
#define _XIANGGUHUAJI_MAP_H__

#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

#include "definition.h"

#include "json/json.h"
#pragma comment(lib, "lib_json.lib")

namespace XGHJ {

class Map : public BaseMap
{
public:
	Map(){;}
	bool load(string file_name);
};

}

#endif
