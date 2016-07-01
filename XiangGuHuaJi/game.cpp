/* XiangGuHuaJi 2016, game.cpp
 * 
 */

#include"game.h"

namespace XGHJ 
{

const TMilitary     Game::MAX_MILITARY = 255;
const TRound        Game::MAX_ROUND = 20;
const TSaving       Game::UNIT_SAVING = 10;
const TRound        Game::TRUCE_TIME = 5;
const float			Game::CORRUPTION_COEF = 0.001f;
const float			Game::DEPRECIATION_COEF = 0.3f;

#ifdef GAME_DEBUG
const TSaving       Game::UNIT_CITY_INCOME = 100; 
#else
const TSaving       Game::UNIT_CITY_INCOME = 1;
#endif




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
	fillVector<TDiplomaticStatus>(Diplomacy_, Undiscovered);
    for (TId id=0; id<PlayerSize; ++id) Diplomacy_[id][id] = Allied;
}

Game::~Game()
{
    
}

// Round<0>
bool Game::Start()
{
	Round = 0;
	++Round;

    //TODO  init the player, call each player to select their birthplace
    for (TId i=0; i<PlayerSize; ++i) OwnershipMasks_[i].at<TMask>(i,i)=255;
	return true;
}

//Game Logic
bool Game::Run(vector<cv::Mat/*TMatMilitary*/> & MilitaryCommandList,
			   vector<vector<TDiplomaticCommand> > & DiplomaticCommandMap)
{
    DiplomacyPhase(DiplomaticCommandMap);
    ConstructionPhase(MilitaryCommandList);
    MilitaryPhase(MilitaryCommandList);
    ProducingPhase();

    ++Round;
    if (Round>=MAX_ROUND) isValid_=false;

    return isValid_;    
}

//Diplomacy Phase (Deal with DiplomaticCommandMap)
bool Game::DiplomacyPhase(vector<vector<TDiplomaticCommand> > & DiplomaticCommandMap)
{
	{
		int c = TruceList.size();
		for (int i=0; i<c; i++)
		{
			TruceTreaty tt = TruceList.front();
			TruceList.pop();
			if (tt.lasttime == 1)
			{
				//end this time
				Diplomacy_[tt.player1][tt.player2] = Neutral;
				Diplomacy_[tt.player2][tt.player1] = Neutral;
			}
			else
			{
				Diplomacy_[tt.player1][tt.player2] = AtTruce;
				Diplomacy_[tt.player2][tt.player1] = AtTruce;
				tt.lasttime--;
				TruceList.push(tt);
			}
		}
	}

	for (TId i=0; i<PlayerSize; i++)
	{
		for (TId j=0; j<i; j++)
		{
			{
				switch (Diplomacy_[i][j])
				{
				case Neutral:
                    if (DiplomaticCommandMap[i][j] == FormAlliance 
						&& DiplomaticCommandMap[j][i] == FormAlliance)
					{
						//allied
						Diplomacy_[i][j] = Allied;
						Diplomacy_[j][i] = Allied;
					}
					else if (DiplomaticCommandMap[i][j] != DeclareWar 
						&& DiplomaticCommandMap[j][i] != DeclareWar)
					{
						//neutral
						Diplomacy_[i][j] = Neutral;
						Diplomacy_[j][i] = Neutral;
					}
					else
					{
						//at war
						Diplomacy_[i][j] = AtTruce;
						Diplomacy_[j][i] = AtTruce;
					}
					break;
				case AtTruce:
					//at truce
					//solved at begin
					break;
				case Undiscovered:
					//Discovery will be solved in Military Phase
					break;
				case Allied:
					if (DiplomaticCommandMap[i][j] == FormAlliance 
						&& DiplomaticCommandMap[j][i] == FormAlliance)
					{
						//allied
						;
					}
					else
					{
						//at truce
						Diplomacy_[i][j] = AtTruce;
						Diplomacy_[j][i] = AtTruce;
						TruceTreaty tt;
						tt.lasttime = TRUCE_TIME;
						tt.player1 = i;
						tt.player2 = j;
						TruceList.push(tt);
						//TODO units on the used-to-be allied islands should be destroyed

					}
					break;
				case AtWar:
					if (DiplomaticCommandMap[i][j] == DeclareWar 
						|| DiplomaticCommandMap[j][i] == DeclareWar)
					{
						//at war
						;
					}
					else
					{
						//at truce
						Diplomacy_[i][j] = AtTruce;
						Diplomacy_[j][i] = AtTruce;
						TruceTreaty tt;
						tt.lasttime = TRUCE_TIME;
						tt.player1 = i;
						tt.player2 = j;
						TruceList.push(tt);
					}
					break;
				}
			}
		}
	}
    return false; //TODO
}

//Construction Phase (Deal with MilitaryCommandList)
bool Game::ConstructionPhase(vector<cv::Mat/*TMatMilitary*/> & MilitaryCommandList)
{
    for (TId id=0; id<PlayerSize; ++id)
    {
        if (PlayerInfoList_[id].Saving==0) continue;

        // Check validity of MilitaryCommandList
        cv::Mat & mat = MilitaryCommandList[id];
        bool valid = true;
        // check the area
        cv::Mat control_mask = cv::Mat::zeros(map.size(), CV_TMask);
        for (TId i=0; i<PlayerSize; ++i)
            if (Allied==Diplomacy_[id][i]) control_mask+=OwnershipMasks_[i];
        // check the money
        TMilitarySummary sum = 0; 
        for (TMapSize j=0; j<map.getRows(); ++j) 
            for (TMapSize i=0; i<map.getCols(); ++i)
            {
                TMilitary& point_military = mat.at<TMilitary>(j, i);
                if (!control_mask.at<TMask>(j, i)) point_military = 0;
                if (point_military + MilitaryMap_[id].at<TMilitary>(j, i) > MAX_MILITARY) 
                    point_military = MAX_MILITARY - MilitaryMap_[id].at<TMilitary>(j, i);
                sum+=point_military;
            }
        if (sum * UNIT_SAVING > PlayerInfoList_[id].Saving) 
        {
            // spend too much money
            valid = false;
            cout << "[Warning] Player {" << (int)id << "} tried to spend more money than he has!!! " << endl;
            unsigned int ratio = sum * UNIT_SAVING / PlayerInfoList_[id].Saving, mult = 0;
            while (ratio>0) { ++mult; ratio >>= 1; }
            for (TMapSize j=0; j<map.getRows(); ++j)
                for (TMapSize i=0; i<map.getCols(); ++i)
                    mat.at<TMilitary>(j, i) >>= mult;
        }
        // sum 
        if (!valid)
        {
            sum = 0;
            for (TMapSize j=0; j<map.getRows(); ++j) 
                for (TMapSize i=0; i<map.getCols(); ++i)
                {
                    TMilitary& point_military = mat.at<TMilitary>(j, i);
                    sum+=point_military;
                }
        }
        // execute
        if ( sum * UNIT_SAVING <= PlayerInfoList_[id].Saving)
        {
            PlayerInfoList_[id].Saving -= sum * UNIT_SAVING;
            MilitaryMap_[id] += mat;
        }
    }


    return true; //TODO
}

//Military Phase (Deal with DefensePointsMap ,AttackPointsMap)
bool Game::MilitaryPhase(vector<cv::Mat/*TMatMilitary*/> & MilitaryCommandList)
{
    // refresh OwnershipMask_
	// refresh GlobalMap
	
	/*
	for (TId id=0; id<PlayerSize; ++id)
	{
		cv::Mat mat = MilitaryCommandList[id];
		vector<vector<cv::Point> > contours;
		vector<cv::Vec4i> hierarchy;
		cv::findContours(mat, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	}
	*/
    return false; //TODO
}

//Producing Phase (Deal with MapResource, PlayerInfoList)
bool Game::ProducingPhase()
{
    for (TId id=0; id<PlayerSize; ++id)
	{
		// lowest income
        TSaving new_saving = 1;

        // map income 
        for (TMapSize j=0; j<map.getRows(); ++j)
            for (TMapSize i=0; i<map.getCols(); ++i)
                if (OwnershipMasks_[id].at<TMask>(j, i)) new_saving+=map.getMapRes().at<TMapPara>(j, i);
        // city income
        new_saving += UNIT_CITY_INCOME*PlayerInfoList_[id].MilitarySummary;

        // corruption 
        PlayerInfoList_[id].Saving+=
			(TSaving)((1-(float)(PlayerInfoList_[id].MapArea)*CORRUPTION_COEF) * (float)new_saving);
    }

    return false; //TODO
}

//Check the winner and the loser (Deal with PlayerInfoList)
bool Game::CheckWinner()
{
    // refresh PlayerInfoList

    return false; //TODO
}

}
