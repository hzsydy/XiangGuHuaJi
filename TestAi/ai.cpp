/* XiangGuHuaJi 2016, ai.cpp
 *
 */

#include "ai.h"


void player_ai(Info& info)
{
    TSaving mySaving = info.saving;
    
    for (TMapSize i=0; i<info.x; ++i)
    {
        if (mySaving<1) break;
        for (TMapSize j=0; j<info.y; ++j)
        {
            if (mySaving<1) break;
            if (info.control[i][j])
            { 
                info.MilitaryCommand[i][j] += 1;
                mySaving -= 1;
            }
        }
    }   

    for (TId i=0; i<info.PlayerSize; ++i)
    {
        if (info.DiplomaticMap[info.id][i] != DiplomaticStatus::War)
            info.DiplomaticCommandList[i] = DiplomaticCommand::AskForUnion;
        else
            info.DiplomaticCommandList[i] = DiplomaticCommand::ClaimWar;
    }

}