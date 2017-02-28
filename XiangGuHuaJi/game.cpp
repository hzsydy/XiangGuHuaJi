/* XiangGuHuaJi 2016, game.cpp
 * 
 */

#include "game.h"

#include <queue>

namespace XGHJ 
{

using namespace std;

inline float x2plusy2(float x, float y){return x*x+y*y;}
inline int absDist(TPosition p1, TPosition p2){return abs((int)p1.x-(int)p2.x)+abs((int)p1.y-(int)p2.y);}

Game::Game(Map& map, vector<vector<float> > militaryKernel,int playersize)
	: map(map), playerSize(playersize)
    , playerSaving(playersize, INITIAL_PLAYER_MONEY)
    , round(0), _isValid(true)
    , isPlayerAlive(playersize, true)
    , playerIncome(playersize, 0)
    , MilitaryKernel(militaryKernel)
    , aliveCnt(playersize)
    , rows(map.getRows()), cols(map.getCols())
    , globalMap(map.getCols(), vector<TId>(map.getRows(), NEUTRAL_PLAYER_ID))
    , isSieged(map.getCols(), vector<bool>(map.getRows(), false))
    , playerCapital(playersize, INVALID_POSITION)
    , playerArea(playersize, 0)
    , diplomacy(playersize, vector<TDiplomaticStatus>(playersize, Undiscovered))
    , roundToJusifyWar(playersize, vector<int>(playersize, 0))
    , backstabUsed(playersize, false)
    , player_ranking(playersize)
{
    
    for (TId id=0; id<playersize; ++id) diplomacy[id][id] = Allied;
    for (int i = 0; i < playerSize; ++i) player_ranking[i] = i;

    //printVecMat<float>(militaryKernel, "MilitaryKernel");
}

Game::~Game()
{
    
}

// Round<0>
bool Game::Start(vector<TMoney> bidPrice, vector<TPosition> posChoosed)
{

    for (TId i=0; i<playerSize; i++) {        
        TPosition& capital = posChoosed[i];
        
        // take bid price
        playerArea[i] = 1;
        playerSaving[i] = INITIAL_PLAYER_MONEY - bidPrice[i];

        // check birth place
        if (canSetGlobalMapPos(capital, i)) {
            globalMap[capital.x][capital.y] = i;
            playerCapital[i] = capital;
        }
        else {
            // invalid birth place
            playerCapital[i] = INVALID_POSITION;
            isPlayerAlive[i] = false; 
        }
        
    }
    DiscoverCountry();

    round = 1;

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
    UpdateMapChecksum();

    ++round;
    if (CheckWinner()) _isValid=false;

    return _isValid;    
}

void Game::StartWar(TId a, TId b) {
    if (a==b || !isPlayer(a) || !isPlayer(b)) return;
    roundToJusifyWar[a][b] = roundToJusifyWar[b][a] = 0;
    diplomacy[a][b] = diplomacy[b][a] = AtWar;
}

//Diplomacy Phase (Deal with DiplomaticCommandMap)
bool Game::DiplomacyPhase(vector<vector<TDiplomaticCommand> > & DiplomaticCommandMap)
{
	for (TId i = 0; i < playerSize-1; ++i)
		for (TId j = i+1; j < playerSize; ++j)
			if (i != j && diplomacy[i][j] != Undiscovered)
			{
                // check JustifyWar
				if (DiplomaticCommandMap[i][j] == JustifyWar) 
					if (playerSaving[i] - WAR_JUSTIFY_PRICE >= 0) playerSaving[i] -= WAR_JUSTIFY_PRICE; else DiplomaticCommandMap[i][j] = KeepNeutral;
				if (DiplomaticCommandMap[j][i] == JustifyWar) 
					if (playerSaving[j] - WAR_JUSTIFY_PRICE >= 0) playerSaving[j] -= WAR_JUSTIFY_PRICE; else DiplomaticCommandMap[j][i] = KeepNeutral;

                // check FormAlliance
                if (DiplomaticCommandMap[i][j] == FormAlliance) {
                    int m = (int)(UNIT_AREA_ALLY_COST*playerIncome[j]);
                    if (playerSaving[i] >= m) playerSaving[i] -= m; else DiplomaticCommandMap[i][j] = KeepNeutral;
                }
                if (DiplomaticCommandMap[j][i] == FormAlliance) {
                    int m = (int)(UNIT_AREA_ALLY_COST*playerIncome[i]);
                    if (playerSaving[j] >= m) playerSaving[j] -= m; else DiplomaticCommandMap[j][i] = KeepNeutral;
                }

                // check Backstab
				if (DiplomaticCommandMap[i][j] == Backstab) 
					if (backstabUsed[i] == false) backstabUsed[i] = true; else DiplomaticCommandMap[i][j] = KeepNeutral;
				if (DiplomaticCommandMap[j][i] == Backstab)
					if (backstabUsed[j] == false) backstabUsed[j] = true; else DiplomaticCommandMap[j][i] = KeepNeutral;
				

                // now check all 
				switch (DiplomaticCommandMap[i][j])
				{
				case KeepNeutral:
					roundToJusifyWar[i][j] = 0;
					switch (DiplomaticCommandMap[j][i])
					{
					case KeepNeutral: case FormAlliance:
						roundToJusifyWar[j][i] = 0;	diplomacy[i][j] = diplomacy[j][i] = Neutral; break;
					case JustifyWar:
						if (diplomacy[i][j] != AtWar) {
							++roundToJusifyWar[j][i];
							if (roundToJusifyWar[j][i] >= WAR_JUSTIFY_TIME) StartWar(i, j);
							else diplomacy[i][j] = diplomacy[j][i] = Neutral;
						}
						break;
					case Backstab:
						StartWar(i, j);
					}
					break;
				case FormAlliance:
					roundToJusifyWar[i][j] = 0;
					switch (DiplomaticCommandMap[j][i])
					{
					case KeepNeutral:
						roundToJusifyWar[j][i] = 0; diplomacy[i][j] = diplomacy[j][i] = Neutral; break;
					case FormAlliance:
						roundToJusifyWar[j][i] = 0; diplomacy[i][j] = diplomacy[j][i] = Allied; break;
					case JustifyWar:
						if (diplomacy[i][j] != AtWar) {
							++roundToJusifyWar[j][i];
							if (roundToJusifyWar[j][i] >= WAR_JUSTIFY_TIME) StartWar(i, j);
							else diplomacy[i][j] = diplomacy[j][i] = Neutral;
						}
						break;
					case Backstab:
						StartWar(i, j); break;
					}
					break;
				case JustifyWar:
					if (diplomacy[i][j] != AtWar) 
                    {
                        ++roundToJusifyWar[i][j];
						if (roundToJusifyWar[i][j]>= WAR_JUSTIFY_TIME) StartWar(i, j);
                        else {
						    switch (DiplomaticCommandMap[j][i]) 
                            {
						    case KeepNeutral: case FormAlliance:
							    roundToJusifyWar[j][i] = 0; diplomacy[i][j] = diplomacy[j][i] = Neutral; break;
						    case JustifyWar:
							    ++roundToJusifyWar[j][i];
							    if (roundToJusifyWar[j][i]>= WAR_JUSTIFY_TIME) StartWar(i, j);
							    else diplomacy[i][j] = diplomacy[j][i] = Neutral;
							    break;
						    case Backstab:
							    StartWar(i, j); break;
						    }
					    }		
                    }
					break;
				case Backstab:
					StartWar(i, j); break;
				}
			}
    return true; //TODO
}

vector<TId> Game::getWarList(TId id) const
{
    vector<TId> wl;
    for (TId i = 0; i < playerSize; ++i) {
        if (id != i && diplomacy[id][i] == AtWar)
        wl.push_back(i);
    }
    return wl;

}

bool Game::MilitaryPhase(vector<vector<TMilitaryCommand> > & MilitaryCommandList, vector<TPosition > &NewCapitalList) 
{
    // 地图
    vector<vector<TMilitary> > MapAtk = map.getMapAtk();
    vector<vector<TMilitary> > MapDef = map.getMapDef();

    // 影响力图
    vector<vector<vector<TMilitary> > > player_influence_map(playerSize,
        vector<vector<TMilitary> >(cols, 
        vector<TMilitary>(rows, 0)));
    // 防御力图
    vector<vector<TMilitary> > map_defense(cols,
        vector<TMilitary>(rows, 0));
    // 击破图
    vector<vector<TId> > map_new_owner(cols, 
        vector<TId>(rows, UNKNOWN_PLAYER_ID));

    // 地图连通性处理用常数
    const char NON_CONTROL = 0;
    const char UNDER_CONTROL = 1;
    const char NEW_POSITION = 2;
    // 四连通方向
    const int dx[] = { 0,  1,  0, -1};
    const int dy[] = {-1,  0,  1,  0};


    // 检查MilitaryCommandList战争指令是否可行;  并扣除战争所花的钱; 最后生成此玩家的影响力图;
    for (TId id = 0; id<playerSize; ++id) {
        vector<TMilitaryCommand>& tmc_list = MilitaryCommandList[id];
        size_t i = 0;
        int valid_comm_cnt = 0;

        // 检查军事指令
        for (i=0; i<tmc_list.size(); ++i) {
            TMilitaryCommand& tmc = tmc_list[i];
            TId t_owner;

            // 检查位置
            if (tmc.place.x >= cols || tmc.place.y >= rows) { tmc.bomb_size = 0; continue; }
            t_owner = globalMap[tmc.place.x][tmc.place.y];
            if (t_owner == NEUTRAL_PLAYER_ID || diplomacy[id][t_owner] != Allied) { tmc.bomb_size = 0; continue; }

            // 检查金额
            if (tmc.bomb_size <0) { tmc.bomb_size = 0; continue; }
            if (tmc.bomb_size > playerSaving[id]) { 
                // out of cash
                tmc.bomb_size = playerSaving[id]; 
                playerSaving[id] = 0; break;
            }
            playerSaving[id] -= tmc.bomb_size;
            
            // 不能同时放置超过MILITARY_COUNT_LIMIT个活动
            ++valid_comm_cnt;
            if (valid_comm_cnt>MILITARY_COUNT_LIMIT) break;
        }
        // 清除剩余的指令
        ++i;
        if (i<tmc_list.size()) 
            for (; i<tmc_list.size(); ++i) tmc_list[i].bomb_size = 0;

        // 添加无消耗的首都自卫指令
        if (playerCapital[id].x < cols && playerCapital[id].y < rows) {
            TMilitaryCommand tmc;
            tmc.place = playerCapital[id];
            tmc.bomb_size = playerIncome[id] * CAPITAL_INFLUENCE;
            tmc_list.push_back(tmc);
        }

        // 生成PlayerSize张影响力图
        vector<vector<TMilitary> >& map = player_influence_map[id];
        for (int i=0; i<tmc_list.size(); ++i) {
            TMilitaryCommand& tmc = tmc_list[i];
            int x = 0, y = 0;
            if (tmc.bomb_size<=0) continue;
            for (int n=0; n<2*MILITARY_KERNEL_SIZE-1; ++n)
                for (int m=0; m<2*MILITARY_KERNEL_SIZE-1; ++m) {
                    x = tmc.place.x + n - MILITARY_KERNEL_SIZE + 1; if (x<0) continue; if (x>=cols) continue;
                    y = tmc.place.y + m - MILITARY_KERNEL_SIZE + 1; if (y<0) continue; if (y>=rows) continue;
                    map[x][y] += MilitaryKernel[n][m] * tmc.bomb_size;
                }
        }       
    }

    // (参考断补性) 生成1张防御力图;  1张击破图
    for (TMap x=0; x<cols; ++x)
        for (TMap y = 0; y < rows; ++y) {
            // 领主
            TId owner = globalMap[x][y];
            // 攻击门限
            TMilitary attack_threshold;
            // 最强攻击
            TMilitary best_attack = 0;
            TId best_owner = UNKNOWN_PLAYER_ID;
            // 次强攻击
            TMilitary second_attack = 0;
            // 攻击系数
            TMilitary atk_ratio = MapAtk[x][y];

            // 计算 非断补 且 有主 领土的守备力
            if (!isSieged[x][y] && isPlayer(owner)) {
                // 累加同盟影响力
                for (TId id = 0; id < playerSize; ++id)
                    if (diplomacy[id][owner] == Allied)
                        map_defense[x][y] += player_influence_map[id][x][y];
                // 防守系数
                map_defense[x][y] *= MapDef[x][y];
            }
            else {
                // 断补和无主地的守备力为 0
                map_defense[x][y] = 0;
            }

            // 逐个检查攻击方，得到最强的2个攻击者
            for (TId id = 0; id < playerSize; ++id)
                if (owner == NEUTRAL_PLAYER_ID || diplomacy[owner][id] == AtWar) {
                    TMilitary attack = player_influence_map[id][x][y] * atk_ratio;
                    if (attack > best_attack) {
                        second_attack = best_attack;
                        best_attack = attack;
                        best_owner = id;
                    }
                    else if (attack > second_attack) {
                        second_attack = attack;
                    }
                }
            // 击破判定
            attack_threshold = map_defense[x][y] + SUPPESS_LIMIT;
            if (best_attack >= attack_threshold) {
                if (best_attack > second_attack)
                    map_new_owner[x][y] = best_owner; // 攻击优势者攻占
                else
                    map_new_owner[x][y] = NEUTRAL_PLAYER_ID; // 无主之地
            }
            else map_new_owner[x][y] = UNKNOWN_PLAYER_ID; // 保持不变
        }

    // 检查击破图的连通性
    for (TId id=0; id<playerSize; ++id) {
        // 连通性判定用地图
        vector<vector<char> > map(cols,
            vector<char>(rows, NON_CONTROL));
        // bfs用的队列
        queue<TPosition> q;
        
        // 初始化地图
        for (TMap x=0; x<cols; ++x)
            for (TMap y=0; y<rows; ++y) {
                TId owner = globalMap[x][y];
                if (owner<playerSize && diplomacy[id][owner] == Allied) {
                    TPosition pp = {x,y};
                    map[x][y] = UNDER_CONTROL;
                    q.push(pp);
                }
                else if (map_new_owner[x][y] == id) {
                    map[x][y] = NEW_POSITION;
                }
            }
        
        // 开始bfs进行连通性判定
        while(!q.empty()) {
            TPosition p = q.front();
            q.pop();
            for (size_t i=0; i<4; ++i) {
                int x = p.x + dx[i]; if (x<0 || x>=cols) continue;
                int y = p.y + dy[i]; if (y<0 || y>=rows) continue;
                if (map[x][y] == NEW_POSITION) {
                    TPosition pp = {x,y};
                    map[x][y] = UNDER_CONTROL;
                    q.push(pp);
                }
            }
        }

        // 确认连通性
        for (TMap x=0; x<cols; ++x)
            for (TMap y=0; y<rows; ++y) 
                if (map_new_owner[x][y] == id && map[x][y] != UNDER_CONTROL) 
                    map_new_owner[x][y] = NEUTRAL_PLAYER_ID; // 火力足够大但是于己方领土不连通，同样是炸成中立
    }

    // 更新世界地图
    for (TMap x=0; x<cols; ++x)
        for (TMap y=0; y<rows; ++y) 
            if (map_new_owner[x][y] != UNKNOWN_PLAYER_ID) 
                globalMap[x][y] = map_new_owner[x][y];

    // 强行去除无敌土地上的领土        
    for (TMap x=0; x<cols; ++x)
        for (TMap y=0; y<rows; ++y)
            if (MapAtk[x][y]<=0)
                globalMap[x][y] = NEUTRAL_PLAYER_ID;

    // 更新首都
    for (TId id = 0; id<playerSize; ++id) {
        TPosition p = NewCapitalList[id];

        // 自动修正违规的首都指令
        if (!(p.x<cols && p.y<rows)) p = playerCapital[id];

        // 位置有效性检查
        if (p.x<cols && p.y<rows) { 
            int owner = globalMap[p.x][p.y];
            if (!isPlayer(owner) || diplomacy[id][owner] != Allied) p = INVALID_POSITION;
        } 
        else p = INVALID_POSITION;

        // 更新首都
        playerCapital[id] = p;

    }

    // (进行PlayerSize次，针对首都和军通的bfs)更新 断补性 
    // 初始化：全部断补
    for (TMap x=0; x<cols; ++x)
        for (TMap y=0; y<rows; ++y) 
            isSieged[x][y] = globalMap[x][y]<playerSize;
    // 判定
    for (TId id = 0; id<playerSize; ++id) {
        TPosition p = playerCapital[id];
        if (p.x<cols && p.y<rows) {
            vector<vector<char> > map(cols, 
                vector<char>(rows, NON_CONTROL));
            // bfs用的队列
            queue<TPosition> q;

            // 确认军通区域
            for (TMap x=0; x<cols; ++x)
                for (TMap y=0; y<rows; ++y) {
                    TId owner = globalMap[x][y];   
                    if (owner<playerSize && diplomacy[id][owner]==Allied) map[x][y] = NEW_POSITION;
                }
            // 同盟首都
            for (TId uid=0; uid<playerSize; ++uid) 
                if (diplomacy[id][uid]==Allied && playerCapital[uid].x<cols && playerCapital[uid].y<rows) {
                    map[playerCapital[uid].x][playerCapital[uid].y] = UNDER_CONTROL;
                    q.push(playerCapital[uid]);
                }
            // 开始bfs
            while (!q.empty()) {
                TPosition p = q.front();
                q.pop();
                for (size_t i=0; i<4; ++i) {
                    int x = p.x + dx[i]; if (x<0 || x>=cols) continue;
                    int y = p.y + dy[i]; if (y<0 || y>=rows) continue;
                    if (map[x][y] == NEW_POSITION) {
                        TPosition pp = {x,y};
                        map[x][y] = UNDER_CONTROL;
                        q.push(pp);
                    }
                }
            }
            // 确认 断补
            for (TMap x=0; x<cols; ++x)
                for (TMap y=0; y<rows; ++y)
                    if (globalMap[x][y]==id && map[x][y]==UNDER_CONTROL) isSieged[x][y] = false;
        }
        else {
            // 首都不存在时，己方领土全部失守
        }
    }

    return true;
}

bool Game::isPlayer(TId id) const
{
    return id >= 0 && id < playerSize;
}

//Producing Phase (Deal with MapResource, PlayerInfoList)
bool Game::ProducingPhase()
{

    vector<vector<TMoney> > mapRes = map.getMapRes();

    //initialize
    for (TId id=0; id<playerSize; ++id)
        if (isPlayerAlive[id]) {
            playerArea[id] = 0;
            // lowest income 至少有社长一个人
            playerIncome[id] = 1;
        }
    // map income 
    for (TMap i=0; i<cols; i++) {
        for (TMap j=0; j<rows; j++) {
            if (isPlayer(globalMap[i][j])) {
                playerIncome[globalMap[i][j]] += mapRes[i][j];
                playerArea[globalMap[i][j]]++;
            }
        }
    }

    for (TId id=0; id<playerSize; ++id)
	{
        // corruption 
        playerSaving[id] = (TMoney)((1-(float)(playerIncome[id])*CORRUPTION_COEF) * (float) playerSaving[id]);
        // city income
		if (isPosValid(playerCapital[id])) {
			playerSaving[id] += (TMoney)(UNIT_CITY_INCOME * (float)round);
		}
        // refresh
		playerSaving[id] += playerIncome[id];
    }
	DiscoverCountry();

    return false; //TODO
}

void Game::killPlayer(TId id)
{
    if (!isPlayer(id)) return;
    if (!isPlayerAlive[id]) return;

    --aliveCnt;

    playerSaving[id] = 0;
    playerIncome[id] = 0;

    isPlayerAlive[id] = false;
    for (TId playerid=0; playerid<playerSize; playerid++) {
        diplomacy[id][playerid] = Undiscovered;
        diplomacy[playerid][id] = Undiscovered;
    }
    diplomacy[id][id] = Allied;

    for (TMap x=0; x<cols; ++x)
        for (TMap y=0; y<rows; ++y)
            if (globalMap[x][y]==id) globalMap[x][y] = NEUTRAL_PLAYER_ID;

}

//Check the winner and the loser (Deal with PlayerInfoList)
bool Game::CheckWinner()
{

    for (TId id=0; id<playerSize; id++) {
        if (isPlayerAlive[id]) {
            if (playerArea[id] == 0 && !isPosValid(playerCapital[id])) {
                //welcome death
                killPlayer(id);
                player_ranking[aliveCnt] = id;
            }
        }
    }

    // refresh the rank
    for (int i = 0; i < aliveCnt; ++i) player_ranking[i] = UNKNOWN_PLAYER_ID;
    for (TId id = 0; id < playerSize; ++id) 
        if (isPlayerAlive[id]) {
            int r = 0;
            while (isPlayer(player_ranking[r]) && playerIncome[player_ranking[r]] > playerIncome[id]) ++r;
            for (int i = aliveCnt - 1; i > r; --i) player_ranking[i] = player_ranking[i - 1];
            player_ranking[r] = id;
        }
    
    // 删除苟活
    if (round > STRICT_ROUND_START) {
        for (int r=0; r<playerSize; ++r) {
            TId id = player_ranking[r];
            if (playerIncome[id] < round - STRICT_ROUND_START) {
                killPlayer(id);
            }
        }
    }

    if (aliveCnt == 1 || round == MAX_ROUND)
        return true;
    else 
        return false;
}

void Game::UpdateMapChecksum() {

    map_checksum = 47831;
    for (int i=0; i<cols; ++i)
        for (int j=0; j<rows; ++j)
            map_checksum = map_checksum * 17 + globalMap[i][j];
}

PlayerInfo Game::getPlayerInfo(TId id, TId playerId) const
{
	PlayerInfo p;
	p.dipStatus = diplomacy[playerId][id];

    // visible
    p.isVisible = p.dipStatus != Undiscovered;
    p.income = p.isVisible ? playerIncome[id] : 0;
    p.mapArea = p.isVisible ? playerArea[id] : 0;
    if (p.isVisible) p.warList = getWarList(id);
    
    // union
    p.isUnion = p.dipStatus == Allied;
    p.saving = p.isUnion ? playerSaving[id] : 0;
    p.capital = p.isUnion ? playerCapital[id] : INVALID_POSITION;

	return p;
}

TMask Game::isPointVisible(TMap x, TMap y, TId playerId) const
{
    if (!isPlayer(playerId)) return true;

	if (globalMap[x][y] == playerId) return true;
	//if (globalMap[x][y] != NEUTRAL_PLAYER_ID && (diplomacy[playerId][globalMap[x][y]] == Neutral || diplomacy[playerId][globalMap[x][y]] == Allied))
    if (isPlayer(globalMap[x][y]) && (diplomacy[playerId][globalMap[x][y]] == Allied))
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
    mp.isSieged = mp.isVisible ? isSieged[x][y] : false;
    mp.owner = mp.isVisible ? globalMap[x][y] : UNKNOWN_PLAYER_ID;

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
    info.backstabUsed = backstabUsed[playerid];
    info.militaryKernel = MilitaryKernel;
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

vector<vector<MapPointInfo> > Game::getGlobalMap(TId id) const
{
    vector<vector<MapPointInfo> > map(cols,
        vector<MapPointInfo>(rows));
    for (TMap i = 0; i < cols; ++i)
        for (TMap j = 0; j < rows; ++j)
            map[i][j] = getMapPointInfo(i, j, id);
    return map;
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
                                {
                                    for (int qt = 0; qt < playerSize; ++qt)
                                    {
                                        if (qt != globalMap[i][j])
                                        {
                                            if (diplomacy[globalMap[i][j]][qt] == Allied)
                                            {
                                                if (diplomacy[qt][globalMap[si][sj]] == Undiscovered)
                                                    diplomacy[qt][globalMap[si][sj]] = Neutral;
                                                if (diplomacy[globalMap[si][sj]][qt] == Undiscovered)
                                                    diplomacy[globalMap[si][sj]][qt] = Neutral;
                                            }
                                        }

                                    }
                                    if (diplomacy[globalMap[i][j]][globalMap[si][sj]] == Undiscovered)
                                        diplomacy[globalMap[i][j]][globalMap[si][sj]] = Neutral;
                                }
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
    if (pos.x<0) return false;
    if (pos.x>cols-1) return false;
    if (pos.y<0) return false;
    if (pos.y>rows-1) return false;
    if (globalMap[pos.x][pos.y] != NEUTRAL_PLAYER_ID) return false;

    for (TId i=0; i<id; i++) {
        if (absDist(playerCapital[i], pos)<MIN_ABS_DIST_BETWEEN_CAP) return false;
    }
    return true;
}

}