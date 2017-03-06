// sample ai

#include "ai.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>
using namespace std; //c++ 语法，为了使用vector

// 我的地图类型
struct myMapPointInfo {
    TPosition pos;
    MapPointInfo mpi;
    int border_count;
};
// 这里按照点的类型，以及点的周边信息，调整你的进攻/防御策略
int getBombSize(const myMapPointInfo& t) {
    //if (t.mpi.atk==10 && t.mpi.def==15) return 100; // 桥梁
    //return  t.border_count * 15 * t.mpi.def / t.mpi.atk; // 边缘的地方多放一点；守住防守高的地方；
    return 40;
}


// 工具 Tool function ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- 
// 随机函数
float rand_f(){
    return (float)rand()/RAND_MAX;
}
// 曼哈顿距离
unsigned int getDistance(TPosition a, TPosition b) {
    return abs((int)a.x-(int)b.x)+abs((int)a.y-(int)b.y);
}
// 出生点合理性检查
bool checkBirthPosValidity(vector<TPosition>& posSelected, TPosition pos){
    for (int i=0; i<posSelected.size(); ++i)
        if (getDistance(posSelected[i], pos) < MIN_ABS_DIST_BETWEEN_CAP) 
			return false;
    return true;
}
// 统计我方边界
int checkBoarder(int x,int y, Info& info)
{
    int dx[] = {1, -1,  0, 0, 1,  1, -1, -1};
    int dy[] = {0,  0, -1, 1, 1, -1,  1, -1};
	int border_count = 0;

    // 遍历8个方向
    for(int i=0;i<4;i++) {
        int fx = x+dx[i];
        int fy = y+dy[i];
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

// 主函数 Main function ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- 

// 竞价流程 - 出钱
TMoney birthplacePrice(BaseMap* map)
{
	return 3; // 决定出50元中的3元来获得选取出生点排位。这样第一回合有47元。
}
// 竞价流程 - 选择出生点
TPosition birthplace(vector<TPosition> posSelected, BaseMap* map)
{   
	int cols = map->getCols();
    int rows = map->getRows();
	vector<vector<TMilitary> > mapAtk = map->getMapAtk();   // get map atk info if needed
    vector<vector<TMoney> > mapRes = map->getMapRes();      // get map money info if needed
	vector<vector<TMilitary>> mapDef=map->getMapDef();      // get map defence info if needed

    TPosition pos;
	TPosition targetPos;
	int rand_cout=0;
	int score=0, bestscore=-100000;

    srand((unsigned)time(NULL)); 

	while (rand_cout<40)
	{
		score = 0;
        pos.x = rand_f() * cols; //map[col][row]
        pos.y = rand_f() * rows;

        if (!checkBirthPosValidity(posSelected, pos)) continue; // 随机到的点不合法
        if (mapAtk[pos.x][pos.y]<=20) continue;   // 拒绝在水/草/桥开局
        
		for(int i=0;i< posSelected.size(); i++)  // 远离每个已选的玩家
		    if (getDistance(pos,posSelected[i])<8) score-=10;

		for(int i=-1;i<=1;i++)  // 遍历这个点周围的情况，钱多&防高的地方有前途!
			for(int j=-1;j<=1;j++) 
                score += map->getMapRes(pos.x+i, pos.y+j) + map->getMapDef(pos.x+i, pos.y+j)/10;  // 越界的点自动返回0
		
		if (score>bestscore) { // 选一个最好的就行
			targetPos=pos;
			bestscore=score;
		}
		rand_cout++;
    }

    return targetPos;
}
// 主流程
void player_ai(Info& info)
{
	
    size_t n = info.playerSize;
    size_t my_id = info.id;
    int money = info.playerInfo[my_id].saving;
    int cols = info.map->getCols();
    int rows = info.map->getRows();
    vector<vector<TMilitary> > mapAtk = info.map->getMapAtk();  // get map atk info if needed
    vector<vector<TMoney> > mapRes = info.map->getMapRes();     // get map money info if needed
	vector<vector<TMilitary>> mapDef = info.map->getMapDef();   //get map defence info if needed

	vector<myMapPointInfo> m_border;  //自己所有边界的坐标和属性，可视作数组
	vector<myMapPointInfo> m_map;     //自己所有地盘的坐标和属性，可视作数组

    srand((unsigned) time(NULL));

	//在决策之前，先阅读地图信息
	for(int i=0;i<cols;i++)
		for (int j=0;j<rows;j++) {
			MapPointInfo& mpi=info.mapPointInfo[i][j];
			if (mpi.owner == my_id) { //如果是自己的地
                myMapPointInfo temp = { {i,j}, mpi, 0};
				m_map.push_back(temp);
                temp.border_count = checkBoarder(i,j,info); //如果是自己的边界
                if (temp.border_count>0) m_border.push_back(temp);
			}		
		}

    // 外交阶段 diplomatic ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- 
    for (int i=0; i<n; ++i) //对每个玩家
	{
        if (i == my_id) continue; // 不用对自己发外交指令
		
        if (info.playerInfo[i].isVisible && info.playerInfo[i].income < info.playerInfo[my_id].income) { // 对income比我少的玩家宣战
            info.DiplomaticCommandList[i] = JustifyWar;
			money -= WAR_JUSTIFY_PRICE;
		}
		else
			info.DiplomaticCommandList[i]=KeepNeutral; // 其余的中立
    }

    // 军事 military ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- 
    if (m_border.size() > 0)
	    while(money>=0) //直到钱用完
	    {
		    TMilitaryCommand t;
		    int randomPlaceId = rand() % m_border.size();// 随机一个自己的边界
            myMapPointInfo& temp = m_border[randomPlaceId];
            t.place = temp.pos;
            t.bomb_size = getBombSize(temp);  // 这里按照点的类型和周边信息，设计你的进攻/防御策略
		    info.MilitaryCommandList.push_back(t); // 在军事指令里加入新的指令

		    money -= t.bomb_size; // 用超出的部分会被逻辑自动修正
        
	    }

    // 选择新的首都 my capital
    if (m_map.size() > 0) {

        TPosition best_capital;    
        int best_score = -10000;

        for (int i=0; i<m_map.size(); ++i) {
            myMapPointInfo& temp = m_map[i];
            int score = 10 * (9 - temp.border_count) * temp.mpi.def / temp.mpi.def;
            if (score>best_score) {
                best_score = score;
                best_capital = temp.pos;
            }
        }
        
        info.newCapital = best_capital;
    }
}

