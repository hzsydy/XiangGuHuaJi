/* XiangGuHuaJi 2016, info.hpp
 *
 */

#ifndef _XIANGGUHUAJI_INFO_H__
#define _XIANGGUHUAJI_INFO_H__

#include<iostream>
#include<vector>

#include "definition.h"

// All details that AI could reach

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
	TMilitary attackRatio, defenseRatio;
	TMoney resource;
	TMask isVisible;
	TId owner;
};

struct Info
{
	TId id;
	TId playerSize;
	TRound round;
	PlayerInfo (*getPlayerInfo)(TId id);
	MapPointInfo (*getMapPointInfo)(TPosition pos);
	vector<TMilitaryCommand> MilitaryCommandList;
	vector<TDiplomaticCommand> DiplomaticCommandList;
};
#endif
