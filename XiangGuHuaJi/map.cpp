/* XiangGuHuaJi 2016, map.cpp
 *
 */

#include"map.h"

namespace XGHJ
{


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
