/* XiangGuHuaJi 2016, map.cpp
 *
 */

#include"map.h"

namespace XGHJ
{

Map::Map()
{
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

bool Map::load(string file_name)
{
	Json::Value root;
	Json::Reader reader;
	std::ifstream file(file_name, std::ifstream::binary);
	if(!reader.parse(file, root, true)){
		std::cerr  << "Failed to parse configuration\n"
			<< reader.getFormattedErrorMessages();
		return false;
	}

	x = root.get("cols", "UTF-8" ).asInt();
	y = root.get("rows", "UTF-8" ).asInt();

	MapResource = cv::Mat(x, y, TMatMapPara);
	MapAttackRatio  = cv::Mat(x, y, TMatMapPara);
	MapDefenseRatio = cv::Mat(x, y, TMatMapPara);

	const Json::Value resources = root["resources"];
	const Json::Value landscape = root["landscape"];
	for (int i=0; i<x; i++)
	{
		for (int j=0; j<y; j++)
		{
			MapResource.at<TMatMapPara>(j, i) = (TMatMapPara)(resources[i][j].asUInt());
			MapAttackRatio.at<TMatMapPara>(j, i) = (TMatMapPara)(landscape[i][j]["Attack"].asUInt());
			MapDefenseRatio.at<TMatMapPara>(j, i) = (TMatMapPara)(landscape[i][j]["Defense"].asUInt());
		}
	}
	convertMyMat();
}

}