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

#include <windows.h> 
enum Color_t {Gray, White ,DarkRed,Red,DarkBlue, Blue, DarkGreen,Green,DarkCyan,Cyan,DarkMagenta,Magenta,DarkYellow,Yellow, None};

Color_t playerColor[] = {Red, Blue, Green, Yellow};

void SetColor(Color_t Fore)
{
    WORD fore;

    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    switch(Fore)
    {
    case 0:
        fore = FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN;
        break;
    case 1:
        fore = FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_INTENSITY;
        break;
    case 2:
        fore = FOREGROUND_RED;
        break;
    case 3:
        fore = FOREGROUND_RED|FOREGROUND_INTENSITY;
        break;
    case 4:
        fore = FOREGROUND_BLUE;
        break;
    case 5:
        fore = FOREGROUND_BLUE|FOREGROUND_INTENSITY;
        break;
    case 6:
        fore = FOREGROUND_GREEN;
        break;
    case 7:
        fore = FOREGROUND_GREEN|FOREGROUND_INTENSITY;
        break;
    case 8:
        fore = FOREGROUND_BLUE|FOREGROUND_GREEN;
        break;
    case 9:
        fore = FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_INTENSITY;
        break;
    case 10:
        fore =  FOREGROUND_BLUE|FOREGROUND_RED;
        break;
    case 11:
        fore = FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_INTENSITY;
        break;
    case 12:
        fore = FOREGROUND_RED|FOREGROUND_GREEN;
        break;
    case 13:
        fore = FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY;
        break;
    }

    SetConsoleTextAttribute(hStdout,fore);
}

int getNumber() {
    
    int cnt = 0;
    int ans = 0;
    bool gotcha = false;
    int sign = 1;
    char c;

    while (++cnt<32768) {

        c = getchar();

        if (c == EOF)
        {
            std::cerr<<"End of file reach!"<<endl;
            while (1)
            {
                ;
            }
        }

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
    SetColor(Gray);
    printf("price for birthprice >>> ");
    SetColor(White);
    TMoney t = getNumber();
    SetColor(Gray);
    return t;
}

TPosition birthplace(vector<TPosition> posSelected)
{
    SetColor(Gray);
    printf("Birthplaces choosed:");
    for (size_t i=0; i<posSelected.size(); i++)
    {
        printf("(%d, %d), ", posSelected[i].x, posSelected[i].y);
    }
    printf("\nyour birthplace >>> ");
    TPosition pos;
    SetColor(White);
    pos.x=(TMap)getNumber();
    pos.y=(TMap)getNumber();
    SetColor(Gray);
    printf("\nyou choose (%d, %d) \n", pos.x, pos.y);
    return pos;
}

void player_ai(Info& info)
{
    SetColor(playerColor[info.id%4]);
    cout << endl << "[P"<<(int)info.id<<"]'s player_ai.";
    cout << endl << "Round <" << info.round << ">";
    cout << endl << "Saving=" << info.playerInfo[info.id].saving << endl;

    // display map
    SetColor(Gray);
    cout << "your map:" << endl;
    cout << std::left << std::setw(4) << std::setfill(' ') <<' ';
    for (TMap x=0; x<info.cols; x++)
    {
        cout << std::left << std::setw(4) << std::setfill(' ') <<(int)x;
    }
    cout<<endl;
    for (TMap y=0; y<info.rows; y++)
    {
        SetColor(Gray);
        cout << std::left << std::setw(4) << std::setfill(' ') <<(int)y;

        for (TMap x=0; x<info.cols; x++)
        {
            MapPointInfo mpi = info.mapPointInfo[x][y];
            string owner;
            if(mpi.isVisible)
            {
                if (mpi.owner == NEUTRAL_PLAYER_ID)
                {
                    SetColor(Gray);
                    owner = "-";
                }
                else
                {
                    int o = (int)mpi.owner;
                    SetColor(playerColor[o%4]);
                    owner = std::to_string(o);
                }
                if (mpi.isSieged) owner += "*";
            }
            else
            {
                SetColor(Gray);
                owner = "?";
            }
            cout << std::left << std::setw(4) << std::setfill(' ') <<owner;
        }
        cout<<endl;
    }

    SetColor(Gray);

    //diplomacy test
    cout << "DipStat: Undiscovered=0, Neutral=1, Allied=2, AtWar=3" << endl;
    cout << "DipComm: KeepNeutral=0, FormAlliance=1, JustifyWar=2, Backstab=3" << endl;
    for (TId id=0; id<info.playerSize; id++)
    {
        if (id != info.id)
        {
            SetColor(Gray);
            cout << "your Dip. Status to P" << (int)id <<" :" << dipStatStr[info.playerInfo[id].dipStatus] << endl;
            cout << "your Dip. Command to P" << (int)id <<" >>> ";

            SetColor(playerColor[info.id%4]);
            int input = getNumber();
            SetColor(Gray);

            if (input>=0 && input<4)
            {
                info.DiplomaticCommandList[id] = (TDiplomaticCommand)input;
                cout << "you decided to " << dipCommStr[input] <<" to P" << (int)id << endl;
            }

        }
    }
    //military test
    
    SetColor(Gray);
    cout << "enter your military command as \"x y cost\", enter -1 to exit >>> ";
    int inputList[3];
    int inputListCnt = 0;
    while (true)
    {
        SetColor(playerColor[info.id%4]);
        int i = getNumber();
        SetColor(Gray);

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
    
    SetColor(Gray);
    cout << "your new capital >>> ";
    SetColor(playerColor[info.id%4]);
    info.newCapital.x = getNumber();
    info.newCapital.y = getNumber();
    SetColor(Gray);
    printf("your new capital : (%d, %d)\n", info.newCapital.x, info.newCapital.y);

    printf("\n");
    return;
}
