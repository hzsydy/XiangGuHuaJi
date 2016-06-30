/* XiangGuHuaJi 2016, game.cpp
 * 
 */

#include"game.h"

namespace XGHJ 
{
const TRound Game::MAX_ROUND = 20;

Game::Game(Map& map, int playersize)
    : map(map), PlayerSize(playersize),
      Round(0), isValid_(true)
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
	GlobalMap_ = cv::Mat::zeros(map.size(), CV_TId);

    //PlayerInfoList
    PlayerInfoList_.resize(PlayerSize);
    for (TId id=0; id<PlayerSize; ++id)     
    {
        PlayerInfoList_[id].id = id;
        PlayerInfoList_[id].MapArea = 1;
        PlayerInfoList_[id].MilitarySummary = 1; // it should be refreshed later.
    }

    //OwnershipMasks_
    OwnershipMasks_.resize(PlayerSize);
    for (TId id=0; id<PlayerSize; ++id) 
		OwnershipMasks_[id]=cv::Mat::zeros(map.size(), CV_8UC1);
    
    //Diplomacy
    resizeVector<TDiplomaticStatus>(Diplomacy_, PlayerSize, PlayerSize);
	fillVector<TDiplomaticStatus>(Diplomacy_, TDiplomaticStatus::Undiscovered);

}

Game::~Game()
{
    
}

bool Game::Start()
{
	Round = 0;
	++Round;
	return true;
}

//Game Logic
bool Game::Run(vector<cv::Mat/*TMatMilitary*/> & MilitaryCommandList,
			   vector<vector<TDiplomaticCommand> > & DiplomaticCommandMap)
{
    cout << "Game: Round[" << Round << "] ";

    //Run_0();
    Run_1(MilitaryCommandList, DiplomaticCommandMap);
    Run_2(MilitaryCommandList);
    Run_3(DiplomaticCommandMap);
    Run_4(MilitaryCommandList);
    Run_5();
    Run_6();

    //7.Update 
    ++Round;
    if (Round>=MAX_ROUND) isValid_=false;


    cout << "finished" << endl;

    return isValid_;    
}

/*
//0.Initialize MilitaryMap,AttackPointsMap,DefensePointsMap,GlobalMap
bool Game::Run_0()
{
    //MilitaryMap
    MilitaryMap.clear();
    for (TId id=0; id<PlayerSize; ++id) {
        vector<vector<TMilitary> > sub_MilitaryMap;
        convertMat<TMilitary>(MilitaryMap_[id], sub_MilitaryMap);
        MilitaryMap.push_back(sub_MilitaryMap);
    }
    //AttackPointsMap
    AttackPointsMap.clear();
    for (TId id=0; id<PlayerSize; ++id) {
        vector<vector<TAttack> > sub_AttackPointMap;
        convertMat<TAttack>(AttackPointsMap_[id], sub_AttackPointMap);
        AttackPointsMap.push_back(sub_AttackPointMap);
    }
    //DefensePointsMap
    convertMat<TDefense>(DefensePointsMap_, DefensePointsMap);
    //GlobalMap
    for (TMapSize j=0; j<map.cols; ++j)
        for (TMapSize i=0; i<map.rows; ++i) 
            GlobalMap[j][i]=255;
    for (TId id=0; id<PlayerSize; ++id) 
        for (TMapSize j=0; j<map.cols; ++j)
            for (TMapSize i=0; i<map.rows; ++i)
                if (OwnershipMasks_[id].at<TId>(i,j)) GlobalMap[j][i]=id;

    return true;
} 
*/

//1.Each player runs
bool Game::Run_1(vector<cv::Mat/*TMatMilitary*/> &     MilitaryCommandList,
                 vector<vector<TDiplomaticCommand> > & DiplomaticCommandMap )
{    
    
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
