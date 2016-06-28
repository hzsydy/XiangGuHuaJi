/* XiangGuHuaJi 2016, info.hpp
 *
 */

#ifndef _XIANGGUHUAJI_INFO_H__
#define _XIANGGUHUAJI_INFO_H__

#include<iostream>
#include<vector>

#include "definition.h"

using namespace std;

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
        const TId id, 
        const TRound round, 
        const TMapSize x, const TMapSize y, 
        const TId PlayerSize,
        unsigned char **  mask,
        unsigned char **  control,
        TMapPara **  MapResource,
        TMapPara **  MapDefenseRatio,
        TMapPara **  MapAttackRatio,
        TId **       Ownership,
        vector<TMilitary**> MilitaryMap,
        vector<TAttack**> AttackPointsMap,
        TDefense ** DefensePointsMap,
        vector<TPlayerInfo> PlayerInfoList,
        TDiplomaticStatus ** DiplomaticMap,
        vector<TDiplomaticCommand> & DiplomaticCommandList,
        vector<vector<TMilitary> > & MilitaryCommand)
        :
        id(id), round(round), x(x), y(y), PlayerSize(PlayerSize), saving(PlayerInfoList[id].Saving),
        mask(mask), control(control), DiplomaticMap(DiplomaticMap), Ownership(Ownership),
        MapResource(MapResource), MapDefenseRatio(MapDefenseRatio), MapAttackRatio(MapAttackRatio),
        MilitaryMap(MilitaryMap), AttackPointsMap(AttackPointsMap), DefensePointsMap(DefensePointsMap),
        PlayerInfoList(PlayerInfoList),
        DiplomaticCommandList(DiplomaticCommandList), MilitaryCommand(MilitaryCommand)
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

    const TDiplomaticStatus*const*const DiplomaticMap; //全体外交状态
    const TId*const*const      Ownership;             //地图主权
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
            point.Military.push_back(MilitaryMap[t][i][j]);
            point.AttackSummary.push_back(AttackPointsMap[t][i][j]);
        }
        point.DefensePoints = DefensePointsMap[i][j];

        return point;
    }
    //获取一个玩家的信息
    TPlayerInfo getPlayerInfo(TId targetId)
    {
        TPlayerInfo player;
        player.id = -1;
        if (targetId<0 || targetId>=PlayerSize) return player;
        
        player = PlayerInfoList[targetId];
        switch (DiplomaticMap[id][targetId])
        {
        case Undiscovered:
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

    vector<TDiplomaticCommand> & DiplomaticCommandList;
    vector<vector<TMilitary> > & MilitaryCommand;

private:
//受到游戏规则限制的信息
    vector<TMilitary**>   MilitaryMap;
    vector<TAttack**>     AttackPointsMap;
    TDefense**            DefensePointsMap;
    vector<TPlayerInfo>    PlayerInfoList;
};


#endif