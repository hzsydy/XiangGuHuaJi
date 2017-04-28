#include"user_toolbox.h"


typedef int TPoint ;
const float max_=1.5;
const float min_=0.8;
const int B_bomb=70;
const int M_bomb=50;
const int S_bomb=40;
const int C_bomb=40;
const int P_round=10;     //前期不宣战的回合数
const int A_ronud=49;     //后期开始乱斗的回合数
static unsigned int old_area=0;
static vector<TId>m_next;         //自己的相邻玩家
static int war_order=P_round;          //初始值意味着前多少个回合不进行宣战
static int my_next_enemy=-1;      //我要动手的玩家
static int m_under_atk=-1;         //我是否被意料之外的玩家攻击
static int attack_money;
int bomb;
int cols,rows;

bool checkBirthPosValidity(vector<TPosition>& posSelected, TPosition pos){
    for (int i=0; i<posSelected.size(); ++i)
        if ( get_distance(posSelected[i], pos)<MIN_ABS_DIST_BETWEEN_CAP) 
			return false;
    return true;
}

bool outOfRange(int indexX, int indexY){
	return indexX < 0 || indexY < 0
		|| indexX >= cols
		|| indexY >= rows;
}

bool checkBoarder(int x,int y, Info info, vector<vector<TMilitary> >& mapAtk)
{
    int dx[] = {1, -1,  0, 0};
    int dy[] = {0,  0, -1, 1};
	bool flag=false;
	for(int i=0;i<4;i++)
	{
        int fx = x+dx[i];
        int fy = y+dy[i];
		if (!outOfRange(fx,fy) )
            if (mapAtk[fx][fy]>0) // not water
			    if (info.mapPointInfo[fx][fy].isVisible)
				    if (info.mapPointInfo[fx][fy].owner != info.id)
					    flag=true;
	}
	return flag;
}

bool isnext(TId i,vector<TId>my_n)
{
	for (auto a:my_n)
		if(i==a)
			return true;
	return false;
}

int count_enemy_one(TPosition point, Info info,TId enemy) {
	int x=point.x;
	int y=point.y;
    TPosition dd[] = {
        {1, 0}, {-1, 0}, {0, -1}, {0, 1},
		{1, 1}, {-1, 1}, {1, -1}, {-1, -1},
    };
	int border_count = 0;
    for(TPosition d : dd) {
        int fx = x + d.x;
        int fy = y + d.y;
        if (info.map->isPosValid(fx,fy) ) // 不要超出地图
            if (info.map->getMapAtk(fx,fy)>0) // 不是水域
            {
                TId owner = info.mapPointInfo[fx][fy].owner;

                if (owner == enemy)  
                    ++border_count;
            }
	}
	return border_count;
}

TPoint point_good(TPosition point,Info info)                      //增加了对于土地和自己领地的理解
{
	int x=point.x;
	int y=point.y;
	int good=0;
	int o_good=0;
    TPosition dd[] = {
        {1, 0}, {-1, 0}, {0, -1}, {0, 1},
        {1, 1}, {-1, 1}, {1, -1}, {-1, -1},
		{0,2},{0,-2},{2,0},{-2,0},
    };
    for(TPosition d : dd) {
        int fx = x + d.x;
        int fy = y + d.y;
        if (info.map->isPosValid(fx,fy) )                                // 不要超出地图
            if (info.map->getMapAtk(fx,fy)>0)                           // 不是水域
			{
				TId owner=info.mapPointInfo[fx][fy].owner;
				if(owner==NEUTRAL_PLAYER_ID)
					good+= info.mapPointInfo[fx][fy].res+1;
				if(owner<info.playerSize)
				{
					if (owner == info.id)    
						o_good+=1+info.mapPointInfo[fx][fy].res/3;
						if(info.playerInfo[owner].dipStatus==AtWar)
					good+= info.mapPointInfo[fx][fy].res+1;
				if(info.mapPointInfo[fx][fy].atk==10&&info.mapPointInfo[fx][fy].def==15)
					o_good+=3;
				}
			}
	}
	if(good!=0)
		good+=o_good;
	//printf("%f  ",good);
	return good;
}     
                                                               //上一个函数包含中立地和敌对地
float point_attack(TPosition point,Info info,int old_money)               //一个问题是这个函数不知道谁是敌人
{
	int x=point.x;
	int y=point.y;
	float attack=-100;
    TPosition dd[] = 
	{
        {1, 0}, {-1, 0}, {0, -1}, {0, 1},
        {1, 1}, {-1, 1}, {1, -1}, {-1, -1},
    };
    for(TPosition d : dd)
	{
        int fx = x + d.x;
        int fy = y + d.y;
        if (info.map->isPosValid(fx,fy) )                                // 不要超出地图
			if (info.mapPointInfo[fx][fy].owner  != info.id)  
				if (info.map->getMapAtk(fx,fy)>0)                           // 不是水域
					if((info.map->getMapDef(fx,fy)/info.map->getMapAtk(fx,fy))>attack)
						attack=info.map->getMapDef(fx,fy)/info.map->getMapAtk(fx,fy);
		if(info.mapPointInfo[fx][fy].atk==10&&info.mapPointInfo[fx][fy].def==15)
			        attack+=0.3;
	}
	if(count_enemy(point.x,point.y,info)==0)
		return 0;
	if(old_money<1200)
	attack+=count_enemy(point.x,point.y,info)/10+0.8;
	else
		attack+=count_enemy(point.x,point.y,info)/10+0.5;
	return attack;
}


void pre_to_war(Info info,int money)
{
	int dan=10;
	war_order=(attack_money-money)/info.playerInfo[info.id].income;
		for(int i=0;i<info.playerSize;++i)
		{
			if(isnext(i,m_next))
			if(danger[i]<=dan)
				dan=danger[i];
			  my_next_enemy=i;
		}
		if(war_order<2)
			war_order=2;
}

void change_next(Info info,vector<TPosition>m_border)
{
	m_next.clear();
			for(auto q:m_border)
		{
			int x=q.x;
	         int y=q.y;
				 TPosition dd[] = {
        {1, 0}, {-1, 0}, {0, -1}, {0, 1},
        {1, 1}, {-1, 1}, {1, -1}, {-1, -1},
			 };
    for(TPosition d : dd) {
        int fx = x + d.x;
        int fy = y + d.y;
        if (info.map->isPosValid(fx,fy) ) // 不要超出地图
            if (info.map->getMapAtk(fx,fy)>0) // 不是水域
			{
				 TId owner = info.mapPointInfo[fx][fy].owner;
				 m_next.push_back(owner);
	        }
	     }
		}
}

int count_none_me(TPosition point, Info info) 
{
	int x=point.x;
	int y=point.y;
    TPosition dd[] = 
	{                                        // 遍历32个方向
        {1, 0}, {-1, 0}, {0, -1}, {0, 1},
		{1, 1}, {-1, 1}, {1, -1}, {-1, -1},
		{0,2},{0,-2},{2,0},{-2,0},
		{2,2},{2,-2},{-2,2},{-2,-2},
		{1,2},{-1,2},{1,-2},{-1,-2},
		{2,1},{2,-1},{-2,1},{-2,-1},
		{3,0},{0,3},{-3,0},{0,-3},
		{4,0},{-4,0},{0,4},{0,-4},
    };
	int border_count = 0;
    for(TPosition d : dd) {
        int fx = x + d.x;
        int fy = y + d.y;
        if (info.map->isPosValid(fx,fy) )               // 不要超出地图
            if (info.map->getMapAtk(fx,fy)>0)              // 不是水域
            {
                TId owner = info.mapPointInfo[fx][fy].owner;

                if (owner == NEUTRAL_PLAYER_ID)  
                    ++border_count;                        //中立的地
                else if (owner<info.playerSize)
				{ 
					if (owner!=info.id) 
						++border_count;                     // 不是我
                }

            }
	}
	return border_count;
}

int war_ob(Info info)
{
	int inc=20000;
	int war_=-1;
	for(auto a:info.playerInfo[info.id].warList)
		{
			if(a==info.id)  continue;
			if(isnext(a,m_next))
				if(info.playerInfo[a].income<inc)
				{
					if(a==info.id) continue;
					inc=info.playerInfo[a].income;
					war_=a;
				}
		}
	return war_;
}

TMoney birthplacePrice(BaseMap* map) 
{    return 12;   }

TPosition birthplace(vector<TPosition>& posSelected, BaseMap* map) 
{ 
	int ass=1;
	TPosition birthplace_list[] = {{16, 24},/*李兆基*/{7,11},/*万人*/ {19, 6},/*紫荆*/{16,15}/*六教*/,{4,30},/*北大*/  };            //紫荆万人是个坑
    for (TPosition p : birthplace_list) 
	{
		ass=1;
		if (check_birth_pos_validity(posSelected, p))
		{
			for(int i=0;i< posSelected.size();i++)
					if (get_distance(p,posSelected[i])<10)
						ass=0;
			if(ass)
			return p;
		}
	}
	                                                               //选不出来就只能随机了
	std::srand((unsigned)time(NULL));
	
		cols = map->getCols();
    rows = map->getRows();
	vector<vector<TMilitary> > mapAtk = map->getMapAtk();// get map atk info if needed
    vector<vector<TMoney> > mapRes = map->getMapRes();// get map money info if needed
	vector<vector<TMilitary>> mapDef=map->getMapDef();//get map defence info if needed

	 
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
					if (get_distance(pos,posSelected[i])<8)//太近不好哦
						score-=15;
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

void player_ai(Info& info)
{
	TId my_id=info.id;
	int n=info.playerSize;
	int money=info.playerInfo[my_id].saving;
	int area=info.playerInfo[my_id].mapArea;
	int back=-1;
	int old_money=money;
	cols=25;
	rows=35;
	attack_money=500*(info.round/10+1);
	if(attack_money>=1500)
		attack_money=1500;
	vector<vector<TMilitary> > mapAtk = info.map->getMapAtk();// get map atk info if needed
    vector<vector<TMoney> > mapRes = info.map->getMapRes();// get map money info if needed
	vector<vector<TMilitary>> mapDef = info.map->getMapDef();//get map defence info if needed
	vector<TPosition> m_border;//自己所有边界的坐标，可视作数组
	vector<TPosition> m_map;//自己所有地盘的坐标，可视作数组

     //在决策之前，先阅读地图信息，并进行评级-------------------------------------------------------------------
		for(int i=0;i<cols;i++)
		for (int j=0;j<rows;j++)
		{
			MapPointInfo& mpi=info.mapPointInfo[i][j];
			if (mpi.owner==my_id)//如果是自己的地
			{
				TPosition temp={i,j};
				m_map.push_back(temp);
				if (checkBoarder(i,j,info,mapAtk))//如果是自己的边界
					m_border.push_back(temp);
			}		
		}
		change_next(info,m_border);
		if(money<500)
			bomb=S_bomb;
		else
		{
			if(money<1200)
				bomb=M_bomb;
			else
				bomb=B_bomb;
		}
		if(my_next_enemy<0&&war_order==0)
		pre_to_war(info,money);
		if(money>=attack_money&&info.round<=P_round&&my_next_enemy<0)
		pre_to_war(info,money);

	// 外交 DiplomaticCommandList ------------------------------------------------------------------
                                //使用背刺
		if(info.backstabUsed!=1)
		{
			if(area<=(info.round/10+1)*10&&info.round<=15)
			 back=war_ob(info);
			else
				if(war_order==0&&my_next_enemy>=0&&info.playerInfo[my_next_enemy].income>=info.playerInfo[my_id].income)
				back=my_next_enemy;
			 else 
				 if(info.round>=47)
				back=my_next_enemy;
		}
		if(back>=0)
		{
			info.DiplomaticCommandList[back]=Backstab;
			for(int i=0;i<n;++i)
			{
				if(i==my_id||i==back)  continue;
				else
					info.DiplomaticCommandList[i]=FormAlliance;
			}
		}
		else
		{
			if(info.playerInfo[my_id].warList.empty())
			{
					for(int i=0;i<n;++i)
					{
						if(i==my_id)
							continue;
						if(i==my_next_enemy&&war_order>=0)
						{
							info.DiplomaticCommandList[i]=JustifyWar;
							money-=WAR_JUSTIFY_PRICE;
						}
						else
						{
							if(info.round<=A_ronud)
							{
								if(info.playerInfo[i].income<=info.playerInfo[my_id].income*0.8)
							info.DiplomaticCommandList[i]=KeepNeutral;
							else
								info.DiplomaticCommandList[i]=FormAlliance;
							}
							else
								info.DiplomaticCommandList[i]=JustifyWar;
						}
					}
			}
			else
			{
				if(info.playerInfo[my_next_enemy].dipStatus==AtWar||info.playerInfo[my_id].income>=attack_money*0.7)
				 {
					 info.DiplomaticCommandList[my_next_enemy]=JustifyWar;
					money-=WAR_JUSTIFY_PRICE;
					war_order=0;
				 }
				else
				{
					my_next_enemy=-1;
				}
				for(TId i:info.playerInfo[my_id].warList)
				{
					if(i!=my_next_enemy&&i!=my_id)
						m_under_atk=i;
					break;
				}
				if(m_under_atk>=0)                                                                 //被攻击了
						if(info.playerInfo[m_under_atk].income<=info.playerInfo[my_id].income+200&&info.playerInfo[my_id].income>=attack_money*0.8)
						{
							info.DiplomaticCommandList[m_under_atk]=JustifyWar;
							money-=WAR_JUSTIFY_PRICE;
							war_order=0;
							my_next_enemy=m_under_atk;
						}
						else 
						pre_to_war(info,money);
				for(int i=0;i<n;++i)
					{
						if(i==my_id||i==my_next_enemy&&war_order!=0)
							continue;
						else
						{
							if(info.round<=A_ronud)
							{
								if(info.playerInfo[i].income<=info.playerInfo[my_id].income*0.8)
							info.DiplomaticCommandList[i]=KeepNeutral;
							else
								info.DiplomaticCommandList[i]=FormAlliance;
							}
							else
								info.DiplomaticCommandList[i]=JustifyWar;
						}
					}
			}
		}
		//printf("%d\n%d",war_order,my_next_enemy);
	// 军事 MilitaryCommandList ------------------------------------------------------------------
		TMilitaryCommand t;int a=1;
		if(war_order<=0||info.round<=P_round)
		{
			int good=0;
			for(TPosition i : m_border)
			{

				if( point_good(i,info)>good)
				good=point_good(i,info);
			}
			if(good!=0)
			{
				for(int count=good;money>=0&&count>0;--count)
				for(auto i:m_border)
			  {
				if(point_good(i,info)==count&&i.x!=0&&i.x!=24)
			     {
					t.place=i;
					if(war_order<=0)
						t.bomb_size=bomb*point_attack(i,info,old_money);
					else
						t.bomb_size=C_bomb;
					money-=t.bomb_size;
					
					info.MilitaryCommandList.push_back(t);
						
				 }
					if(money<0)
			     		break;
			  }
		}
		}

	// 选择新的首都 newCapital ------------------------------------------------------------------
		if(info.round<=5&&info.playerInfo[my_id].warList.empty())
		{
				int randomId=rand() % m_border.size();     //随机一个自己的地盘
	            info.newCapital=m_border[randomId];           //设置新的首都
				
		}
		else
		{
			int ene=33;
			TPosition newc;
			for(auto a:m_map)
			{
					if(count_none_me(a,info)<ene)
					{
						ene=count_none_me(a,info);
						newc=a;
					}
			}
			info.newCapital=newc;
		}
		old_area=area;
		if(war_order>0)
		war_order--;
}
