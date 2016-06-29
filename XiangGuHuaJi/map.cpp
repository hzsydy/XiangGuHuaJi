/* XiangGuHuaJi 2016, map.cpp
 *
 */

#include"map.h"

namespace XGHJ
{

//Map::Map()
//{
//}

// 从文件加载Map
Map::Map(string file_name)
{
    MapResource = cv::Mat(x, y, TMatMapPara);
    MapAttackRatio  = cv::Mat(x, y, TMatMapPara);
    MapDefenseRatio = cv::Mat(x, y, TMatMapPara);

    //TODO: load from file
    //

    convertMyMat();
}

Map::~Map()
{
}

void Map::convertMyMat()
{
    convertMat<TMapPara>(MapResource_, MapResource);
    convertMat<TMapPara>(MapDefenseRatio_, MapDefenseRatio);
    convertMat<TMapPara>(MapAttackRatio_, MapAttackRatio);
}

}