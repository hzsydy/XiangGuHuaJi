/* XiangGuHuaJi 2016, definition.h
 * 
 */

#ifndef _XIANGGUHUAJI_DEFINITION_H__
#define _XIANGGUHUAJI_DEFINITION_H__


#include "debug.hpp"

typedef unsigned char TId;       //PlayerId
#define CV_TId CV_8UC1
typedef unsigned char TMapSize;  //x,y
typedef unsigned int  TMapArea;  //Land area
typedef unsigned int  TRound;    //round 

typedef unsigned char TMapPara;  //MapResource, MapDefenseRatio, MapAttackRatio
#define CV_TMapPara CV_8UC1
typedef unsigned int  TSaving;   //saving of a player

typedef unsigned char TMask;
#define CV_TMask CV_8UC1

typedef unsigned char TDefense;  //DefensePoints
#define CV_TDefense CV_8UC1
typedef unsigned char TAttack;   //AttackPoints
#define CV_TAttack CV_8UC1

typedef unsigned char TMilitary; //Military(Building)
#define CV_TMilitary CV_8UC1
typedef unsigned int  TMilitarySummary; //all military of a player

enum TDiplomaticStatus
{
    Undiscovered,   // a country that has never appeared in your visible area
    Neutral,        // default status of a newly discovered country
    Allied,          
    AtWar,     
    AtTruce         // Neutral but cannont DeclareWar
};
enum TDiplomaticCommand
{
    KeepNeutral, 
    FormAlliance, 
    DeclareWar    
};

struct TPlayerInfo
{    
    TId id; 
    bool Visible; // this player is visible to you ; you two share basic info.
    bool Union;   // this player has reached an alliance with you ; you two share all info.
//basic info
    TMapArea MapArea; // area of all this player's land
    TMilitarySummary MilitarySummary; // all this player's active Military
//all info
    TSaving Saving; // resource of this player
};


#endif
