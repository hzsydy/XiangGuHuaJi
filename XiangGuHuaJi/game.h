/* XiangGuHuaJi 2016, game.h
 *
 * 实现游戏的主逻辑
 *    （第一回合特有）选择出生点
 *    建造
 *    外交
 *    战争
 *    经济
 *    检查胜负
 * 
 */

#ifndef _XIANGGUHUAJI_GAME_H__
#define _XIANGGUHUAJI_GAME_H__


#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<opencv2/opencv.hpp>

#include"definition.h"
#include"matrix.hpp"

#include"map.h"
#include"player.h"


namespace XGHJ {

using namespace std;

class Player;

class Game
{
public:
    Game(Map& map, vector<Player>& players);
    ~Game();

    bool Run();

    TRound       round;
    const TId    PlayerSize;
    
    TMapSize     x,y;

    Map&                map;
    vector<cv::Mat>     MilitaryMap;
    cv::Mat             DefensePointsMap;
    vector<cv::Mat>     AttackPointsMap;
    vector<PlayerInfo>  PlayerInfoList;
    cv::Mat             Ownership;

    DiplomaticStatus**  DiplomaticMap;

private:
    
    vector<Player>& players;

};

}


#endif