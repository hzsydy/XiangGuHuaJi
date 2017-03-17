/* XiangGuHuaJi 2016, definition.h
 * 
 */

#ifndef _XIANGGUHUAJI_DEFINITION_H__
#define _XIANGGUHUAJI_DEFINITION_H__

#include "debug.hpp"


typedef int          TId;       // player id 玩家编号
typedef unsigned int TMap;      // map size 地图尺寸
typedef unsigned int TRound;    // round 回合数
typedef int          TMoney;    // money 钱(SAV/INC)
typedef int          TMilitary; // military 军事计算
typedef bool         TMask;     // mask 蒙版

struct TPosition { TMap x, y; }; // 二维位置
const static TPosition INVALID_POSITION = {255, 255};

enum TDiplomaticStatus {
    Undiscovered,   // have never appeared in your sight 玩家还没有在你的视野中出现过
    Neutral,        // default diplomatic  默认外交关系
    Allied,          
    AtWar
};
enum TDiplomaticCommand {
    KeepNeutral, 
    FormAlliance, 
    JustifyWar,
	Backstab
};

struct TMilitaryCommand {
	TPosition place;        // 活动地点
	TMilitary bomb_size;    // 活动规模
};

class BaseMap
{
public:
	inline TMap getRows() const {return rows;}  // rows=25
	inline TMap getCols() const {return cols;}  // cols=35
	
    inline bool isPosValid(int x, int y) const { return x>=0&&x<cols&&y>=0&&y<rows; }
    inline bool isPosValid(TPosition pos) const  { return isPosValid(pos.x, pos.y); }

    inline vector<vector<TMoney> >    getMapRes() const {return MapResource_;}      // 资源分布图
	inline vector<vector<TMilitary> > getMapAtk() const {return MapAttackRatio_;}   // 攻击系数图
	inline vector<vector<TMilitary> > getMapDef() const {return MapDefenseRatio_;}  // 防守系数图
    
    inline TMoney getMapRes(int x, int y) const    { if (isPosValid(x,y)) return MapResource_[x][y]; else return 0;}        // res
    inline TMilitary getMapAtk(int x, int y) const { if (isPosValid(x,y)) return MapAttackRatio_[x][y]; else return 0;}     // atk
    inline TMilitary getMapDef(int x, int y) const { if (isPosValid(x,y)) return MapDefenseRatio_[x][y]; else return 0;}    // def


protected:
	TMap	rows, cols;
	vector<vector<TMoney> >  MapResource_;
	vector<vector<TMilitary> > MapDefenseRatio_, MapAttackRatio_;
};


// 游戏常数
const static TRound     MAX_ROUND = 50;             // 轮数限额
const static TRound     STRICT_ROUND_START = 10;    // 自动淘汰开始生效的回合
// 外交常数
const static TRound     WAR_JUSTIFY_TIME = 3;           // 确立敌对关系所需要的JustifyWar个数
const static TMoney     WAR_JUSTIFY_PRICE = 5;          // JustifyWar所需要的费用
const static float      UNIT_AREA_ALLY_COST = 0.05f;    // FormAlliance所需要支付的费用(比例)
// 军事常数
const static int        MILITARY_COUNT_LIMIT = 32;      // 一回合最多进行的有效活动数目
const static int		MILITARY_KERNEL_SIZE = 5;       // 影响力核心的半径 (尺寸为9*9)
const static TMilitary	SUPPESS_LIMIT = 400;            // 参考攻击判定 atk*atk_flu - def*def_flu >= SUPPRESS_LIMIT
const static float      CAPITAL_INFLUENCE = 0.3f;       // 首都自卫 (比例)
// 经济常数
const static TMoney     INITIAL_PLAYER_MONEY = 50;      // 初始金额
const static float		UNIT_CITY_INCOME = 1.0f;        // 低保比例系数
const static float		CORRUPTION_COEF = 0.0003f;      // 存款腐败系数
const static TMoney     UNIT_BOMB_COST = 1;             // 单位活动花费
// 地图常数
const static TMap		MIN_ABS_DIST_BETWEEN_CAP = 3;   // 首都之间的最小街道距离限制 曼哈顿距离 >= 3 
const static int		FIELD_BOUNDARY = 2;             // 一块领土的视野，曼哈顿距离 = 2
// 玩家ID常数
const static TId		NEUTRAL_PLAYER_ID = 233;
const static TId		UNKNOWN_PLAYER_ID = 213;

struct PlayerInfo {    
	TDiplomaticStatus dipStatus;    // 和你的外交状态
	bool isVisible;     // 是否可见，决定是否可知 basic info
	bool isUnion;       // 是否同盟，决定是否可知 all info
	
    // basic info
    TMoney income;          // 收入，即用于排名的社员数
	int mapArea;            // 领土总面积
	vector<TId> warList;    // 处在战争对象列表
	
    //all info
	TMoney saving;      // 对方的存款
	TPosition capital;  // 对方的首都位置
};

struct MapPointInfo {
    TMoney res;
    TMilitary atk, def;

	TMask isVisible;    // 是否可见
	TId owner;          // 主人
	TMask isSieged;
};

struct Info
{
	TId id;             // 我方id
	TId playerSize;     // 游戏总人数=8
	TMap rows;          // rows=25
	TMap cols;          // cols=35
	TRound round;       // 当前回合数

    bool backstabUsed;  // 我是否已经用了背刺

	BaseMap* map;       // 地图

    vector<PlayerInfo> playerInfo;              // 玩家信息
	vector<vector<MapPointInfo> > mapPointInfo; // 地图信息

    vector<vector<float> > militaryKernel;  // 军事kernel

	vector<TDiplomaticCommand> DiplomaticCommandList;   // 外交指令
    vector<TMilitaryCommand> MilitaryCommandList;       // 军事指令
	TPosition newCapital;                               // 新的首都位置
};

#endif
