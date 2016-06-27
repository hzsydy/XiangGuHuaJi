/* XiangGuHuaJi 2016, info.hpp
 *
 */

#ifndef _XIANGGUHUAJI_INFO_H__
#define _XIANGGUHUAJI_INFO_H__

#include<iostream>
#include<vector>

using namespace std;


typedef unsigned char TId;       //玩家Id
typedef unsigned char TMapSize;  //地图尺寸(x,y)
typedef unsigned int  TMapArea;  //地图面积
typedef unsigned int  TRound;    //回合数
typedef unsigned int  TPlayerSize; //玩家数量

typedef unsigned char TMapPara;  //地图参数(MapResource, MapDefenseRatio, MapAttackRatio)
typedef unsigned int  TSaving;   //用户库存量

typedef unsigned char TDefense;  //防守点数(DefensePoints)
typedef unsigned char TAttack;   //攻击点数(AttackPoints)

typedef unsigned char TMilitary; //一个点放置的兵力
typedef unsigned int  TMilitarySummary; //玩家的总兵力


//外交关系
enum DiplomaticStatus
{
    Unknown, //未知国家
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

//地图上某一个点的军事情况
struct PointMilitary
{    
    bool Visible;  //此点是否可见
    bool Union;    //此点是否为联盟(你是否可以在这个地上放兵)
    TId Owner;     //主权归属
 
    vector<TMilitary> Military; //此地的防御建筑现状(此地的同盟玩家们分别在这里放置了多少兵)

    TDefense DefensePoints;        //此点的攻防现状
    vector<TAttack> AttackSummary; //此点的攻防现状
};


//玩家获取的全部数据
class Info
{
public:
    Info(
        TId id, TRound round, TMapSize x, TMapSize y, TId PlayerSize,
        unsigned char **  mask,
        unsigned char **  control,
        TMapPara **  MapResource,
        TMapPara **  MapDefenseRatio,
        TMapPara **  MapAttackRatio,
        TId **       Ownership,
        TMilitary *** MilitaryMap,
        TAttack *** AttackPointsMap,
        TDefense ** DefensePointsMap,
        PlayerInfo * PlayerList,
        DiplomaticStatus ** DiplomaticMap)
        : id(id), round(round), x(x), y(y), PlayerSize(PlayerSize), saving(PlayerList[id].Saving),
        mask(mask), control(control), DiplomaticMap(DiplomaticMap), Ownership(Ownership),
        MapResource(MapResource), MapDefenseRatio(MapDefenseRatio), MapAttackRatio(MapAttackRatio),
        MilitaryMap(MilitaryMap), AttackPointsMap(AttackPointsMap), DefensePointsMap(DefensePointsMap),
        PlayerList(PlayerList)
    {
        for (TId i=0; i<PlayerSize; ++i) DiplomaticCommandList[i] = KeepNeutral;
    }
    
    const TId       id;    //自己的ID
    const TRound    round; //当前回合数
    const TSaving   saving;//当前库存
    const TMapSize  x, y;  //地图的尺寸
    const TId       PlayerSize; //玩家数量
    
    const unsigned char*const*const     mask;    //当前可见地区
    const unsigned char*const*const     control; //你可以放兵的地区

    const DiplomaticStatus*const*const  DiplomaticMap; //全体外交状态
    const TId*const*const Ownership;            //地图主权
    const TMapPara*const*const MapResource;     //地图资源参数
    const TMapPara*const*const MapAttackRatio;  //地图攻击参数
    const TMapPara*const*const MapDefenseRatio; //地图防御参数

    //获取一个点的信息
    PointMilitary getPointMilitary(int i, int j) 
    {
        PointMilitary point;
        
        if (mask[i][j]) point.Visible = true;
        else { point.Visible = false; return point; }

        point.Owner = Ownership[i][j];
        point.Union = DiplomaticMap[id][point.Owner] == Union;
        
        for (TId t=0; t<PlayerSize; ++t)
        {
            point.Military.push_back(MilitaryMap[i][j][t]);
            point.AttackSummary.push_back(AttackPointsMap[i][j][t]);
        }
        point.DefensePoints = DefensePointsMap[i][j];

        return point;
    }

    //获取一个玩家的信息
    PlayerInfo getPlayerInfo(TId targetId)
    {
        PlayerInfo player;
        player.id = -1;
        if (targetId<0 || targetId>=PlayerSize) return player;
        
        player = PlayerList[targetId];
        switch (DiplomaticMap[id][targetId])
        {
        case Unknown:
        case War:
            player.Visible = player.Union  = false;
            player.Saving = player.MapArea = player.MilitarySummary = 0;
            return player;
        case Neutral:
            player.Visible = true;
            player.Union = false;
            player.Saving = 0; 
            return player;
        case Union:
            player.Visible = player.Union  = false;
            return player;
        default:
            return player;
        }

    }

//可供写的信息
    vector<DiplomaticCommand> DiplomaticCommandList;
    vector<vector<unsigned char> > MilitaryCommand;

private:
    const TMilitary*const*const*const MilitaryMap;
    const TAttack*const*const*const   AttackPointsMap;
    const TDefense*const*const        DefensePointsMap;
    const PlayerInfo*const            PlayerList;
};


#endif