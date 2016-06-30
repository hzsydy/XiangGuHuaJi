/* XiangGuHuaJi 2016, player.h
 *
 */

#ifndef _XIANGGUHUAJI_PLAYER_H__
#define _XIANGGUHUAJI_PLAYER_H__


#include<string>

#include<opencv2/opencv.hpp>

#include"definition.h"

#include "ai.h"
#include "info.hpp"

namespace XGHJ {

using namespace std;

typedef void (*TPlayerAi)(Info& info); 

class Player
{
public:
    Player();
    Player(string file_name, int id);
    ~Player();

    bool Player::Run(Info &info);

    inline bool     isValid() { return Valid; }

private:
    TPlayerAi       player_ai;
    string          file_name;
    TId             id;
    bool            Valid;
};

}


#endif
