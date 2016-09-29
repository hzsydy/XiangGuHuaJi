/* XiangGuHuaJi 2016, map.h
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

using namespace std;

class Map
{
public:
    Map(){;}
    ~Map(){;}
	bool                load(string file_name);
    
	inline TMap getRows(){return rows;}
	inline TMap getCols(){return cols;}
	inline vector<vector<TMoney> > getMapRes(){return MapResource_;}
	inline vector<vector<TMilitary> > getMapAtk(){return MapAttackRatio_;}
	inline vector<vector<TMilitary> > getMapDef(){return MapDefenseRatio_;}
private:
	TMap	rows, cols;
	vector<vector<TMoney> >  MapResource_;
	vector<vector<TMilitary> > MapDefenseRatio_, MapAttackRatio_;
};

}

#endif
