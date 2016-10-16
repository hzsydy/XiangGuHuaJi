/* XiangGuHuaJi 2016, definition.h
 * 
 */

#ifndef _XIANGGUHUAJI_DEFINITION_H__
#define _XIANGGUHUAJI_DEFINITION_H__

#include "debug.hpp"


typedef unsigned char TId;       //PlayerId
typedef unsigned char TMap;  //x,y
typedef unsigned int  TRound;    //round 

typedef unsigned int  TMoney;   //money unit

typedef unsigned int  TMilitary; //Military

typedef bool TMask;

struct TPosition
{
	TMap x;
	TMap y;
};

const static TPosition invalidPos = {255, 255};


inline bool isPosEqual(TPosition a, TPosition b){return (a.x==b.x) && (a.y==b.y);}
inline bool isPosValid(TPosition p){return !isPosEqual(p, invalidPos);}

enum TDiplomaticStatus
{
    Undiscovered,   // a country that has never appeared in your visible area
    Neutral,        // default status of a newly discovered country
    Allied,          
    AtWar
};
enum TDiplomaticCommand
{
    KeepNeutral, 
    FormAlliance, 
    JustifyWar,
	Backstab
};

struct TMilitaryCommand
{
	TPosition place; //your goal place
	TMilitary bomb_size;
};

class BaseMap
{
public:
	BaseMap(){;}
	inline TMap getRows() const {return rows;}
	inline TMap getCols() const {return cols;}
	inline vector<vector<TMoney> > getMapRes() const {return MapResource_;}
	inline vector<vector<TMilitary> > getMapAtk() const {return MapAttackRatio_;}
	inline vector<vector<TMilitary> > getMapDef() const {return MapDefenseRatio_;}
protected:
	TMap	rows, cols;
	vector<vector<TMoney> >  MapResource_;
	vector<vector<TMilitary> > MapDefenseRatio_, MapAttackRatio_;
};


//CONSTANTS
const static TRound     MAX_ROUND = 50;//轮数限额
const static TMoney     UNIT_BOMB_COST = 1;
const static float		UNIT_CITY_INCOME = 1.0f;//低保的比例系数
const static TRound     WAR_JUSTIFY_TIME = 3;//从战争合理化到宣战的时间
const static float		CORRUPTION_COEF = 0.001f;//收获资源的腐败系数
const static int		MILITARY_KERNEL_SIZE = 5;//影响力核心的大小
const static float		MILITARY_KERNEL_SIGMA_2 = 2.25f;//影响力核心的高斯函数的系数sigma的平方
const static float		MILITARY_KERNEL_GAUSS_COEF = 100.0f;//影响力核心的高斯函数前面的系数
const static float		MILITARY_KERNEL_DELTA;//基础影响力
const static TMilitary	SUPPESS_LIMIT = 3;//压制上限，超过压制上限之后就会破城。“压制”这个词语灵感来自FPS。
const static TMoney     INITIAL_PLAYER_MONEY = 10;
const static TMoney     WAR_JUSTIFY_PRICE = 1;
const static int		FIELD_BOUNDARY = 2;//一块领土的视野范围


const static TId		NEUTRAL_PLAYER_ID = 233;
const static TId		UNKNOWN_PLAYER_ID = 213;
const static TId		PEKING_UNIVERSITY_ID = 0;


struct PlayerInfo
{    
	TDiplomaticStatus dipStatus;
	bool isVisible; // this player is visible to you ; you two share basic info.
	bool isUnion;   // this player has reached an alliance with you ; you two share all info.
	//basic info
	int mapArea; // area size of this player's land
	vector<TId> warList;//player at war
	//all info
	TMoney saving; // resource of this player
	TPosition capital;//capital
};

struct MapPointInfo
{
	TMask isVisible;
	TId owner;
	TMask isSieged;
};

struct Info
{
	TId id;
	TId playerSize;
	TMap rows;
	TMap cols;
	TRound round;

	BaseMap* map;

	vector<PlayerInfo> playerInfo;
	vector<vector<MapPointInfo> > mapPointInfo;
	vector<TMilitaryCommand> MilitaryCommandList;
	vector<TDiplomaticCommand> DiplomaticCommandList;
	TPosition newCapital;
};

#endif
