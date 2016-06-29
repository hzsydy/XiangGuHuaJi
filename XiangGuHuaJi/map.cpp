/* XiangGuHuaJi 2016, map.cpp
 *
 */

#include"map.h"

namespace XGHJ
{

Map::Map()
{
}

Map::Map(int x, int y)
{
    MapResource = cv::Mat(x, y, TMatMapPara);
    MapAttackRatio  = cv::Mat(x, y, TMatMapPara);
    MapDefenseRatio = cv::Mat(x, y, TMatMapPara);

    //MMapResource     = newCopyMatrix<TMapPara>(MapResource);
    //MMapDefenseRatio = newCopyMatrix<TMapPara>(MapAttackRatio);
    //MMapAttackRatio  = newCopyMatrix<TMapPara>(MapDefenseRatio);

}

// 从文件加载Map
Map::Map(string file_name)
{
}

Map::~Map()
{
    /*releaseMatrix<TMapPara>(MMapResource, x, y);
    releaseMatrix<TMapPara>(MMapDefenseRatio, x, y);
    releaseMatrix<TMapPara>(MMapAttackRatio, x, y);*/
}

}