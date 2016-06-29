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
    vector<TAttack> AttackPoints; //此点的攻防现状
};


//玩家获取的全部数据
class Info
{
public:
    Info(
        TId             id, 
        TRound          Round, 
        TMapSize        rows,
        TMapSize        cols, 
        TId             PlayerSize,
        vector<vector<unsigned char> >	OwnershipMask,
        vector<vector<unsigned char> >  VisibleMask,
        vector<vector<unsigned char> >  ControlMask,
        vector<vector<TId> >	        GlobalMap,
        vector<vector<TMapPara> >       MapResource,
        vector<vector<TMapPara> >       MapDefenseRatio,
        vector<vector<TMapPara> >       MapAttackRatio,
        vector<vector<vector<TMilitary > > >	MilitaryMap,
        vector<vector<vector<TAttack > > >	    AttackPointsMap,
        vector<vector<TDefense> >	            DefensePointsMap,
        vector<TPlayerInfo>	                    PlayerInfoList,
        vector<vector<TDiplomaticStatus> >	    Diplomatic,
        vector<vector<TMilitary> > & MilitaryCommand,
        vector<TDiplomaticCommand> & DiplomaticCommandList)
        :
        id(id),
        Round(Round),
        rows(rows), cols(cols),
        PlayerSize(PlayerSize),
        OwnershipMask(OwnershipMask),
        VisibleMask(VisibleMask),
        ControlMask(ControlMask),
        MapResource(MapResource),
        MapDefenseRatio(MapDefenseRatio),
        MapAttackRatio(MapAttackRatio),
        GlobalMap(GlobalMap),
        MilitaryMap(MilitaryMap),
        AttackPointsMap(AttackPointsMap),
        DefensePointsMap(DefensePointsMap),
        PlayerInfoList(PlayerInfoList),
        DiplomaticCommandList(DiplomaticCommandList),
        MilitaryCommand(MilitaryCommand),
        saving(PlayerInfoList[id].Saving)
    {
        for (TId i=0; i<PlayerSize; ++i) DiplomaticCommandList[i] = KeepNeutral;
    }
    
    TId       id;           //自己的ID
    TRound    Round;        //当前回合数
    TSaving   saving;       //当前库存
    TMapSize  rows, cols;         //地图的尺寸
    TId       PlayerSize;   //玩家数量
    
    vector<vector<unsigned char> >  OwnershipMask;  //你的领土
    vector<vector<unsigned char> >	VisibleMask;    //当前可见地区
    vector<vector<unsigned char> >	ControlMask;    //你可以放兵的地区

    vector<vector<TDiplomaticStatus> >	Diplomatic;
    vector<vector<TId> >	            GlobalMap;

    vector<vector<TMapPara> >	    MapResource, MapDefenseRatio, MapAttackRatio; //地图参数

    //获取一个点的信息
    PointMilitary getPointMilitary(int i, int j) 
    {
        PointMilitary point;
        
        if (VisibleMask[i][j]) point.Visible = true;
        else { point.Visible = false; return point; }

        point.Owner = GlobalMap[i][j];
        point.Union = Diplomatic[id][point.Owner] == Union;
        
        for (TId t=0; t<PlayerSize; ++t)
        {
            point.Military.push_back(MilitaryMap[t][i][j]);
            point.AttackPoints.push_back(AttackPointsMap[t][i][j]);
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
        switch (Diplomatic[id][targetId])
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
    vector<vector<vector<TMilitary > > >	MilitaryMap;
    vector<vector<vector<TAttack > > >	    AttackPointsMap;
    vector<vector<TDefense> >	            DefensePointsMap;

    vector<TPlayerInfo>	PlayerInfoList;

};


#endif