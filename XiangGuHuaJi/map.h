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

namespace XGHJ {

class Map : public BaseMap
{
public:
    bool easy_load(string file_name);
};

}

#endif
