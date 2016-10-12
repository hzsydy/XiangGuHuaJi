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
    for (int i=0; i<posSelected.size(); i++)
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
    cout << endl << "It's info.hpp TEST here. Round <" << info.round << ">" << endl;
    return;
}
