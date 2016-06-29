/* XiangGuHuaJi 2016, game.h
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
#include"converter.hpp"

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
    bool Run_0();
    bool Run_1(vector<cv::Mat/*TMatMilitary*/> &       MilitaryCommandList,
               vector<vector<TDiplomaticCommand> > &   DiplomaticCommandMap);
    bool Run_2(vector<cv::Mat/*TMatMilitary*/> &       MilitaryCommandList);
    bool Run_3(vector<vector<TDiplomaticCommand> > &   DiplomaticCommandMap);
    bool Run_4(vector<cv::Mat/*TMatMilitary*/> &       MilitaryCommandList);
    bool Run_5();
    bool Run_6();

    Map&                map;
    vector<Player>&     players;

    vector<cv::Mat>     MilitaryMap_;
    vector<cv::Mat>     AttackPointsMap_;
    cv::Mat             DefensePointsMap_;
    vector<vector<vector<TMilitary> > >	MilitaryMap;
    vector<vector<vector<TAttack> > >	AttackPointsMap;
    vector<vector<TDefense> >	        DefensePointsMap;

    vector<cv::Mat>     OwnershipMasks_;
    vector<TPlayerInfo>	PlayerInfoList;
    vector<vector<TDiplomaticStatus> >	Diplomatic;
    TRound              Round;
    TId                 PlayerSize;

private:

};

}


#endif