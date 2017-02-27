// sample ai

#include "ai.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <iostream>
using namespace std;//c++ 语法，只是为了使用vector

// Ai参数 ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- 
int cols,rows;//地图大小
const int MIN_BOMB_SIZE=40;//炸弹的最小大小

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
        if (getDistance(posSelected[i], pos)<MIN_ABS_DIST_BETWEEN_CAP) 
			return false;
    return true;
}
//判断是否出界
bool outOfRange(int indexX, int indexY){
	return indexX < 0 || indexY < 0
		|| indexX >= cols
		|| indexY >= rows;
}
//判断是否是边界
bool checkBoarder(int x,int y, Info& info)
{
    int dx[] = {1, -1,  0, 0};
    int dy[] = {0,  0, -1, 1};
	bool flag=false;
	//暴力做法，遍历上下左右四个方向
	for(int i=0;i<4;i++)
	{
        int fx = x+dx[i];
        int fy = y+dy[i];
		if (!outOfRange(fx,fy) )
			if (info.mapPointInfo[fx][fy].isVisible)
				if (info.mapPointInfo[fx][fy].owner!=info.id)
					flag=true;
	}
	return flag;
}


// 主函数 Main function ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- 
// 竞价流程 - 出钱
TMoney birthplacePrice(BaseMap* map)
{
	return 3;
}
// 竞价流程 - 选择出生点
TPosition birthplace(vector<TPosition> posSelected, BaseMap* map)
{   
	cols = map->getCols();
    rows = map->getRows();
	vector<vector<TMilitary> > mapAtk = map->getMapAtk();// get map atk info if needed
    vector<vector<TMoney> > mapRes = map->getMapRes();// get map money info if needed
	vector<vector<TMilitary>> mapDef=map->getMapDef();//get map defence info if needed
	srand((unsigned)time(NULL)); 

    TPosition pos;
	TPosition targetPos;
	int rand_cout=0;
	int score=0,bestscore=-100000;

	while (rand_cout<40)
	{
		score=0;
        pos.x = rand_f() * cols; //map[col][row]
        pos.y = rand_f() * rows;
        if (checkBirthPosValidity(posSelected, pos))// if valid
		{
			if (mapAtk[pos.x][pos.y]>20)//攻击系数太小，炸不开，开局死
			{
				for(int i=0;i< posSelected.size();i++)//对每个已选的玩家
					if (getDistance(pos,posSelected[i])<8)//太近不好哦
						score-=10;
				for(int i=-1;i<=1;i++)//遍历这个点周围的情况，钱多的地方有前途!
					for(int j=-1;j<=1;j++)
					{
						if (!outOfRange(pos.x+i,pos.y+j))
							score+=mapRes[pos.x+i][pos.y+j]+mapDef[pos.x+i][pos.y]/10;
					}
				if (score>bestscore)
				{
					targetPos=pos;
					bestscore=score;
				}
				rand_cout++;
			}
		}
    }
    return targetPos;
}
// 主流程
void player_ai(Info& info)
{
	srand((unsigned) time(NULL));
    size_t n = info.playerSize;
    size_t my_id = info.id;
    int money = info.playerInfo[my_id].saving;

	vector<TPosition> m_border;//自己所有边界的坐标，可视作数组
	vector<TPosition> m_map;//自己所有地盘的坐标，可视作数组

	//在决策之前，先阅读地图信息
	for(int i=0;i<cols;i++)
		for (int j=0;j<rows;j++)
		{
			MapPointInfo& mpi=info.mapPointInfo[i][j];
			if (mpi.owner==my_id)//如果是自己的地
			{
				TPosition temp={i,j};
				m_map.push_back(temp);
				if (checkBoarder(i,j,info))//如果是自己的边界
					m_border.push_back(temp);
			}		
		}

    // 外交阶段 diplomatic ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- 
    for (int i=0; i<n; ++i) //对每个玩家
	{
        if (i==my_id) continue;
		if (info.playerInfo[i].isVisible)
		{
			info.DiplomaticCommandList[i]=JustifyWar;//类似的指令可以见 TDiplomaticCommand
			money-=WAR_JUSTIFY_PRICE;
		}
		else
			info.DiplomaticCommandList[i]=KeepNeutral;
    }

    // 军事 military ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- 
	while(money>=0)//直到钱用完
	{
		TMilitaryCommand t;
		int randomPlaceId=rand() % m_border.size();//随机一个自己的边界
		t.place=m_border[randomPlaceId];
		t.bomb_size=MIN_BOMB_SIZE;//设置一个小炸弹
		info.MilitaryCommandList.push_back(t);//在军事指令里加入新的指令
		money-=t.bomb_size;
	}

    // 选择新的首都 my capital
	int randomId=rand() % m_map.size();//随机一个自己的地盘
	info.newCapital=m_map[randomId];//设置新的首都

}

