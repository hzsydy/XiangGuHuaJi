/* XiangGuHuaJi 2016, game.cpp
 * 
 */

#include "game.h"

namespace XGHJ 
{

inline float x2plusy2(float x, float y){return x*x+y*y;}


Game::Game(Map& map, int playersize)
	: map(map), playerSize(playersize), playerSaving(playersize, INITIAL_PLAYER_MONEY),
      round(0), isValid(true)
{
	rows = map.getRows();
	cols = map.getCols();
	//init all the vectors using vector::resize
	globalMap.resize(cols);
	isSieged.resize(cols);
	for (TMap i=0; i<cols; i++)
	{
		globalMap[i].resize(rows);
		isSieged[i].resize(rows);
		for (TMap j=0; j<rows; j++)
		{
			globalMap[i][j] = NEUTRAL_PLAYER_ID;
			isSieged[i][j] = false;
		}
	}
	playerCapital.resize(playerSize);
	playerArea.resize(playerSize);
	diplomacy.resize(playerSize);
	for (TId i=0; i<playersize; i++)
	{
		diplomacy[i].resize(playerSize);
		playerArea[i] = 0;
		for (TId j=0; j<playersize; j++)
		{
			if (i == j)
			{
				diplomacy[i][j] = Allied;
			}
			else
			{
				diplomacy[i][j] = Undiscovered;
			}
		}
	}

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
	//设置首都为默认出生点

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

	//读取DiplomaticCommandMap
	//更新Diplomacy（记得战争需要造WAR_JUSTIFY_TIME回合，不要忘了维护列表）
	//记得扣钱（造借口要扣WAR_JUSTIFY_PRICE的钱）

	//实现次要函数Game::getWarList

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

vector<TId> Game::getWarList(TId id) const
{
	vector<TId> wl;
	//TODO
	return wl;
}

bool Game::MilitaryPhase(vector<vector<TMilitaryCommand> > & MilitaryCommandList)
{
	//TODO
	//全是opencv代码，一点都没法复用，星宇你就老老实实写好了

	//读取MilitaryCommandList并且扣UNIT_BOMB_COST的钱

	//更新GlobalMap，使用MilitaryKernel

	//阈值取SUPPESS_LIMIT，如果两边一样就强行把地打成中立

	//检查新首都
	//要是无法放置新首都，就把新首都的位置设定为invalidPos

	//检查包围
	//更新isSieged
	
    return false; //TODO
}

//Producing Phase (Deal with MapResource, PlayerInfoList)
bool Game::ProducingPhase()
{
	
    for (TId id=0; id<playerSize; ++id)
	{
		playerArea[id] = 0;
		// lowest income
		TMoney new_saving = 1;

        // map income 
		for (TMap i=0; i<cols; i++)
		{
			for (TMap j=0; j<rows; j++)
			{
				if (globalMap[i][j] == id) 
				{
					new_saving+=map.getMapRes()[i][j];
					playerArea[id]++;
				}
			}
		}

        // city income
		if (isPosValid(playerCapital[id]))
		{
			new_saving += (TMoney)(UNIT_CITY_INCOME * (float)round);
		}

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

PlayerInfo Game::getPlayerInfo(TId id, TId playerId) const
{
	PlayerInfo p;
	p.dipStatus = diplomacy[playerId][id];
	if (p.dipStatus == Allied)
	{
		p.isUnion = true;
		p.saving = playerSaving[id];
		p.capital = playerCapital[id];
	}
	else
	{
		p.isUnion = false;
		p.saving = 0;
		p.capital = invalidPos;
	}
	if (p.dipStatus != Undiscovered)
	{
		p.isVisible = true;
		p.warList = getWarList(id);
		p.mapArea = playerArea[id];
	} 
	else
	{
		p.isVisible = true;
		p.warList = vector<TId>();
		p.mapArea = -1;
	}
	return p;
}

TMask Game::isPointVisible(TMap x, TMap y, TId playerId) const
{
	//TODO
	return true;
}

MapPointInfo Game::getMapPointInfo(TMap x, TMap y, TId playerId) const
{
	MapPointInfo mp;
	mp.attackRatio = map.getMapAtk()[x][y];
	mp.defenseRatio = map.getMapDef()[x][y];
	mp.resource = map.getMapRes()[x][y];
	mp.isVisible = isPointVisible(x, y, playerId);
	if (mp.isVisible)
	{
		mp.owner = globalMap[x][y];
		mp.isSieged = isSieged[x][y];
	}
	else
	{
		mp.owner = UNKNOWN_PLAYER_ID;
		mp.isSieged = false;
	}
	return mp;
}

Info Game::generateInfo(TId playerid) const
{
	Info info;
	info.DiplomaticCommandList = vector<TDiplomaticCommand>();
	info.MilitaryCommandList = vector<TMilitaryCommand>();
	info.id = playerid;
	info.playerSize = playerSize;
	info.round = round;
	info.newCapital = invalidPos;
	info.rows = rows;
	info.cols = cols;
	info.playerInfo = vector<PlayerInfo>(playerSize);
	for (TId id=0; id<playerSize; id++)
	{
		info.playerInfo[id] = getPlayerInfo(id, playerid);
	}
	info.mapPointInfo = vector<vector<MapPointInfo> >(cols);
	for (TMap i=0; i<cols; i++)
	{
		info.mapPointInfo[i] = vector<MapPointInfo>(rows);
		for (TMap j=0; j<rows; j++)
		{
			info.mapPointInfo[i][j] = getMapPointInfo(i, j, playerid);
		}
	}
	return info;
}

}
