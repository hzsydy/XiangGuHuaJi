// *********************************************************
//                 请不要修改或提交此文件
//            Do NOT edit or commit this file
// *********************************************************

// FC14选手工具包
// 自动军事 auto_military 有一定的参考价值哦
// Created by sugar10w, 2017.03
// Last modified by sugar10w, 2017.03.17

#include "user_toolbox.h"


// 军事部分 可以复制这3个函数到ai.cpp，并将函数名改成my_military
// *********************************************************
// 我的地图点
struct MyMapPointInfo {
    TPosition pos;
    MapPointInfo mpi;
    int enemy_count;
    int friend_count;
};
// 对点的估价
static int evaluate(MyMapPointInfo& t) {
    int value = 10;
    for (TPosition p : BRIGDES) if (get_distance(p, t.pos)<=2) value*=2;
    return value;
}
// 自动军事
vector<TMilitaryCommand> auto_military(const Info& info, TMaskMap& mask, int money, int size)
{
    vector<TMilitaryCommand> MilitaryCommandList;
    vector<TMilitaryCommand> candidates;
    MaxHeap<float, MyMapPointInfo> heap;
    int summary_value = 0;
    int count_command = 0;
    const int MIN_MONEY = 25;

    // 处理 money 和 size
    if (money < MIN_MONEY) return MilitaryCommandList;
    if (MIN_MONEY * size > money) size = money / MIN_MONEY;
    if (size > MILITARY_COUNT_LIMIT) size = MILITARY_COUNT_LIMIT;

    // 检查mask是否合法
    if (!check_mask_validity(mask, info))
        mask = get_border_TMaskMap(info);

    for (int i = 0; i < info.cols; ++i)
        for (int j = 0; j < info.rows; ++j) 
            if (mask[i][j]) {
                const MapPointInfo& mpi=info.mapPointInfo[i][j];
                MyMapPointInfo temp = { {i,j}, mpi, 
                    count_enemy(i,j,info), count_friend(i,j,info)}; //生成此点的局部信息
                heap.add(evaluate(temp)+rand_f(), temp);  //估价并压入最大堆
            }

    // 取出估价最高的size个指令
    for (int _=0; _<size; ++_) {
        MyMapPointInfo temp;
        int value = heap.getMax(temp);
        heap.removeMax();
        if (value>0) {
            TMilitaryCommand tmc = {temp.pos, value};
            candidates.push_back(tmc);
            summary_value += value;
        }
        else break;
    }

    // 归一化 bomb_size
    if (candidates.size() > 0) {
        for (TMilitaryCommand& tmc : candidates) {
            tmc.bomb_size = int ((float)money * (float)tmc.bomb_size / (float)summary_value);
            if (tmc.bomb_size < MIN_MONEY) tmc.bomb_size = MIN_MONEY;
            MilitaryCommandList.push_back(tmc);
            
            money -= tmc.bomb_size;
            if (money <=0 ) break;
        }
    }

    return MilitaryCommandList;

}
// *********************************************************

// Implementation  ------------------------------------------------------------------

// 随机函数
float rand_f(){
    return (float)rand()/RAND_MAX;
}

// 获取 TMaskMap
TMaskMap new_TMaskMap(size_t x, size_t y) {
    return vector<vector<bool> > (x, vector<bool>(y, false));
}
TMaskMap new_TMaskMap(const Info& info) {
    return new_TMaskMap(info.cols, info.rows);
}
TMaskMap get_border_TMaskMap(const Info& info) {
    TMaskMap map = new_TMaskMap(info);
    for(int i = 0; i < info.cols; i++)
		for (int j = 0; j < info.rows; j++) {
			const MapPointInfo& mpi = info.mapPointInfo[i][j];
            if (mpi.owner == info.id || 
                    (mpi.owner < info.playerSize && info.playerInfo[mpi.owner].dipStatus == Allied))  //如果是自己或者盟友的地
                if (count_enemy(i,j,info) > 0) map[i][j] = true;
		}
    return map;
}
// 检查TMaskMap
bool check_mask_validity(const TMaskMap& mask, const Info& info) {
    if (mask.size() != info.cols) return false;
    for (const vector<bool>& v : mask) if (v.size() != info.rows) return false;
    return true;
}

// 获取曼哈顿距离
unsigned int get_distance(TPosition a, TPosition b) {
    return abs((int)a.x-(int)b.x)+abs((int)a.y-(int)b.y);
}
// 检查出生点是否合法
bool check_birth_pos_validity(vector<TPosition>& posSelected, TPosition pos) {
    for (int i=0; i<posSelected.size(); ++i)
        if (get_distance(posSelected[i], pos) < MIN_ABS_DIST_BETWEEN_CAP) 
			return false;
    return true;
}
// 类似扫雷，检查周围8格有多少中立/敌对点
int count_enemy(int x,int y, const Info& info) {
    TPosition dd[] = {
        {1, 0}, {-1, 0}, {0, -1}, {0, 1},
        {1, 1}, {-1, 1}, {1, -1}, {-1, -1},
    };
	int border_count = 0;

    // 遍历8个方向
    for(TPosition d : dd) {
        int fx = x + d.x;
        int fy = y + d.y;
        if (info.map->isPosValid(fx,fy) ) // 不要超出地图
            if (info.map->getMapAtk(fx,fy)>0) // 不是水域
            {
                TId owner = info.mapPointInfo[fx][fy].owner;

                if (owner == NEUTRAL_PLAYER_ID)  
                    ++border_count; //中立的地
                else if (owner<info.playerSize) { 
                    if (info.playerInfo[owner].dipStatus == AtWar) ++border_count; // 在和此玩家打架
                }

            }
	}
	return border_count;
}
int count_friend(int x,int y, const Info& info) {
    TPosition dd[] = {
        {1, 0}, {-1, 0}, {0, -1}, {0, 1},
        {1, 1}, {-1, 1}, {1, -1}, {-1, -1},
    };
	int border_count = 0;

    // 遍历8个方向
    for(TPosition d : dd) {
        int fx = x + d.x;
        int fy = y + d.y;
        if (info.map->isPosValid(fx,fy) ) // 不要超出地图
            if (info.map->getMapAtk(fx,fy)>0) // 不是水域
            {
                TId owner = info.mapPointInfo[fx][fy].owner;

                if (owner == NEUTRAL_PLAYER_ID)  
                    continue; //中立的地
                else if (owner<info.playerSize) { 
                    if (owner==info.id || info.playerInfo[owner].dipStatus == Allied) ++border_count; // 友方领土
                }

            }
	}
	return border_count;
}


// 随机出生地点
TPosition random_birthplace(vector<TPosition> posSelected, BaseMap* map) {
    srand(time(NULL));
    while (true) {
        TPosition p = {rand() % map->getCols(), rand() % map->getRows()};
        if (map->getMapAtk(p.x, p.y)>0 && check_birth_pos_validity(posSelected, p)) return p;
    }
}

// 自动首都
TPosition auto_capital(const Info& info) {
    
    TPosition best_capital = {0, 0};    
    int best_score = -10000;
    
    for (int i=0; i<info.cols; ++i)
        for (int j=0; j<info.rows; ++j)
            if (info.mapPointInfo[i][j].owner == info.id) {
                
                TPosition p = {i,j};
                if (info.map->getMapAtk(i,j)==0) continue;

                int score = 10 *  count_friend(i,j,info) * info.map->getMapDef(i,j) / info.map->getMapAtk(i,j);
                if (score>best_score) {
                    best_score = score;
                    best_capital = p;
                }
        }

    return best_capital;
}