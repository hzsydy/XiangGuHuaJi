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
#include"converter.hpp"

namespace XGHJ {

using namespace std;

class Map
{
public:
    //Map();
    ~Map();
    Map(string file_name);

    TMapSize	x, y;

    cv::Mat/*TMatMapPara*/	    MapResource_, MapDefenseRatio_, MapAttackRatio_;
    vector<vector<TMapPara> >	MapResource, MapDefenseRatio, MapAttackRatio;

private:
    //将cv::Mat转为vector<vector<T>>, 在加载完毕后使用
    void convertMyMat();

};

}

#endif