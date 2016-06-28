/* XiangGuHuaJi 2016, game.cpp
 *
 */

#include"game.h"

namespace XGHJ 
{

Game::Game(Map& map, vector<Player>& players)
    : map(map), players(players), PlayerSize(players.size()),
    Round(0)
{
}

Game::~Game()
{
    
}

bool Game::Run()
{
    vector<cv::Mat> MilitaryCommandList;
    vector<vector<TDiplomaticCommand> > DiplomaticCommandMap;

    //0. 预处理 & 每一个玩家行动

    //1.建造 处理MilitaryMapList

    //2.外交 处理DiplomaticCommandMap

    //3.军事 更新DefensePoints,AttackPointsMap

    //4.经济 更新PlayerInfoList

    //5.胜负判定

    return true;    
}

}