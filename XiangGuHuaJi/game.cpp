/* XiangGuHuaJi 2016, game.cpp
 * 
 */

#include"game.h"

namespace XGHJ 
{

Game::Game(Map& map, vector<Player>& players)
    : map(map), players(players), PlayerSize(players.size()),
      Round(0), Valid(true)
{
    //MilitaryMap_, AttackPointsMap_, DefensePointsMap_
    MilitaryMap_.resize(PlayerSize);
    AttackPointsMap_.resize(PlayerSize);
    for (TId id=0; id<PlayerSize; ++id)
    {
        MilitaryMap_[id] = cv::Mat::zeros(map.size(), CV_TMilitary);
        AttackPointsMap_[id] = cv::Mat::zeros(map.size(), CV_TAttack);        
    }
    DefensePointsMap_ = cv::Mat::zeros(map.size(), CV_TDefense);

    //PlayerInfoList
    PlayerInfoList.resize(PlayerSize);
    for (TId id=0; id<PlayerSize; ++id)     
    {
        PlayerInfoList[id].id = id;
        PlayerInfoList[id].MapArea = 1;
        PlayerInfoList[id].MilitarySummary = 1; // it should be refreshed later.
    }

    //Diplomacy
    Diplomacy.resize(PlayerSize);
    for (TId id=0; id<PlayerSize; ++id) Diplomacy[id].resize(PlayerSize);
}

Game::~Game()
{
    
}

const TRound Game::MAX_ROUND = 300;

//Game Logic
bool Game::Run()
{
    cout << "Game: Round[" << Round << "] ";

    vector<cv::Mat/*TMatMilitary*/>     MilitaryCommandList(PlayerSize);
    vector<vector<TDiplomaticCommand> > DiplomaticCommandMap(PlayerSize);
    
    Run_0();
    Run_1(MilitaryCommandList, DiplomaticCommandMap);
    Run_2(MilitaryCommandList);
    Run_3(DiplomaticCommandMap);
    Run_4(MilitaryCommandList);
    Run_5();
    Run_6();

    //7.Update 
    ++Round;
    if (Round>=MAX_ROUND) Valid=false;


    cout << "finished" << endl;

    return Valid;    
}

//0.Initialize MilitaryMap,AttackPointsMap,DefensePointsMap
bool Game::Run_0()
{
    MilitaryMap.clear();
    for (TId id=0; id<PlayerSize; ++id) {
        vector<vector<TMilitary> > sub_MilitaryMap;
        convertMat<TMilitary>(MilitaryMap_[id], sub_MilitaryMap);
        MilitaryMap.push_back(sub_MilitaryMap);
    }
    AttackPointsMap.clear();
    for (TId id=0; id<PlayerSize; ++id) {
        vector<vector<TAttack> > sub_AttackPointMap;
        convertMat<TAttack>(AttackPointsMap_[id], sub_AttackPointMap);
        AttackPointsMap.push_back(sub_AttackPointMap);
    }
    convertMat<TDefense>(DefensePointsMap_, DefensePointsMap);
    
    return true;
} 

//1.Each player runs
bool Game::Run_1(vector<cv::Mat/*TMatMilitary*/> &     MilitaryCommandList,
                 vector<vector<TDiplomaticCommand> > & DiplomaticCommandMap )
{    
    for (TId id=0; id<PlayerSize; ++id)
    {
        cv::Mat/*TMatMilitary*/	    MilitaryCommand_;
        vector<TDiplomaticCommand>  DiplomaticCommand(PlayerSize);
        players[id].Run(*this, MilitaryCommand_, DiplomaticCommand);
        MilitaryCommandList[id]     = MilitaryCommand_;
        DiplomaticCommandMap[id]    = DiplomaticCommand;
    }
	return false; //TODO
}

//2.Building (Deal with MilitaryCommandList)
bool Game::Run_2(vector<cv::Mat/*TMatMilitary*/> & MilitaryCommandList)
{
    return false; //TODO
}

//3.Diplomacy (Deal with DiplomaticCommandMap)
bool Game::Run_3(vector<vector<TDiplomaticCommand> > & DiplomaticCommandMap)
{
    return false; //TODO
}

//4.Military (Deal with DefensePointsMap ,AttackPointsMap)
bool Game::Run_4(vector<cv::Mat/*TMatMilitary*/> & MilitaryCommandList)
{
    return false; //TODO
}

//5.Economy (Deal with MapResource, PlayerInfoList)
bool Game::Run_5()
{
    return false; //TODO
}

//6.Check the winner and the loser (Deal with PlayerInfoList)
bool Game::Run_6()
{
    return false; //TODO
}

}
