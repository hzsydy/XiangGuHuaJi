/* XiangGuHuaJi 2016, ai.cpp
 *
 */

#include "ai.h"
#include "stdio.h"

TMoney birthplacePrice(void)
{
    printf("Your price for birthprice?");
    int price;
    scanf("%d", &price);
    return (TMoney)price;
}

TPosition birthplace(vector<TPosition> posSelected)
{
    printf("Birthplaces choosed:");
    for (size_t i=0; i<posSelected.size(); i++)
    {
        printf("(%d, %d), ", posSelected[i].x, posSelected[i].y);
    }
    printf("\n Now input your birthplace to choose\n");
    int x; 
    int y;
    scanf("%d %d", &x, &y);
    TPosition pos;
    pos.x=(TMap)x;
    pos.y=(TMap)y;
    return pos;
}

void player_ai(Info& info)
{
    cout << endl << "It's Player "<<(int)info.id<<" 's AI main function here. Round <" << info.round << ">" << endl;
    cout << "Saving left:" << info.playerInfo[info.id].saving << endl;
    //diplomacy status test
    for (TId id=0; id<info.playerSize; id++)
    {
        if (id != info.id)
        {
            cout << "dip status towards player " << (int)id <<":" << info.playerInfo[id].dipStatus << endl;
            cout << "your attitude towards player " << (int)id <<":"<<endl
                <<"(KeepNeutral=0, FormAlliance=1, JustifyWar=2, Backstab=3)" << endl;
            int input;
            scanf("%d", &input);
            if (input>=0 && input<4)
            {
                info.DiplomaticCommandList[id] = (TDiplomaticCommand)input;
            }
        }
    }
    return;
}
