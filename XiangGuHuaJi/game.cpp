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
	roundToJusifyWar.resize(playerSize);
	backstabUsed.resize(playerSize);
	for (TId i=0; i<playersize; i++)
	{
		diplomacy[i].resize(playerSize);
		roundToJusifyWar[i].resize(playerSize);
		playerArea[i] = 0;
		backstabUsed[i] = false;
		for (TId j=0; j<playersize; j++)
		{
			roundToJusifyWar[i][j] = 0;
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

	for (TId i = 0; i < playerSize-1; ++i)
		for (TId j = i+1; j < playerSize; ++j)
			if (i != j && diplomacy[i][j] != Undiscovered)
			{
				if (DiplomaticCommandMap[i][j] == JustifyWar)
				{
					if (playerSaving[i] - WAR_JUSTIFY_PRICE >= 0)
						playerSaving[i] -= WAR_JUSTIFY_PRICE;
					else
						DiplomaticCommandMap[i][j] = KeepNeutral;

				}
				if (DiplomaticCommandMap[j][i] == JustifyWar)
				{
					if (playerSaving[j] - WAR_JUSTIFY_PRICE >= 0)
						playerSaving[j] -= WAR_JUSTIFY_PRICE;
					else
						DiplomaticCommandMap[j][i] = KeepNeutral;
				}
				if (DiplomaticCommandMap[i][j] == Backstab)
				{
					if (backstabUsed[i] == false)
						backstabUsed[i] = true;
					else 
						DiplomaticCommandMap[i][j] = KeepNeutral;
				}
				if (DiplomaticCommandMap[j][i] == Backstab)
				{
					if (backstabUsed[j] == false)
						backstabUsed[j] = true;
					else 
						DiplomaticCommandMap[j][i] = KeepNeutral;
				}
				switch (DiplomaticCommandMap[i][j])
				{
				case KeepNeutral:
					roundToJusifyWar[i][j] = 0;
					switch (DiplomaticCommandMap[j][i])
					{
					case KeepNeutral:
						roundToJusifyWar[j][i] = 0;
						diplomacy[i][j] = diplomacy[j][i] = Neutral;
						break;
					case FormAlliance:
						roundToJusifyWar[j][i] = 0;
						diplomacy[i][j] = diplomacy[j][i] = Neutral;
						break;
					case JustifyWar:
						if (diplomacy[i][j] != AtWar)
						{
							++roundToJusifyWar[j][i];
							if (roundToJusifyWar[j][i] >= WAR_JUSTIFY_TIME)
							{
								roundToJusifyWar[j][i] = 0;
								diplomacy[i][j] = diplomacy[j][i] = AtWar;
							}
							else
								diplomacy[i][j] = diplomacy[j][i] = Neutral;
						}
						break;
					case Backstab:
						roundToJusifyWar[j][i] = 0;
						diplomacy[i][j] = diplomacy[j][i] = AtWar;
						break;
					}
					break;
				case FormAlliance:
					roundToJusifyWar[i][j] = 0;
					switch (DiplomaticCommandMap[j][i])
					{
					case KeepNeutral:
						roundToJusifyWar[j][i] = 0;
						diplomacy[i][j] = diplomacy[j][i] = Neutral;
						break;
					case FormAlliance:
						roundToJusifyWar[j][i] = 0;
						diplomacy[i][j] = diplomacy[j][i] = Allied;
						break;
					case JustifyWar:
						if (diplomacy[i][j] != AtWar)
						{
							++roundToJusifyWar[j][i];
							if (roundToJusifyWar[j][i] >= WAR_JUSTIFY_TIME)
							{
								roundToJusifyWar[j][i] = 0;
								diplomacy[i][j] = diplomacy[j][i] = AtWar;
							}
							else
								diplomacy[i][j] = diplomacy[j][i] = Neutral;
						}
						break;
					case Backstab:
						roundToJusifyWar[j][i] = 0;
						diplomacy[i][j] = diplomacy[j][i] = AtWar;
						break;
					}
					break;
				case JustifyWar:
					if (diplomacy[i][j] != AtWar)
					{
						switch (DiplomaticCommandMap[j][i])
						{
						case KeepNeutral:
							++roundToJusifyWar[i][j];
							roundToJusifyWar[j][i] = 0;
							if (roundToJusifyWar[i][j]>= WAR_JUSTIFY_TIME)
							{
								roundToJusifyWar[i][j] = 0;
								diplomacy[i][j] = diplomacy[j][i] = AtWar;
							}
							else
								diplomacy[i][j] = diplomacy[j][i] = Neutral;
							break;
						case FormAlliance:
							++roundToJusifyWar[i][j];
							roundToJusifyWar[j][i] = 0;
							if (roundToJusifyWar[i][j]>= WAR_JUSTIFY_TIME)
							{
								roundToJusifyWar[i][j] = 0;
								diplomacy[i][j] = diplomacy[j][i] = AtWar;
							}
							else
								diplomacy[i][j] = diplomacy[j][i] = Neutral;
							break;
						case JustifyWar:
							++roundToJusifyWar[i][j];
							++roundToJusifyWar[j][i];
							if (roundToJusifyWar[i][j]>= WAR_JUSTIFY_TIME || roundToJusifyWar[j][i]>= WAR_JUSTIFY_TIME)
							{
								roundToJusifyWar[i][j] = roundToJusifyWar[j][i] = 0;
								diplomacy[i][j] = diplomacy[j][i] = AtWar;
							}
							else
								diplomacy[i][j] = diplomacy[j][i] = Neutral;
							break;
						case Backstab:
							roundToJusifyWar[i][j] = roundToJusifyWar[j][i] = 0;
							diplomacy[i][j] = diplomacy[j][i] = AtWar;
							break;
						}
					}				
					break;
				case Backstab:
					roundToJusifyWar[i][j] = roundToJusifyWar[j][i] = 0;
					diplomacy[i][j] = diplomacy[j][i] = AtWar;
					break;
				}
			}
    return true; //TODO
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
	DiscoverCountry();

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
	if (globalMap[x][y] == playerId) return true;
	if (globalMap[x][y] != NEUTRAL_PLAYER_ID && (diplomacy[playerId][globalMap[x][y]] == Neutral || diplomacy[playerId][globalMap[x][y]] == Allied))
		return true;
	for (int fi = -1; fi <= 1; fi += 2)
		for (int fj = -1; fj <= 1; fj += 2)
			for (int k = 1; k <= FIELD_BOUNDARY; ++k)
				for (int pi = 0; pi <= k; ++pi)
				{
					int pj = FIELD_BOUNDARY - pi;
					int si = x + fi * pi;
					int sj = y + fj * pj;
					if (si >= 0 && si < cols && sj >= 0 && sj < rows)
						if (globalMap[si][sj]!= NEUTRAL_PLAYER_ID)
							if (globalMap[si][sj] == playerId || diplomacy[playerId][globalMap[si][sj]] == Allied)
								return true;
				}
	return false;
}

MapPointInfo Game::getMapPointInfo(TMap x, TMap y, TId playerId) const
{
	MapPointInfo mp;
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
	info.map = &map;
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

void Game::DiscoverCountry()
{
	for (TMap i = 0; i <cols; ++i)
		for (TMap j = 0; j < rows; ++j)
			for (int fi = -1; fi <= 1; fi += 2)
				for (int fj = -1; fj <= 1; fj += 2)
					for (int k = 1; k <= FIELD_BOUNDARY; ++k)
						for (int pi = 0; pi <= k; ++pi)
						{
							int pj = FIELD_BOUNDARY - pi;
							int si = i + fi * pi;
							int sj = j + fj * pj;
							if (si >= 0 && si < cols && sj >= 0 && sj < rows)
								if (globalMap[i][j] != globalMap[si][sj] && globalMap[i][j] < playerSize && globalMap[si][sj] < playerSize)
									if (diplomacy[globalMap[i][j]][globalMap[si][sj]] == Undiscovered)
										diplomacy[globalMap[i][j]][globalMap[si][sj]] = Neutral;
						}
}

}
