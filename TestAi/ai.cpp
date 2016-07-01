/* XiangGuHuaJi 2016, ai.cpp
 *
 */

#include "ai.h"

int cnt = 1;

void checkBasic(Info& info)
{    
    cout << "My id: " << (int)info.id << " ; Round: "<< info.Round << "; Saving: "<< info.saving << endl;
    cout << "MapSize: "<< (int)info.cols << " " << (int)info.rows << " PlayerSize: " << (int)info.PlayerSize << endl;
}
void checkMask(Info& info)
{
    TMapArea OwnershipArea = 0;
    TMapArea VisibleArea = 0;
    TMapArea ControlArea = 0;

    for (TMapSize i=0; i<info.cols; ++i)
        for (TMapSize j=0; j<info.rows; ++j)
        {
            if (info.OwnershipMask[i][j]) OwnershipArea+=1;
            if (info.VisibleMask[i][j]) VisibleArea+=1;
            if (info.ControlMask[i][j]) ControlArea+=1;
        }
    cout << "OwnershipArea: "<< OwnershipArea 
        << " VisibleArea: "<< VisibleArea
        << " ControlArea: "<< ControlArea <<  endl;

    vector<TMapArea> player_area(info.PlayerSize);
    for (TMapSize i=0; i<info.cols; ++i)
        for (TMapSize j=0; j<info.rows; ++j)
            if (info.GlobalMap[i][j] < info.PlayerSize)
                player_area[info.GlobalMap[i][j]]+=1;
    for (TId i=0; i<info.PlayerSize; ++i)
        cout << "player{" << (int)i << "}=" << player_area[i] << " ";
    cout<<endl;
}
void checkDiplomacy(Info& info)
{
    cout << "MyDiplomacy: ";
    for (TId i=0; i<info.PlayerSize; ++i)
        cout<<"["<<(int)i<<"]"<<(int)info.Diplomacy[info.id][i]<<" ";
    cout<<endl;
}
void checkMap(Info& info)
{
    TSaving saving = 0;
    for (TMapSize i=0; i<info.cols; ++i)
        for (TMapSize j=0; j<info.rows; ++j)
            if (info.OwnershipMask[i][j])
            {
                saving += info.MapResource[i][j];
            }
    saving += info.getPlayerInfo(info.id).MilitarySummary * info.UNIT_CITY_INCOME;
    cout<<"new_saving="<<saving<<endl;
}

void executeBasicAi(Info& info)
{    
    for (TMapSize i=0; i<info.cols; ++i)
        for (TMapSize j=0; j<info.rows; ++j) info.MilitaryCommand[i][j]=255;

    for (TId i=0; i<info.PlayerSize; ++i)
    {
        if (info.Diplomacy[info.id][i] != AtWar)
            info.DiplomaticCommandList[i] = FormAlliance;
        else
            info.DiplomaticCommandList[i] = DeclareWar;
    }

}

void player_ai(Info& info)
{
    cout << endl << "It's info.hpp TEST here. <" << cnt++ << ">" << endl;

    checkBasic(info);
    checkMask(info);
    checkDiplomacy(info);
    checkMap(info);

    executeBasicAi(info);
}
