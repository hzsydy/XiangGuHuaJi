/* XiangGuHuaJi 2016, player.h
 *
 */

#ifndef _XIANGGUHUAJI_PLAYER_H__
#define _XIANGGUHUAJI_PLAYER_H__


#include<string>
#include<windows.h>

#include<opencv2/opencv.hpp>

#include"ai.h"
#include"game.h"
#include"definition.h"

namespace XGHJ {

using namespace std;

typedef void (*TPlayerAi)(Info& info); 

class Game;

class Player
{
public:
    Player();
    Player(string file_name, int id);
    ~Player();

    void Run(Game& game, cv::Mat& MilitaryCommamd, DiplomaticCommand* DiplomaticCommandList);

    inline bool isValid() { return Valid; }

private:
    TPlayerAi player_ai;
    int Id;
    bool Valid;
};

}


#endif
