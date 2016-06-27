/* XiangGuHuaJi 2016, definition.h
 * 
 */

#ifndef _XIANGGUHUAJI_DEFINITION_H__
#define _XIANGGUHUAJI_DEFINITION_H__

typedef unsigned char TId;       //玩家Id
typedef unsigned char TMapSize;  //地图尺寸(x,y)
typedef unsigned int  TMapArea;  //地图面积
typedef unsigned int  TRound;    //回合数

typedef unsigned char TMapPara;  //地图参数(MapResource, MapDefenseRatio, MapAttackRatio)
typedef unsigned int  TSaving;   //用户库存量

typedef unsigned char TDefense;  //防守点数(DefensePoints)
typedef unsigned char TAttack;   //攻击点数(AttackPoints)

typedef unsigned char TMilitary; //一个点放置的兵力
typedef unsigned int  TMilitarySummary; //玩家的总兵力

//外交关系
enum DiplomaticStatus
{
    Undiscovered, //未知国家
    Neutral, //中立
    Union,   //同盟
    War,     //战争
    StopWar  //停战
};
//外交指令
enum DiplomaticCommand
{
    KeepNeutral, //保持中立
    AskForUnion, //请求同盟
    ClaimWar     //宣战
};

//玩家信息
struct PlayerInfo
{    
    TId id; //玩家ID
    bool Visible; //辅助参考信息: 此玩家基本信息是否对你可见
    bool Union;   //辅助参考信息: 此玩家的完整信息是否对你可见（是否达成同盟）
//基本信息
    TMapArea MapArea; //领土总面积
    TMilitarySummary MilitarySummary; //已被放置的总兵力
//完整信息
    TSaving Saving; //库存金额
};


#endif