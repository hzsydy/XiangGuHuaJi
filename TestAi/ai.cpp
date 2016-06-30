/* XiangGuHuaJi 2016, ai.cpp
 *
 */

#include "ai.h"

int cnt = 0;

void printBasicInfo(Info& info)
{    
    cout << "My id: " << (int)info.id << " ; Round: "<< info.Round << "; Saving: "<< info.saving << endl;
    cout << "MapSize: "<< (int)info.cols << " " << (int)info.rows << " PlayerSize: " << (int)info.PlayerSize << endl;
}

void checkMask(Info& info)
{
    TMapArea OwnershipArea = 0;
    TMapArea VisibleArea = 0;

    for (TMapSize i=0; i<info.cols; ++i)
        for (TMapSize j=0; j<info.rows; ++j)
        {
            if (info.OwnershipMask[i][j]) OwnershipArea+=1;
            if (info.VisibleMask[i][j]) VisibleArea+=1;
        }
    cout << "OwnershipArea: "<< OwnershipArea << " VisibleArea: "<<VisibleArea<<endl;
}

void executeBasicAi(Info& info)
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
        if (info.Diplomacy[info.id][i] != War)
            info.DiplomaticCommandList[i] = AskForUnion;
        else
            info.DiplomaticCommandList[i] = DeclareWar;
    }

}

void player_ai(Info& info)
{
    cout << endl << "It's info.hpp TEST here. <" << cnt++ << ">" << endl;

    printBasicInfo(info);
    checkMask(info);

    executeBasicAi(info);
}
