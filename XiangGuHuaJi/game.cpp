/* XiangGuHuaJi 2016, game.cpp
 * 
 */

#include "game.h"

namespace XGHJ 
{

inline float x2plusy2(float x, float y){return x*x+y*y;}


Game::Game(Map& map, vector<vector<float> > militaryKernel,int playersize)
	: map(map), playerSize(playersize), playerSaving(playersize, INITIAL_PLAYER_MONEY),
      round(0), isValid(true), isPlayerAlive(playersize), playerIncome(playersize, 0), 
      MilitaryKernel(militaryKernel)
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
        isPlayerAlive[i] = true;
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

    printVecMat<float>(militaryKernel, "MilitaryKernel");
	const float pi = 3.1416f;
}

Game::~Game()
{
    
}

// Round<0>
bool Game::Start(vector<TMoney> bidPrice, vector<TPosition> posChoosed)
{
	round = 0;
	++round;

    for (TId i=0; i<playerSize; i++)
    {
        playerArea[i] = 1;
        TPosition capital = posChoosed[i];
        if (canSetGlobalMapPos(capital, i))
        {
            globalMap[capital.x][capital.y] = i;
            playerCapital[i] = capital;
        }
        else
        {
            playerCapital[i] = invalidPos;
            //那就直接干死吧 滑稽咯
            isPlayerAlive[i] = false;
        }
        playerSaving[i] = INITIAL_PLAYER_MONEY - bidPrice[i];
    }
    DiscoverCountry();
	return true;
}

//Game Logic
bool Game::Run(vector<vector<TMilitaryCommand> > & MilitaryCommandMap,
	vector<vector<TDiplomaticCommand> > & DiplomaticCommandMap, 
	vector<TPosition > &NewCapitalList)
{
    DiplomacyPhase(DiplomaticCommandMap);
    MilitaryPhase(MilitaryCommandMap, NewCapitalList);
    ProducingPhase();

    ++round;
    if (CheckWinner()) 
        isValid=false;

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

TMap Game::inf(TMap pos)
{
	if(pos >= MILITARY_KERNEL_SIZE - 1)
		return (pos + 1 - MILITARY_KERNEL_SIZE);
	else
		return 0;
}

TMap Game::sup(TMap pos, TMap max)
{
	if(pos + MILITARY_KERNEL_SIZE <= max)
		return (pos + MILITARY_KERNEL_SIZE);
	else
		return max;
}

bool Game::MilitaryPhase(vector<vector<TMilitaryCommand> > & MilitaryCommandList, vector<TPosition > &NewCapitalList)
{
	//读取MilitaryCommandList并且扣UNIT_BOMB_COST的钱
	TMoney bombSumCost = 0;

	//静态数组，用于构造bfs的栈
	static TPosition bfs_queue[1000000];
	int head = 0, tail = 0;
	static TPosition list[1000];
	int list_length = 0;

	//basemap类对象，用于调用函数

	//存储钱是否够的数组
	vector<TMask> moneyEnough(playerSize);

	//防御力的数组
	vector<vector<float> > defPower(cols);
	for(TMap i = 0; i < cols; ++i)
	{
		defPower[i].resize(rows, 0);
	}

	//攻击力的数组
	vector<vector<vector<float> > > atkPower(playerSize);
	for(TMilitary i = 0; i <playerSize; ++i)
	{
		atkPower[i].resize(cols);
		for(TMilitary j = 0; j < cols; ++j)
		{
			atkPower[i][j].resize(rows, 0);
		}
	}

	//本局战争结果地图
	vector<vector<TId>> tmpGlobalMap(cols);
	for(TId i = 0; i < cols; ++i)
		tmpGlobalMap[i].resize(rows, UNKNOWN_PLAYER_ID);

	vector<vector<TMask>> changeMap(cols);
	for(TMap i = 0; i < cols; ++i)
		changeMap[i].resize(rows, false);

	//判断连通性用到的
	vector<vector<TMask>> newIsSieged(cols);
	for(int i = 0; i < cols; ++i)
		newIsSieged[i].resize(rows, true);

	//开始执行命令队列的内容
	for(TMilitary i = 0; i < playerSize;++i)
	{
		//判断钱是否够用
		for(TMilitary j = 0; j < MilitaryCommandList[i].size(); ++j)
		{
			bombSumCost += MilitaryCommandList[i][j].bomb_size*UNIT_BOMB_COST;
		}
		if(bombSumCost > playerSaving[i])
			moneyEnough[i] = false;
		else
			moneyEnough[i] = true;
		bombSumCost = 0;
		//攻击力和防御力
		for(TMilitary j = 0; j <= MilitaryCommandList[i].size(); ++j)
		{
			//单独计算首都
			if(j == MilitaryCommandList[i].size())
			{
				if(isPosValid(playerCapital[i]))
				{
					//下面两个是首都的位置坐标，以及首都处地形的攻击力
					TMap capXPos, capYPos;
					capXPos = playerCapital[i].x;
					capYPos = playerCapital[i].y;
					TMilitary atk = map.getMapAtk()[capXPos][capYPos];
					for(TMap k = inf(capXPos), m = (MILITARY_KERNEL_SIZE - 1)-(capXPos - k); k < sup(capXPos, cols); ++k, ++m)
					{
						for(TMap l = inf(capYPos), n = (MILITARY_KERNEL_SIZE - 1)-(capYPos - l); l < sup(capYPos, rows); ++l, ++n)
						{
							//这两个for循环用来计算首都出提供的攻击力和防御力，两个for循环的范围为高斯核矩阵的大小
							//这里分为三种情况，对中立地区施加攻，对同盟施加防御力和对敌对势力施加攻击力
							if(globalMap[k][l] == NEUTRAL_PLAYER_ID)
								atkPower[i][k][l] += playerIncome[i]*CAPITAL_INFLUENCE*MilitaryKernel[m][n]*atk;
							else if((diplomacy[i][globalMap[k][l]] == Allied) && (globalMap[k][l] != i || !isSieged[k][l]))
								defPower[k][l] += playerIncome[i]*CAPITAL_INFLUENCE*MilitaryKernel[m][n]*(map.getMapDef()[k][l]);
							else if(diplomacy[i][globalMap[k][l]] == AtWar)
								atkPower[i][k][l] += playerIncome[i]*CAPITAL_INFLUENCE*MilitaryKernel[m][n]*atk;
						}	
					}
				}
			}
			else
			{
				//这是对于炸弹放置点的计算，和上面完全相同
				TMap xPos, yPos;
				xPos = MilitaryCommandList[i][j].place.x;
				yPos = MilitaryCommandList[i][j].place.y;
				//加了一条放置地点合法性的判断
				if(xPos<0||xPos>=cols||yPos<0||yPos>=rows)
					continue;
				TMilitary atk = map.getMapAtk()[xPos][yPos];
				for(TMap k = inf(xPos), m = (MILITARY_KERNEL_SIZE - 1)-(xPos - k); k < sup(xPos, cols); ++k, ++m)
				{
					for(TMap l = inf(yPos), n = (MILITARY_KERNEL_SIZE - 1)-(yPos - l); l < sup(yPos, rows); ++l, ++n)
					{
		
						if(globalMap[k][l] == NEUTRAL_PLAYER_ID)
							atkPower[i][k][l] += MilitaryCommandList[i][j].bomb_size*MilitaryKernel[m][n]*atk;
						else if((diplomacy[i][globalMap[k][l]] == Allied) && (globalMap[k][l] != i || !isSieged[k][l]))
							defPower[k][l] += MilitaryCommandList[i][j].bomb_size*MilitaryKernel[m][n]*(map.getMapDef()[k][l]);
						else if(diplomacy[i][globalMap[k][l]] == AtWar)
							atkPower[i][k][l] += MilitaryCommandList[i][j].bomb_size*MilitaryKernel[m][n]*atk;
					}
				}
			}
		}
	}
	//计算战争结果
	for(TMap i = 0; i <cols; ++i)
		for(TMap j = 0; j < rows; ++j)
		{
			//关于坐标的计算，在每一个坐标出保存一个最大攻击力，和达到最大攻击力的玩家的个数，以及最大攻击力玩家的ID
			float maxAtk = 0;
			TMilitary equalCount = 0;
			TId maxAtkId = UNKNOWN_PLAYER_ID;
			for(TMilitary k = 0; k < playerSize; ++k)
			{
				//由于攻击力每个玩家单独计算，所以进行一个k次循环
				//分两种情况，当前玩家攻击力大于当前该点最大攻击力和等于该点当前最大攻击力，小于的情况不处理
				if(atkPower[k][i][j] > maxAtk)
				{
					maxAtk = atkPower[k][i][j];
					equalCount = 1;
					maxAtkId = k;
				}
				else if(atkPower[k][i][j] == maxAtk)
					equalCount += 1;
			}
			//下面处理最大攻击力减去防御力大于阈值的情况，
			//如果达到最大攻击力的玩家只有一个，则在该局改变所属地图（tmpGlobalMap）中记录
			//若超过一个，打成中立
			if(maxAtk > defPower[i][j] + SUPPESS_LIMIT)
				if(equalCount == 1)
				{
					tmpGlobalMap[i][j] = maxAtkId;
					changeMap[i][j] = true;
				}
				else
				{
					tmpGlobalMap[i][j] = NEUTRAL_PLAYER_ID;
					changeMap[i][j] = true;
				}
		}
	//计算连通性,并更新GlobalMap
	for(TMap i = 0; i <cols; ++i)
		for(TMap j = 0; j < rows; ++j)
		{
			//首先判断这个点在这回合有没有发生变化，如果没有，这个地方不处理
			if(changeMap[i][j])
			{
				//当该点所属发生变化后，则先判断是否中立，如果中立，改为中立结束这次循环
				TMask connection = false;
				TPosition curPos = {i, j};
				if(tmpGlobalMap[i][j] == NEUTRAL_PLAYER_ID)
				{
					globalMap[i][j] = NEUTRAL_PLAYER_ID;
					changeMap[i][j] = false;
				}
				//否则，判断变化部分的连通性，连通性用bfs判断
				//策略是判断当前坐标周围的四个坐标放入队列，然后从对列头开始判断
				//如果队列头所对应坐标的playerId和tmpGlobalMap中正在判断的这点，则判断联通，结束这轮循环
				//否则判断队列头在tmpGlobalMap中的id是否和正在判断的这点相同，如果是，将队列头周围的四个点放入队列，判断队列中下一点
				//否则，判断队列中下一点
				//为了防止重复判断，除了最开始加入的四个点外，所有进入队列的点changeMap变量会被置为false。
				else//bfs,按照x-1，x+1, y-1, y+1顺序如队列
				{
					//先添加进去周围的四个点
					list[list_length++] = curPos;
					changeMap[i][j] = false;
					if(i > 0)
					{
						bfs_queue[tail].x = i - 1;
						bfs_queue[tail].y = j;
						tail++;
					}
					if(i + 1 < cols)
					{
						bfs_queue[tail].x = i + 1;
						bfs_queue[tail].y = j;
						tail++;
					}
					if(j > 0)
					{
						bfs_queue[tail].x = i;
						bfs_queue[tail].y = j - 1;
						tail++;
					}
					if(j + 1 < rows)
					{
						bfs_queue[tail].x = i;
						bfs_queue[tail].y = j + 1;
						tail++;
					}
					while(head != tail)//bfs终止条件，当队列空了以后停止
					{
						TMap m = bfs_queue[head].x;
						TMap n = bfs_queue[head].y;
						//判断是GlobalMap中是否为中立id
						if(globalMap[m][n] != NEUTRAL_PLAYER_ID)
						{
							if(diplomacy[globalMap[m][n]][tmpGlobalMap[i][j]] == Allied)
							{
								head = tail = 0;
								connection = true;
								break;
							}
						}
						else if(tmpGlobalMap[m][n] != UNKNOWN_PLAYER_ID)
						{
							if(tmpGlobalMap[m][n]!=NEUTRAL_PLAYER_ID && diplomacy[tmpGlobalMap[i][j]][tmpGlobalMap[m][n]] == Allied)
							{
								curPos.x = m;
								curPos.y = n;
								list[list_length++] = curPos;
								changeMap[m][n] = false;
								if(m > 0 && (changeMap[m - 1][n]||globalMap[m - 1][n] != NEUTRAL_PLAYER_ID))
								{
									bfs_queue[tail].x = m - 1;
									bfs_queue[tail].y = n;
									tail++;
								}
								if(m + 1< cols && (changeMap[m + 1][n]||globalMap[m + 1][n] != NEUTRAL_PLAYER_ID))
								{
									bfs_queue[tail].x = m + 1;
									bfs_queue[tail].y = n;
									tail++;
								}
								if(n > 0 && (changeMap[m][n - 1]||globalMap[m][n - 1] != NEUTRAL_PLAYER_ID))
								{
									bfs_queue[tail].x = m;
									bfs_queue[tail].y = n - 1;
									tail++;
								}
								if(n + 1< rows && (changeMap[m][n + 1]||globalMap[m][n + 1] != NEUTRAL_PLAYER_ID))
								{
									bfs_queue[tail].x = m;
									bfs_queue[tail].y = n + 1;
									tail++;
								}
							}
						}
						head++;
					}
				}
				//将检测到的点加上
				if(connection){
					for(TMap k = 0; k < list_length; ++k)
						globalMap[list[k].x][list[k].y] = tmpGlobalMap[i][j];
				}
				list_length = 0;
			}
		}

	//更新首都
	for(TMap i = 0; i < playerSize; ++i)
	{
		TPosition tmpPos = NewCapitalList[i];
		if(tmpPos.x >= cols ||tmpPos.y >= rows)
			playerCapital[i] = invalidPos;
		else if(diplomacy[i][globalMap[tmpPos.x][tmpPos.y]] == Allied)
			playerCapital[i] = tmpPos;
		else
			playerCapital[i] = invalidPos;
	}

	//检测包围
	for(TMap i = 0; i < playerCapital.size(); ++i)
	{
		if(isPosValid(playerCapital[i]))
		{
			TMap xPos = playerCapital[i].x, yPos = playerCapital[i].y;
			newIsSieged[xPos][yPos] = false;
			//再做一次bfs，利用静态的bfs数组
			head = tail = 0;
			if(xPos > 0 && newIsSieged[xPos - 1][yPos])
			{
				bfs_queue[tail].x = xPos - 1;
				bfs_queue[tail].y = yPos;
				tail++;
			}
			if(xPos + 1 < cols && newIsSieged[xPos + 1][yPos])
			{
				bfs_queue[tail].x = xPos + 1;
				bfs_queue[tail].y = yPos;
				tail++;
			}
			if(yPos > 0 && newIsSieged[xPos][yPos - 1])
			{
				bfs_queue[tail].x = xPos;
				bfs_queue[tail].y = yPos - 1;
				tail++;
			}
			if(yPos + 1 < rows && newIsSieged[xPos][yPos + 1])
			{
				bfs_queue[tail].x = xPos;
				bfs_queue[tail].y = yPos + 1;
				tail++;
			}
			while(head != tail)
			{
				TMap x_pos = bfs_queue[head].x, y_pos = bfs_queue[head].y;
				if(globalMap[x_pos][y_pos] != NEUTRAL_PLAYER_ID)
				{
					if(diplomacy[i][globalMap[x_pos][y_pos]])
					{
						newIsSieged[x_pos][y_pos] = false;
						if(x_pos > 0 && newIsSieged[x_pos - 1][y_pos])
						{
							bfs_queue[tail].x = x_pos - 1;
							bfs_queue[tail].y = y_pos;
							tail++;
						}
						if(x_pos + 1< cols && newIsSieged[x_pos + 1][y_pos])
						{
							bfs_queue[tail].x = x_pos + 1;
							bfs_queue[tail].y = y_pos;
							tail++;
						}
						if(y_pos > 0 && newIsSieged[x_pos][y_pos - 1])
						{
							bfs_queue[tail].x = x_pos;
							bfs_queue[tail].y = y_pos - 1;
							tail++;
						}
						if(y_pos + 1 < rows && newIsSieged[x_pos][y_pos + 1])
						{
							bfs_queue[tail].x = x_pos;
							bfs_queue[tail].y = y_pos + 1;
							tail++;
						}
					}
				}
				head++;
			}
		}
	}
	//上面一部分是把所有的包括同盟的连通全部算上了，下面单独减去首都不合法的
	for(TMap i = 0; i < cols; ++i)
		for(TMap j = 0; j < rows; ++j)
		{
			if(globalMap[i][j] == NEUTRAL_PLAYER_ID)
				isSieged[i][j] = false;
			else if(isPosValid(playerCapital[globalMap[i][j]]))
				isSieged[i][j] = newIsSieged[i][j];
			else
				isSieged[i][j] = false;
		}
    return false; //TODO
}

bool Game::isPlayer(TId id)
{
    return id >= 0 && id < playerSize;
}

//Producing Phase (Deal with MapResource, PlayerInfoList)
bool Game::ProducingPhase()
{
    //initialize
    for (TId id=0; id<playerSize; ++id)
    {
        playerArea[id] = 0;
        // lowest income
        playerIncome[id] = 1;
        // corruption 
        playerSaving[id] = (TMoney)((1-(float)(playerArea[id])*CORRUPTION_COEF) * (float) playerSaving[id]);
    }
    // map income 
    for (TMap i=0; i<cols; i++)
    {
        for (TMap j=0; j<rows; j++)
        {
            if (isPlayer(globalMap[i][j])) 
            {
                playerIncome[globalMap[i][j]] +=map.getMapRes()[i][j];
                playerArea[globalMap[i][j]]++;
            }
        }
    }

    for (TId id=0; id<playerSize; ++id)
	{
        // city income
		if (isPosValid(playerCapital[id]))
		{
			playerSaving[id] += (TMoney)(UNIT_CITY_INCOME * (float)round);
		}
        // refresh
		playerSaving[id] += playerIncome[id];
    }
	DiscoverCountry();

    return false; //TODO
}

//Check the winner and the loser (Deal with PlayerInfoList)
bool Game::CheckWinner()
{
	if (round == MAX_ROUND)
	{
        return true;
	}
    else
    {
        TId aliveCnt=0;
        for (TId id=0; id<playerSize; id++)
        {
            if (isPlayerAlive[id])
            {
                if (playerArea[id] == 0 && !isPosValid(playerCapital[id]))
                {
                    //welcome death
                    isPlayerAlive[id] = false;
                    for (TId playerid=0; playerid<playerSize; playerid++)
                    {
                        diplomacy[id][playerid] = Undiscovered;
                        diplomacy[playerid][id] = Undiscovered;
                    }
                    diplomacy[id][id] = Allied;
                }
                else
                {
                    aliveCnt++;
                }
            }
        }
        if (aliveCnt == 1)
        {
            return true;
        }
        return false;
    }
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
					int pj = k - pi;
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
	info.DiplomaticCommandList = vector<TDiplomaticCommand>(playerSize);
	info.MilitaryCommandList = vector<TMilitaryCommand>();
	info.id = playerid;
	info.playerSize = playerSize;
	info.round = round;
	info.newCapital = playerCapital[playerid];
	info.map = &map;
	info.rows = rows;
	info.cols = cols;
	info.playerInfo = vector<PlayerInfo>(playerSize);
	for (TId id=0; id<playerSize; id++)
	{
		info.playerInfo[id] = getPlayerInfo(id, playerid);
        info.DiplomaticCommandList[id] = getDefaultCommand(diplomacy[playerid][id]);
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
							int pj = k - pi;
							int si = i + fi * pi;
							int sj = j + fj * pj;
							if (si >= 0 && si < cols && sj >= 0 && sj < rows)
								if (globalMap[i][j] != globalMap[si][sj] && globalMap[i][j] < playerSize && globalMap[si][sj] < playerSize)
									if (diplomacy[globalMap[i][j]][globalMap[si][sj]] == Undiscovered)
										diplomacy[globalMap[i][j]][globalMap[si][sj]] = Neutral;
						}
}

TDiplomaticCommand Game::getDefaultCommand(TDiplomaticStatus ds) const
{
    switch (ds)
    {
    case Undiscovered:
        return KeepNeutral;
        break;
    case Neutral:
        return KeepNeutral;
        break;
    case Allied:
        return FormAlliance;
        break;
    case AtWar:
        return JustifyWar;
        break;
    }
    return KeepNeutral;
}

bool Game::canSetGlobalMapPos(TPosition pos, TId id)
{
    //if (pos.x<0) return false;
    if (pos.x>cols-1) return false;
    //if (pos.y<0) return false;
    if (pos.y>rows-1) return false;
    if (globalMap[pos.x][pos.y] != NEUTRAL_PLAYER_ID)
    {
        return false;
    }
    return true;
}

}
