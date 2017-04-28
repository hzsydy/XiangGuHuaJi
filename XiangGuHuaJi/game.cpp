/* XiangGuHuaJi 2016, game.cpp
 * 
 */

#include "game.h"

#include <queue>

namespace XGHJ 
{

using namespace std;

inline int absDist(TPosition p1, TPosition p2){return abs((int)p1.x-(int)p2.x)+abs((int)p1.y-(int)p2.y);}

// 初始化
Game::Game(Map& map, vector<vector<float> > militaryKernel,int playersize)
	: map(map), playerSize(playersize)
    , playerSaving(playersize, INITIAL_PLAYER_MONEY)
    , round(0), _isValid(true)
    , isPlayerAlive(playersize, true)
    , playerIncome(playersize, 1)
    , MilitaryKernel(militaryKernel)
    , aliveCnt(playersize)
    , rows(map.getRows()), cols(map.getCols())
    , globalMap(map.getCols(), vector<TId>(map.getRows(), NEUTRAL_PLAYER_ID))
    , isSieged(map.getCols(), vector<bool>(map.getRows(), false))
    , playerCapital(playersize, INVALID_POSITION)
    , playerArea(playersize, 1)
    , diplomacy(playersize, vector<TDiplomaticStatus>(playersize, Undiscovered))
    , roundToJusifyWar(playersize, vector<int>(playersize, 0))
    , backstabUsed(playersize, false)
    , backstab_enabled(playersize, false)
    , player_ranking(playersize)
    , exciting_game_score(0)
{
    
    for (TId id = 0; id < playersize; ++id) diplomacy[id][id] = Allied;
    for (int i = 0; i < playersize; ++i) player_ranking[i] = i;
}

// 解析函数
Game::~Game()
{
    
}

// 开局 竞价阶段
bool Game::Start(vector<TMoney> bidPrice, vector<TPosition> posChoosed)
{

    for (TId i=0; i<playerSize; i++) {        
        TPosition& capital = posChoosed[i];
        
        // 去除竞价金额
        if (bidPrice[i] < 0) bidPrice[i] = 0;
        if (bidPrice[i] > INITIAL_PLAYER_MONEY) bidPrice[i] = INITIAL_PLAYER_MONEY;
        playerSaving[i] = INITIAL_PLAYER_MONEY - bidPrice[i];

        if (canSetGlobalMapPos(capital, i)) {
            // 设置出生点
            globalMap[capital.x][capital.y] = i;
            playerCapital[i] = capital;
        }
        else {
            // 无效的出生点
            playerCapital[i] = INVALID_POSITION;
            killPlayer(i);
        }
        
    }

    DiscoverCountry();

    round = 1; // 进入第一回合

	return true;
}

// 主逻辑
bool Game::Run(vector<vector<TMilitaryCommand> > & MilitaryCommandMap,
	vector<vector<TDiplomaticCommand> > & DiplomaticCommandMap, 
	vector<TPosition > &NewCapitalList)
{
    ++round;

    DiplomacyPhase(DiplomaticCommandMap);
    MilitaryPhase(MilitaryCommandMap, NewCapitalList);
    ProducingPhase();
    DiscoverCountry();

    if (CheckWinner()) _isValid=false;

    UpdateMapChecksum();

    return _isValid;    
}

// 两个玩家开始战争
void Game::StartWar(TId a, TId b) {
    if (a==b || !isPlayer(a) || !isPlayer(b)) return;
    roundToJusifyWar[a][b] = roundToJusifyWar[b][a] = 0;
    diplomacy[a][b] = diplomacy[b][a] = AtWar;
}

// 外交阶段
bool Game::DiplomacyPhase(vector<vector<TDiplomaticCommand> > & DiplomaticCommandMap)
{
    // clear backstab_enabled
    for (size_t i=0; i<playerSize; ++i) backstab_enabled[i] = false;

	for (TId i = 0; i < playerSize-1; ++i)
		for (TId j = i+1; j < playerSize; ++j)
			if (i != j && diplomacy[i][j] != Undiscovered) {

                // Backstab 费用（一次性）
				if (DiplomaticCommandMap[i][j] == Backstab) 
                    if (backstabUsed[i] == false) { 
                        backstabUsed[i] = true; 
                        backstab_enabled[i] = true;
                    } 
                    else DiplomaticCommandMap[i][j] = JustifyWar;
				if (DiplomaticCommandMap[j][i] == Backstab)
					if (backstabUsed[j] == false) { 
                        backstabUsed[j] = true; 
                        backstab_enabled[j] = true;
                    }
                    else DiplomaticCommandMap[j][i] = JustifyWar;

                // JustifyWar 费用
				if (DiplomaticCommandMap[i][j] == JustifyWar) 
					if (playerSaving[i] - WAR_JUSTIFY_PRICE >= 0) playerSaving[i] -= WAR_JUSTIFY_PRICE; else DiplomaticCommandMap[i][j] = KeepNeutral;
				if (DiplomaticCommandMap[j][i] == JustifyWar) 
					if (playerSaving[j] - WAR_JUSTIFY_PRICE >= 0) playerSaving[j] -= WAR_JUSTIFY_PRICE; else DiplomaticCommandMap[j][i] = KeepNeutral;

                // FormAlliance 费用
                if (DiplomaticCommandMap[i][j] == FormAlliance) {
                    int m = (int)(UNIT_AREA_ALLY_COST*playerIncome[j]);
                    if (playerSaving[i] >= m) playerSaving[i] -= m; else DiplomaticCommandMap[i][j] = KeepNeutral;
                }
                if (DiplomaticCommandMap[j][i] == FormAlliance) {
                    int m = (int)(UNIT_AREA_ALLY_COST*playerIncome[i]);
                    if (playerSaving[j] >= m) playerSaving[j] -= m; else DiplomaticCommandMap[j][i] = KeepNeutral;
                }


				
                // --- --- --- ---

                // 处理 roundToJustifyWar
                if (DiplomaticCommandMap[i][j]!=JustifyWar) roundToJusifyWar[i][j] = 0; else ++roundToJusifyWar[i][j];
                if (DiplomaticCommandMap[j][i]!=JustifyWar) roundToJusifyWar[j][i] = 0; else ++roundToJusifyWar[j][i];
                // 处理 JustifyWar
                if (DiplomaticCommandMap[i][j] == JustifyWar) {
                    if (diplomacy[i][j]!=AtWar && roundToJusifyWar[i][j] >= WAR_JUSTIFY_TIME) { StartWar(i,j); continue; }
                    if (diplomacy[i][j]==AtWar) { StartWar(i,j); continue; }
                }
                if (DiplomaticCommandMap[j][i] == JustifyWar) {
                    if (diplomacy[j][i]!=AtWar && roundToJusifyWar[j][i] >= WAR_JUSTIFY_TIME) { StartWar(j,i); continue; }
                    if (diplomacy[j][i]==AtWar) { StartWar(j,i); continue; }
                }
                // 特判 Backstab
                if (DiplomaticCommandMap[i][j]==Backstab || DiplomaticCommandMap[j][i]==Backstab){ StartWar(i, j); continue; }

                // 特判 FormAlliance
                if (DiplomaticCommandMap[i][j]==FormAlliance && DiplomaticCommandMap[j][i]==FormAlliance) { diplomacy[i][j] = diplomacy[j][i] = Allied; continue; }

                // 正常情况
                diplomacy[i][j] = diplomacy[j][i] = Neutral;
                
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

    // 四连通方向
    const int dx[] = { 0,  1,  0, -1};
    const int dy[] = {-1,  0,  1,  0};
    // bfs
    const char NON_CONTROL = 0;
    const char UNDER_CONTROL = 1;
    const char NEW_POSITION = 2;

    // 检查MilitaryCommandList战争指令是否可行;  并扣除战争所花的钱; 最后生成此玩家的影响力图;
    for (TId id = 0; id<playerSize; ++id) {
        vector<TMilitaryCommand>& tmc_list = MilitaryCommandList[id];
        size_t i = 0;
        int valid_command_cnt = 0;

        // 检查军事指令
        for (i=0; i<tmc_list.size(); ++i) {
            TMilitaryCommand& tmc = tmc_list[i];
            TId t_owner;

            // 检查位置
            if (!map.isPosValid(tmc.place)) { tmc.bomb_size = 0; continue; }
            t_owner = globalMap[tmc.place.x][tmc.place.y];

            if (!isPlayer(t_owner) || diplomacy[id][t_owner] != Allied) { tmc.bomb_size = 0; continue; }

            // 检查金额
            if (tmc.bomb_size < 0) { tmc.bomb_size = 0; continue; }
            if (tmc.bomb_size > playerSaving[id]) { 
                // out of cash
                tmc.bomb_size = playerSaving[id]; 
                playerSaving[id] = 0; break;
            }
            playerSaving[id] -= tmc.bomb_size;
            
            // 不能同时放置超过MILITARY_COUNT_LIMIT个活动
            ++valid_command_cnt;
            if (valid_command_cnt>=MILITARY_COUNT_LIMIT) break;
        }
        // 清除剩余的指令
        ++i;
        if (i<tmc_list.size()) 
            for (; i<tmc_list.size(); ++i) tmc_list[i].bomb_size = 0;

        // 添加无消耗的首都自卫指令
        if (map.isPosValid(playerCapital[id])) {
            TMilitaryCommand tmc;
            tmc.place = playerCapital[id];
            tmc.bomb_size = playerIncome[id] * CAPITAL_INFLUENCE;
            tmc_list.push_back(tmc);
        }

        // 生成PlayerSize张影响力图
        vector<vector<TMilitary> >& my_map = player_influence_map[id];
        for (int i=0; i<tmc_list.size(); ++i) {
            TMilitaryCommand& tmc = tmc_list[i];
            int x = 0, y = 0;
            if (tmc.bomb_size<=0) continue;
            for (int n=0; n<2*MILITARY_KERNEL_SIZE-1; ++n)
                for (int m=0; m<2*MILITARY_KERNEL_SIZE-1; ++m) {
                    x = tmc.place.x + n - MILITARY_KERNEL_SIZE + 1; 
                    y = tmc.place.y + m - MILITARY_KERNEL_SIZE + 1; 
                    if (!map.isPosValid(x,y)) continue;
                    my_map[x][y] += MilitaryKernel[n][m] * tmc.bomb_size;
                }
        }
        // 规则更新 背刺buff
        if (backstab_enabled[id]) {
            for (int x = 0; x < cols; ++x)
                for (int y = 0; y < rows; ++y)
                    my_map[x][y] = my_map[x][y] + my_map[x][y] / 2;
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
                if (!isPlayer(owner) || diplomacy[owner][id] == AtWar) {
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
        vector<vector<char> > my_map(cols,
            vector<char>(rows, NON_CONTROL));
        // bfs用的队列
        queue<TPosition> q;
        
        // 初始化地图
        for (TMap x=0; x<cols; ++x)
            for (TMap y=0; y<rows; ++y) {
                TId owner = globalMap[x][y];
                if (isPlayer(owner) && diplomacy[id][owner] == Allied) {
                    TPosition pp = {x, y};
                    q.push(pp);
                    my_map[x][y] = UNDER_CONTROL;
                }
                else if (map_new_owner[x][y] == id) {
                    my_map[x][y] = NEW_POSITION;
                }
            }
        
        // 开始bfs进行连通性判定
        while(!q.empty()) {
            TPosition p = q.front();
            q.pop();
            for (size_t i=0; i<4; ++i) {
                int x = p.x + dx[i]; 
                int y = p.y + dy[i]; 
                if (!map.isPosValid(x,y)) continue;
                if (my_map[x][y] == NEW_POSITION) {
                    TPosition pp = {x,y};
                    my_map[x][y] = UNDER_CONTROL;
                    q.push(pp);
                }
            }
        }

        // 确认连通性
        for (TMap x=0; x<cols; ++x)
            for (TMap y=0; y<rows; ++y) 
                if (map_new_owner[x][y] == id && my_map[x][y] != UNDER_CONTROL) 
                    map_new_owner[x][y] = NEUTRAL_PLAYER_ID; // 火力足够大但是于己方领土不连通，炸成中立
    }

    // 更新世界地图
    for (TMap x=0; x<cols; ++x)
        for (TMap y=0; y<rows; ++y) 
            if (map_new_owner[x][y] != UNKNOWN_PLAYER_ID) {
                ++exciting_game_score;
                globalMap[x][y] = map_new_owner[x][y];
            }

    // 强行去除无敌土地上的领土        
    for (TMap x=0; x<cols; ++x)
        for (TMap y=0; y<rows; ++y)
            if (MapAtk[x][y]<=0)
                globalMap[x][y] = NEUTRAL_PLAYER_ID;

    // 更新首都
    for (TId id = 0; id<playerSize; ++id) {
        TPosition p = NewCapitalList[id];

        // 自动修正违规的首都指令
        if (!map.isPosValid(p)) p = playerCapital[id];

        // 位置有效性检查
        if (map.isPosValid(p)) { 
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
            isSieged[x][y] = true;
    // 判定
    for (TId id = 0; id<playerSize; ++id) {
        TPosition p = playerCapital[id];
        if (map.isPosValid(p)) {
            vector<vector<char> > my_map(cols, 
                vector<char>(rows, NON_CONTROL));
            // bfs用的队列
            queue<TPosition> q;

            // 确认军通区域
            for (TMap x=0; x<cols; ++x)
                for (TMap y=0; y<rows; ++y) {
                    TId owner = globalMap[x][y];   
                    if (isPlayer(owner) && diplomacy[id][owner]==Allied) my_map[x][y] = NEW_POSITION;
                }
            // 同盟首都
            for (TId uid=0; uid<playerSize; ++uid) 
                if (diplomacy[id][uid]==Allied && map.isPosValid(playerCapital[uid])) {
                    my_map[playerCapital[uid].x][playerCapital[uid].y] = UNDER_CONTROL;
                    q.push(playerCapital[uid]);
                }
            // 开始bfs
            while(!q.empty()) {
                TPosition p = q.front();
                q.pop();
                for (size_t i=0; i<4; ++i) {
                    int x = p.x + dx[i]; 
                    int y = p.y + dy[i]; 
                    if (!map.isPosValid(x,y)) continue;
                    if (my_map[x][y] == NEW_POSITION) {
                        TPosition pp = {x,y};
                        my_map[x][y] = UNDER_CONTROL;
                        q.push(pp);
                    }
                }
            }

            // 确认 断补
            for (TMap x=0; x<cols; ++x)
                for (TMap y=0; y<rows; ++y)
                    if (globalMap[x][y]==id && my_map[x][y]==UNDER_CONTROL) isSieged[x][y] = false;
        }
        else {
            // 首都不存在时，己方领土全部失守
        }
    }

    return true;
}

// 确认ID是否为玩家
bool Game::isPlayer(TId id) const
{
    return id >= 0 && id < playerSize;
}

// 生产阶段
bool Game::ProducingPhase()
{

    vector<vector<TMoney> > mapRes = map.getMapRes();

    vector<int>	bakPlayerArea = playerArea;// 为了计算游戏兴奋程度，留下一个备份

    // 初始化
    for (TId id=0; id<playerSize; ++id)
        if (isPlayerAlive[id]) {
            playerArea[id] = 0;
            playerIncome[id] = 1; // 至少有社长一个人
        }
        else {
            playerArea[id] = 0;
            playerIncome[id] = 0;
        }
    // 更新income，mapArea
    for (TMap i=0; i<cols; i++) 
        for (TMap j=0; j<rows; j++) 
            if (isPlayer(globalMap[i][j])) {
                playerIncome[globalMap[i][j]] += mapRes[i][j];
                ++playerArea[globalMap[i][j]];
            }

    // 收入
    for (TId id=0; id<playerSize; ++id)
	{
        // 资金腐败
        playerSaving[id] = (TMoney)((1-(float)(playerIncome[id])*CORRUPTION_COEF) * (float) playerSaving[id]);
        // 首都低保
		if (map.isPosValid(playerCapital[id])) 
			playerSaving[id] += (TMoney)(UNIT_CITY_INCOME * (float)round);
        // 社员
		playerSaving[id] += playerIncome[id];
    }

    return true;
}

// 宣判玩家离场
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

// 检查玩家存活，并更新排名
bool Game::CheckWinner()
{
    // 全灭式离场
    for (TId id=0; id<playerSize; id++) 
        if (isPlayerAlive[id] && playerArea[id] == 0) {
            killPlayer(id);
            player_ranking[aliveCnt] = id;
        }

    // 刷新排名
    for (int i = 0; i < aliveCnt; ++i) player_ranking[i] = UNKNOWN_PLAYER_ID;
    for (TId id = 0; id < playerSize; ++id) 
        if (isPlayerAlive[id]) {
            int r = 0;
            while (isPlayer(player_ranking[r]) && playerIncome[player_ranking[r]] > playerIncome[id]) ++r;
            for (int i = aliveCnt - 1; i > r; --i) player_ranking[i] = player_ranking[i - 1];
            player_ranking[r] = id;
        }
    
    // 严格模式下末位淘汰 STRICT_ROUND_START
    if (round > STRICT_ROUND_START) 
        for (int r=0; r<playerSize; ++r) {
            TId id = player_ranking[r];
            if (playerIncome[id] < round - STRICT_ROUND_START) killPlayer(id);
        }
    
    if (aliveCnt == 1 || round == MAX_ROUND)
        return true;
    else 
        return false;
}

// 更新地图验证信息 CheckSum
void Game::UpdateMapChecksum() {
    map_checksum = 47831;
    for (int i=0; i<cols; ++i)
        for (int j=0; j<rows; ++j)
            map_checksum = map_checksum * 17 + globalMap[i][j];
}

// 获取玩家信息
PlayerInfo Game::getPlayerInfo(TId id, TId playerId) const
{
	PlayerInfo p;
	p.dipStatus = diplomacy[playerId][id];

    // 是否可见 visible
    p.isVisible = p.dipStatus != Undiscovered;
    p.income = p.isVisible ? playerIncome[id] : 0;
    p.mapArea = p.isVisible ? playerArea[id] : 0;
    if (p.isVisible) p.warList = getWarList(id);
    
    // 是否同盟 union
    p.isUnion = p.dipStatus == Allied;
    p.saving = p.isUnion ? playerSaving[id] : 0;
    p.capital = p.isUnion ? playerCapital[id] : INVALID_POSITION;

	return p;
}

// 点是否可见
TMask Game::isPointVisible(TMap x, TMap y, TId playerId) const
{
    if (!isPlayer(playerId)) return true;
    if (!map.isPosValid(x,y)) return false;

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

// 获取地图信息
MapPointInfo Game::getMapPointInfo(TMap x, TMap y, TId playerId) const
{
	MapPointInfo mp;

    mp.atk = map.getMapAtk(x,y);
    mp.def = map.getMapDef(x,y);
    mp.res = map.getMapRes(x,y);

    mp.isVisible = isPointVisible(x, y, playerId);
    mp.isSieged = mp.isVisible ? isSieged[x][y] : false;
    mp.owner = mp.isVisible ? globalMap[x][y] : UNKNOWN_PLAYER_ID;

	return mp;
}

// 获取整体信息
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
	for (TId id=0; id<playerSize; id++) {
		info.playerInfo[id] = getPlayerInfo(id, playerid);
        info.DiplomaticCommandList[id] = getDefaultCommand(diplomacy[playerid][id]);
	}
	info.mapPointInfo = vector<vector<MapPointInfo> >(cols,
                        vector<MapPointInfo>(rows));
	for (TMap i=0; i<cols; i++)
		for (TMap j=0; j<rows; j++)
			info.mapPointInfo[i][j] = getMapPointInfo(i, j, playerid);
	
	return info;
}

// 获取全国地图
vector<vector<MapPointInfo> > Game::getGlobalMap(TId id) const
{
    vector<vector<MapPointInfo> > map(cols,
        vector<MapPointInfo>(rows));
    for (TMap i = 0; i < cols; ++i)
        for (TMap j = 0; j < rows; ++j)
            map[i][j] = getMapPointInfo(i, j, id);
    return map;
}

// 国家发现
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

// 默认外交指令
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

// 检查出生点是否合法  MIN_ABS_DIST_BETWEEN_CAP
bool Game::canSetGlobalMapPos(TPosition pos, TId id)
{
    if (pos.x<0 || pos.x>=cols) return false;
    if (pos.y<0 || pos.y>=rows) return false;
    if (globalMap[pos.x][pos.y] != NEUTRAL_PLAYER_ID) return false;

    for (TId i=0; i<id; i++) 
        if (absDist(playerCapital[i], pos)<MIN_ABS_DIST_BETWEEN_CAP) return false;
    
    return true;
}

}