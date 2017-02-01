/* XiangGuHuaJi 2016, ai.cpp
 * 
 * This is a test ai driven by keyboard input.
 * Last edited by sugar10w, 2017.02.
 */

#include "ai.h"

#include <cstdio>
#include <string>
using std::string;

string dipStatStr[] = {"Undiscovered", "Neutral", "Allied", "At War"};
string dipCommStr[] = {"Keep Neutral", "Form Alliance", "Justify War", "Backstab"};



int getNumber() {
    
    int cnt = 0;
    int ans = 0;
    bool gotcha = false;
    int sign = 1;
    char c;

    while (++cnt<32768) {

        c = getchar();

        if (c=='-') {
            gotcha = true;
            sign = -1;
            continue;
        }

        if ('0'<=c&&c<='9') {
            gotcha = true;
            ans = ans * 10 + c - '0';
            continue;
        }

        if (c=='#') {
            while ((c=getchar())!='\n') ;
            if (gotcha) return ans*sign;
            continue;
        }

        if (gotcha) return ans*sign;
        continue;
    }

    printf("[Error][GetNumber] input error.\n");
    printf("\n");
    return ans*sign;

}

TMoney birthplacePrice(void)
{
    printf("price for birthprice >>> ");
    return (TMoney)getNumber();
}

TPosition birthplace(vector<TPosition> posSelected)
{
    printf("Birthplaces choosed:");
    for (size_t i=0; i<posSelected.size(); i++)
    {
        printf("(%d, %d), ", posSelected[i].x, posSelected[i].y);
    }
    printf("\nyour birthplace >>> ");
    TPosition pos;
    pos.x=(TMap)getNumber();
    pos.y=(TMap)getNumber();
    printf("\nyou choose (%d, %d) \n", pos.x, pos.y);
    return pos;
}

void player_ai(Info& info)
{

    cout << endl << "[P"<<(int)info.id<<"]'s player_ai.";
    cout << endl << "Round <" << info.round << ">";
    cout << endl << "Saving=" << info.playerInfo[info.id].saving << endl;

    // display map
    cout << "your map:" << endl;
    for (TMap y=0; y<info.rows; y++)
    {
        for (TMap x=0; x<info.cols; x++)
        {
            MapPointInfo mpi = info.mapPointInfo[x][y];
            string owner;
            if(mpi.isVisible)
            {
                owner = (mpi.owner == NEUTRAL_PLAYER_ID) ? "-" : std::to_string((int)mpi.owner);
                if (mpi.isSieged) owner += "*";
            }
            else
            {
                owner = "?";
            }
            cout << std::left << std::setw(4) << std::setfill(' ') <<owner;
        }
        cout<<endl;
    }

    //diplomacy test
    cout << "DipStat: Undiscovered=0, Neutral=1, Allied=2, AtWar=3" << endl;
    cout << "DipComm: KeepNeutral=0, FormAlliance=1, JustifyWar=2, Backstab=3" << endl;
    for (TId id=0; id<info.playerSize; id++)
    {
        if (id != info.id)
        {
            cout << "your Dip. Status to P" << (int)id <<" :" << dipStatStr[info.playerInfo[id].dipStatus] << endl;
            cout << "your Dip. Command to P" << (int)id <<" >>> ";
            int input = getNumber();
            if (input>=0 && input<4)
            {
                info.DiplomaticCommandList[id] = (TDiplomaticCommand)input;
                cout << "you decided to " << dipCommStr[input] <<" to P" << (int)id << endl;
            }

        }
    }
    //military test
    
    cout << "enter your military command as \"x y cost\", enter -1 to exit >>> ";
    int inputList[3];
    int inputListCnt = 0;
    while (true)
    {
        int i = getNumber();
        if (i<0) break;

        inputList[inputListCnt] = i;
        inputListCnt++;
        if (inputListCnt == 3)
        {
            TMilitaryCommand tmc;
            tmc.place.x = inputList[0];
            tmc.place.y = inputList[1];
            tmc.bomb_size = inputList[2];
            inputListCnt = 0;
            info.MilitaryCommandList.push_back(tmc);

            printf("\nyou decided to attack (%d, %d) with bomb size %d\n", tmc.place.x, tmc.place.y, tmc.bomb_size);
        }
    }
    
    printf("\n");
    return;
}
