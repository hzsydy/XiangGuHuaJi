/* XiangGuHuaJi 2016, map.h
 *
 */

#ifndef _XIANGGUHUAJI_MAP_H__
#define _XIANGGUHUAJI_MAP_H__

#include<iostream>
#include<fstream>
#include<string>
#include<opencv2/opencv.hpp>

#include"definition.h"
#include"matrix.hpp"

namespace XGHJ {

using namespace std;

class Map
{
public:
    Map();
    Map(int x, int y);
    // 从文件加载Map
    Map(string file_name);

    inline int getX() { return x; }
    inline int getY() { return y; }
    inline cv::Size size() { return MapResource.size(); }
    inline cv::Mat getMapResource()     { return MapResource; }
    inline cv::Mat getMapDefenseRatio() { return MapDefenseRatio; }
    inline cv::Mat getMapAttackRatio()  { return MapAttackRatio; } 
    inline TMapPara** getMMapResource()     { return MMapResource; }
    inline TMapPara** getMMapDefenseRatio() { return MMapDefenseRatio; }
    inline TMapPara** getMMapAttackRatio()  { return MMapAttackRatio; }

    ~Map();
private:
    int x, y;
    cv::Mat MapResource, MapDefenseRatio, MapAttackRatio;
    
    TMapPara ** MMapResource, ** MMapDefenseRatio, ** MMapAttackRatio;
    

};

}

#endif