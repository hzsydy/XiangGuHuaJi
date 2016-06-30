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

    ConstructionPhase(MilitaryCommandList);
    DiplomacyPhase(DiplomaticCommandMap);
    MilitaryPhase(MilitaryCommandList);
    ProducingPhase();

    ++Round;
    if (Round>=MAX_ROUND) isValid_=false;


    cout << "finished" << endl;

    return isValid_;    
}



//Construction Phase (Deal with MilitaryCommandList)
bool Game::ConstructionPhase(vector<cv::Mat/*TMatMilitary*/> & MilitaryCommandList)
{
    return false; //TODO
}

//Diplomacy Phase (Deal with DiplomaticCommandMap)
bool Game::DiplomacyPhase(vector<vector<TDiplomaticCommand> > & DiplomaticCommandMap)
{
	for (TId i=0; i<PlayerSize; i++)
	{
		for (TId j=0; j<PlayerSize; j++)
		{
			if (i == j)
			{
				Diplomacy_[i][j] = Allied;
			}
			else
			{
				switch (Diplomacy_[i][j])
				{
				case Neutral:
					break;
				case AtTruce:
					break;
				case Undiscovered:
					break;
				case Allied:
					break;
				case AtWar:
					break;
				}
			}
		}
	}
    return false; //TODO
}

//Military Phase (Deal with DefensePointsMap ,AttackPointsMap)
bool Game::MilitaryPhase(vector<cv::Mat/*TMatMilitary*/> & MilitaryCommandList)
{
    return false; //TODO
}

//Producing Phase (Deal with MapResource, PlayerInfoList)
bool Game::ProducingPhase()
{
    return false; //TODO
}

//Check the winner and the loser (Deal with PlayerInfoList)
bool Game::CheckWinner()
{
    return false; //TODO
}

}
