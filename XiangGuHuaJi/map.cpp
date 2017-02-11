/* XiangGuHuaJi 2016, map.cpp
 *
 */

#include"map.h"

namespace XGHJ
{

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

	MapResource_.resize(cols);
	MapAttackRatio_.resize(cols);
	MapDefenseRatio_.resize(cols);
	for (int i=0; i<cols; i++)
	{
		MapResource_[i].resize(rows);
		MapAttackRatio_[i].resize(rows);
		MapDefenseRatio_[i].resize(rows);
		for (int j=0; j<rows; j++)
		{
			int fuck;
			fuck = (root["resources"][i][j]).asInt();
			MapResource_[i][j] = (TMoney)(fuck);
			fuck = (root["landscape"][i][j]["Attack"]).asInt();
			MapAttackRatio_[i][j] = (TMilitary)(fuck);
			fuck = (root["landscape"][i][j]["Defense"]).asInt();
			MapDefenseRatio_[i][j] = (TMilitary)(fuck);
		}
	}
	return true;
}

bool Map::easy_load(string filename) {

    using namespace std;

    try{
        ifstream ifs(filename);
        int t;
        ifs >> t; cols = t;
        ifs >> t; rows = t;

        MapResource_.resize(cols);
        MapAttackRatio_.resize(cols);
        MapDefenseRatio_.resize(cols);
        for (int i=0; i<cols; ++i) MapResource_[i].resize(rows);
        for (int i=0; i<cols; ++i) MapAttackRatio_[i].resize(rows);
        for (int i=0; i<cols; ++i) MapDefenseRatio_[i].resize(rows);

        // res
        for (int y=0; y<rows; ++y)
            for (int x=0; x<cols; ++x) {
                ifs >> t;
                MapResource_[x][y] = t;
            }
        // atk
        for (int y=0; y<rows; ++y)
            for (int x=0; x<cols; ++x) {
                ifs >> t;
                MapAttackRatio_[x][y] = t;
            }
        // def
        for (int y=0; y<rows; ++y)
            for (int x=0; x<cols; ++x) {
                ifs >> t;
                MapDefenseRatio_[x][y] = t;
            }
        ifs.close();
        return true;
    }
    catch(exception e) {
        cout << e.what(); 
    }
    return false;
}

}
