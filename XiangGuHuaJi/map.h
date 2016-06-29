/* XiangGuHuaJi 2016, map.h
 *
 */

#ifndef _XIANGGUHUAJI_MAP_H__
#define _XIANGGUHUAJI_MAP_H__

#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include <cassert>

#include "definition.h"
#include "converter.hpp"

#include "json/json.h"
#pragma comment(lib, "lib_json.lib")

namespace XGHJ {

using namespace std;

class Map
{
public:
    Map();
    ~Map();

    TMapSize	rows, cols;

    cv::Mat/*TMatMapPara*/	    MapResource_, MapDefenseRatio_, MapAttackRatio_;
    vector<vector<TMapPara> >	MapResource, MapDefenseRatio, MapAttackRatio; //[x][y]

	bool                load(string file_name);
    inline cv::Size     size() { return MapResource_.size(); }

private:
    // convert from cv::Mat to vector<vector<T> > ; remember to call it before constructor
    void convertMyMat();
};

}

#endif
