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
	std::ifstream file(file_name.c_str(), std::ifstream::binary);
	if(!reader.parse(file, root, true)){
		std::cerr  << "Failed to parse configuration\n"
			<< reader.getFormattedErrorMessages();
		return false;
	}

	cols = root["cols"].asInt();
	rows = root["rows"].asInt();

	MapResource_ = cv::Mat(rows, cols, CV_TMapPara);
	MapAttackRatio_  = cv::Mat(rows, cols, CV_TMapPara);
	MapDefenseRatio_ = cv::Mat(rows, cols, CV_TMapPara);

	for (int i=0; i<cols; i++)
	{
		for (int j=0; j<rows; j++)
		{
			int fuck;
			fuck = (root["resources"][i][j]).asInt();
			MapResource_.at<TMapPara>(j, i) = (TMapPara)(fuck);
			fuck = (root["landscape"][i][j]["Attack"]).asInt();
			MapAttackRatio_.at<TMapPara>(j, i) = (TMapPara)(fuck);
			fuck = (root["landscape"][i][j]["Defense"]).asInt();
			MapDefenseRatio_.at<TMapPara>(j, i) = (TMapPara)(fuck);
		}
	}
	convertMyMat();
	return true;
}

}
