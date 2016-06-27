/* XiangGuHuaJi 2016, game.h\
 *
 */

#ifndef _XIANGGUHUAJI_GAME_H__
#define _XIANGGUHUAJI_GAME_H__

#include<vector>

#include<opencv2/opencv.hpp>

#include"map.h"
#include"player.h"

namespace XGHJ {

using namespace std;

class Game
{
public:
    Game(Map& map, vector<Player>& players);
    ~Game();

    bool Run();
private:
    Map& map;
    vector<Player>& players;
};

}


#endif