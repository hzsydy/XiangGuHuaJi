/* XiangGuHuaJi 2016, game.cpp
 * 
 */

#include "game.h"

namespace XGHJ 
{

inline float x2plusy2(float x, float y){return x*x+y*y;}


Game::Game(Map& map, int playersize)
    : map(map), playerSize(playersize),
      round(0), isValid(true)
{
	//TODO
	//init all the vectors using vector::resize

	const float pi = 3.1416f;
	MilitaryKernel.resize(2*MILITARY_KERNEL_SIZE-1);
	for (int i=0; i<2*MILITARY_KERNEL_SIZE-1; i++)
	{
		MilitaryKernel[i].resize(2*MILITARY_KERNEL_SIZE-1);
		for (int j=0; j<2*MILITARY_KERNEL_SIZE-1; j++)
		{
			float f = x2plusy2((float)(i-MILITARY_KERNEL_SIZE+1),(float)(j-MILITARY_KERNEL_SIZE+1));
			f /= -2*MILITARY_KERNEL_SIGMA_2;
			f = (float)exp(f);
			f /= 2*MILITARY_KERNEL_SIGMA_2*pi;
			MilitaryKernel[i][j] = f;
		}
	}
	printVecMat<float>(MilitaryKernel, "MilitaryKernel");
}

Game::~Game()
{
    
}

// Round<0>
//TODO  init the player, call each player to select their birthplace
bool Game::Start()
{
	round = 0;
	++round;

	//TODO
	//选择出生点

	return true;
}

//Game Logic
bool Game::Run(vector<vector<TMilitaryCommand> > & MilitaryCommandMap,
	vector<vector<TDiplomaticCommand> > & DiplomaticCommandMap)
{
    DiplomacyPhase(DiplomaticCommandMap);
    MilitaryPhase(MilitaryCommandMap);
    ProducingPhase();

    ++round;
    if (round>=MAX_ROUND) isValid=false;

    return isValid;    
}

//Diplomacy Phase (Deal with DiplomaticCommandMap)
bool Game::DiplomacyPhase(vector<vector<TDiplomaticCommand> > & DiplomaticCommandMap)
{
	//TODO
	//外交
	//下面的狗比代码可能可以复用，皮埃尔你可以研究一下有没有软用
	/*
    for (TId i=0; i<PlayerSize; ++i)
        for (TId j=0; j<PlayerSize; ++j)        
            if (TruceTreaty[i][j]>0) 
            {
                --TruceTreaty[i][j];
                --TruceTreaty[i][j];
                if (0==TruceTreaty[i][j])
                    Diplomacy_[i][j] = Diplomacy_[j][i] = Neutral;
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
						Diplomacy_[i][j] = AtWar;
                        Diplomacy_[j][i] = AtWar;
					}
					break;
				case AtTruce:
					if (DiplomaticCommandMap[i][j] == FormAlliance
                        && DiplomaticCommandMap[j][i] == FormAlliance)
                    {
                        Diplomacy_[i][j]  = Diplomacy_[j][i]  = Allied;
                        TruceTreaty[i][j] = TruceTreaty[j][i] = 0;
                    }
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
						Diplomacy_[i][j]  = Diplomacy_[j][i]  = AtTruce;
						TruceTreaty[i][j] = TruceTreaty[j][i] = TRUCE_TIME;
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
                        Diplomacy_[i][j]  = Diplomacy_[j][i]  = AtTruce; 
                        TruceTreaty[i][j] = TruceTreaty[j][i] = TRUCE_TIME;
					}
					break;
				}
			}
		}
	}
	*/
    return false; //TODO
}

//TODO
//这一段所代表的阶段已经不复存在了 大概没有用了把 记得删掉
/*
//Construction Phase (Deal with MilitaryCommandList)
bool Game::ConstructionPhase(vector<cv::Mat> & MilitaryCommandList)
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
*/
//Military Phase (Deal with DefensePointsMap ,AttackPointsMap)
bool Game::MilitaryPhase(vector<vector<TMilitaryCommand> > & MilitaryCommandList)
{
	//TODO
	//全是opencv代码，一点都没法复用，星宇你就老老实实写好了
	
    return false; //TODO
}

//Producing Phase (Deal with MapResource, PlayerInfoList)
bool Game::ProducingPhase()
{
	//TODO
	//谁来写个首都和判定包围的部分

	
    for (TId id=0; id<playerSize; ++id)
	{
		playerArea[id] = 0;
		// lowest income
		TMoney new_saving = 1;

        // map income 
        for (TMap j=0; j<map.getRows(); ++j)
            for (TMap i=0; i<map.getCols(); ++i)
                if (globalMap[i][j] == id) 
				{
					new_saving+=map.getMapRes()[i][j];
					playerArea[id]++;
				}
        // city income
        new_saving += (TMoney)(UNIT_CITY_INCOME * (float)round);

        // corruption 
        playerSaving[id] = (TMoney)((1-(float)(playerArea[id])*CORRUPTION_COEF) * (float) playerSaving[id]);
		playerSaving[id] += new_saving;
    }

    return false; //TODO
}

//Check the winner and the loser (Deal with PlayerInfoList)
bool Game::CheckWinner()
{
	//TODO
	//检查胜负
    return false; //TODO
}

PlayerInfo Game::getPlayerInfo(TId id) const
{
	//TODO
	PlayerInfo p;
	return p;
}
MapPointInfo Game::getMapPointInfo(TPosition pos) const
{
	//TODO
	MapPointInfo mp;
	return mp;
}

Info Game::generateInfo(TId id) const
{
	//TODO
	Info i;
	return i;
}

}
