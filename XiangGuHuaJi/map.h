/* XiangGuHuaJi 2016, map.h
 *
 */

#ifndef _XIANGGUHUAJI_MAP_H__
#define _XIANGGUHUAJI_MAP_H__

#include<iostream>
#include<fstream>
#include<string>
#include<opencv2/opencv.hpp>

namespace XGHJ {

using namespace std;

class Map
{
public:
    // 从文件加载Map
    Map(string file_name);

    ~Map();
private:
    int x, y;
    cv::Mat Resource, Defense, Attack;
    
};

}

#endif