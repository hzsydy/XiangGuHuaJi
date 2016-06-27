/* XiangGuHuaJi 2016, game.h
 *
 * 实现游戏的主逻辑
 *    （第一回合特有）选择出生点
 *    建造
 *    外交
 *    战争
 *    检查胜负
 * 
 */

#ifndef _XIANGGUHUAJI_GAME_H__
#define _XIANGGUHUAJI_GAME_H__

#include<vector>

#include<opencv2/opencv.hpp>

#include"map.h"
#include"player.h"
#include"definition.h"

namespace XGHJ {

using namespace std;

class Player;

class Game
{
public:
    Game(Map& map, vector<Player>& players);
    ~Game();

    bool Run();

    TRound round;
    TId    PlayerSize;
    
    cv::Mat         MapResource, MapAttack, MapDefense;
    vector<cv::Mat> MilitaryMap;
    cv::Mat         DefensePoints;
    vector<cv::Mat> AttackPointsMap;

private:
    Map& map;
    vector<Player>& players;

};

}


#endif