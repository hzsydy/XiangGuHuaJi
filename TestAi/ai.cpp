/* XiangGuHuaJi 2016, ai.cpp
 *
 */

#include "ai.h"

void player_ai(Info& info)
{
    TSaving mySaving = info.saving;
    
    for (TMapSize i=0; i<info.cols; ++i)
    {
        if (mySaving<1) break;
        for (TMapSize j=0; j<info.rows; ++j)
        {
            if (mySaving<1) break;
            if (info.ControlMask[i][j])
            { 
                info.MilitaryCommand[i][j] += 1;
                mySaving -= 1;
             }
        }
    }   

    for (TId i=0; i<info.PlayerSize; ++i)
    {
        if (info.Diplomatic[info.id][i] != War)
            info.DiplomaticCommandList[i] = AskForUnion;
        else
            info.DiplomaticCommandList[i] = ClaimWar;
    }

}
