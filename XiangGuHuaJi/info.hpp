/* XiangGuHuaJi 2016, info.hpp
 *
 */

#ifndef _XIANGGUHUAJI_INFO_H__
#define _XIANGGUHUAJI_INFO_H__

#include<iostream>
#include<vector>

#include "definition.h"

using namespace std;

// Military details about a point
struct PointMilitary
{    
    bool Visible;  
    bool Union;    
    TId Owner;   
 
    vector<TMilitary> Military; // everyone's Military on this point

    TDefense DefensePoints;       // DefensePoints currently
    vector<TAttack> AttackPoints; // everyone's AttackPoints to this point
};


// All details that AI could reach 
class Info
{
public:
    Info(
        TId             id, 
        TRound          Round, 
        TMapSize        rows,
        TMapSize        cols, 
        TId             PlayerSize,
        vector<vector<TMask> >	OwnershipMask,
        vector<vector<TMask> >  VisibleMask,
        vector<vector<TMask> >  ControlMask,
        vector<vector<TId> >	        GlobalMap,
        vector<vector<TMapPara> >       MapResource,
        vector<vector<TMapPara> >       MapDefenseRatio,
        vector<vector<TMapPara> >       MapAttackRatio,
        vector<vector<vector<TMilitary > > >	MilitaryMap,
        vector<vector<vector<TAttack > > >	    AttackPointsMap,
        vector<vector<TDefense> >	            DefensePointsMap,
        vector<TPlayerInfo>	                    PlayerInfoList,
        vector<vector<TDiplomaticStatus> >	    Diplomacy,
        vector<vector<TMilitary> > MilitaryCommand,
        vector<TDiplomaticCommand> DiplomaticCommandList)
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
        Diplomacy(Diplomacy),
        DiplomaticCommandList(DiplomaticCommandList),
        MilitaryCommand(MilitaryCommand),
        saving(PlayerInfoList[id].Saving)
    {
        for (TId i=0; i<PlayerSize; ++i) DiplomaticCommandList[i] = KeepNeutral;
    }
    
    const TId       id;           // your id Number
    const TRound    Round;        // the round currently
    const TSaving   saving;       // your resource gathered
    const TMapSize  rows, cols;   // map
    const TId       PlayerSize;   // playersize [Caution] Do not raise OutOfRange Exception 
    
    const vector<vector<TMask> >  OwnershipMask;  // your land
    const vector<vector<TMask> >	VisibleMask;    // visible area
    const vector<vector<TMask> >	ControlMask;    // your and your allian's land

    const vector<vector<TDiplomaticStatus> >	Diplomacy;
    const vector<vector<TId> >	            GlobalMap;

    const vector<vector<TMapPara> >	        MapResource, MapDefenseRatio, MapAttackRatio; // map
    
    const PointMilitary                       getPointMilitary(int i, int j) 
    {
        PointMilitary point;
        
        if (VisibleMask[i][j]) point.Visible = true;
        else { point.Visible = false; return point; }

        point.Owner = GlobalMap[i][j];
        point.Union = Diplomacy[id][point.Owner] == Allied;
        
        for (TId t=0; t<PlayerSize; ++t)
        {
            point.Military.push_back(MilitaryMap[t][i][j]);
            point.AttackPoints.push_back(AttackPointsMap[t][i][j]);
        }
        point.DefensePoints = DefensePointsMap[i][j];

        return point;
    }
    
    const TPlayerInfo                         getPlayerInfo(TId targetId)
    {
        TPlayerInfo player;
        player.id = -1;
        if (targetId<0 || targetId>=PlayerSize) return player;
        
        player = PlayerInfoList[targetId];
        switch (Diplomacy[id][targetId])
        {
        case Undiscovered:
        case AtWar:
            player.Visible = player.Union  = false;
            player.Saving = player.MapArea = player.MilitarySummary = 0;
            return player;
        case Neutral:
            player.Visible = true;
            player.Union = false;
            player.Saving = 0; 
            return player;
        case Allied:
            player.Visible = player.Union  = false;
            return player;
        default:
            return player;
        }
    }

    vector<TDiplomaticCommand>        DiplomaticCommandList;
    vector<vector<TMilitary> >        MilitaryCommand;

private:
// info restricted by the game
    const vector<vector<vector<TMilitary > > >	MilitaryMap;
    const vector<vector<vector<TAttack > > >	    AttackPointsMap;
    const vector<vector<TDefense> >	            DefensePointsMap;

    const vector<TPlayerInfo>	                PlayerInfoList;

};


#endif
