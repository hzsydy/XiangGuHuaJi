/* XiangGuHuaJi 2016, player.cpp
 *
 * Game <-- Player ---> AI
 *
 */

#include <ctime>
#include"player.h"

namespace XGHJ {

Player::Player()
{ 
}

Player::Player(string file_name, int id)
    : id(id), file_name(file_name)
{
    name = file_name;
    if (name.rfind('/') != string::npos) {
        name = name.substr(name.rfind('/')+1);
    }
    if (name.rfind('\\') != string::npos) {
        name = name.substr(name.rfind('\\')+1);
    }

    player_ai = NULL;
    birthplacePrice = NULL;
    birthplace = NULL;
    hDLL = NULL;
    load();
}

Player::~Player()
{
}

bool Player::load()
{ 
    if (NULL != hDLL) 
    {
        __FUCK_CLOSEDLL(hDLL);
        player_ai = NULL;
        birthplacePrice = NULL;
        birthplace = NULL;
    }
    hDLL = __FUCK_LOADDLL(file_name.c_str());
    if (NULL != hDLL)
    {
        player_ai = (TPlayerAi)__FUCK_GETFUNC(hDLL, "player_ai");
        birthplacePrice = (TBirthplacePrice)__FUCK_GETFUNC(hDLL, "birthplacePrice");
        birthplace = (TBirthplace)__FUCK_GETFUNC(hDLL, "birthplace");
    }

    Valid = true;
    Valid = Valid && (NULL != player_ai);
    Valid = Valid && (NULL != birthplacePrice);
    Valid = Valid && (NULL != birthplace);

    if (NULL == hDLL) 
        cout << "[ERROR] failed to load \"" << file_name << "\"" << endl;
    else 
        cout << "Succeed to load \"" << file_name << "\". ";
    if (isValid())
        cout << "Player{" <<  (int)id << "} has been loaded. " << endl;
    else
        cout << "[ERROR] failed to load Player{"<<  (int)id << "} !" << endl;

    return Valid;
}

bool Player::run(Info &info)
{
    int time_a = GetTickCount();
    int time_b;

#if (!defined _MSC_VER) || (defined _DEBUG)
    try {
        player_ai(info);
    }
    catch(exception e) {
        cout << "[ERROR] Player " <<  (int)id << " raised an exception in run()." <<  endl;
        cout << e.what() << endl; 
        kill(); return false;
    }
#else 
    __try {
        player_ai(info);
    }
    __except(EXCEPTION_EXECUTE_HANDLER) {
        cout << "[ERROR] Player " <<  (int)id << " raised an exception in run()." <<  endl;
        kill(); return false;
    }
#endif

    time_b = GetTickCount();
    if (time_b - time_a > 2000) kill();

    return true;

}

bool Player::run(TMoney &price, BaseMap* map)
{
#if (!defined _MSC_VER) || (defined _DEBUG)
    try {
        price = birthplacePrice(map);
        if (price>INITIAL_PLAYER_MONEY) price = INITIAL_PLAYER_MONEY;
    }
    catch(exception e) {
        cout << "[ERROR] Player " <<  (int)id << " raised an exception in birthplacePrice()." <<  endl;
        cout << e.what() << endl;
        kill(); return false;
    }
#else 
    __try {
        price = birthplacePrice(map);
        if (price>INITIAL_PLAYER_MONEY) price = INITIAL_PLAYER_MONEY;
    }
    __except(EXCEPTION_EXECUTE_HANDLER) {
        cout << "[ERROR] Player " <<  (int)id << " raised an exception in birthplacePrice()." <<  endl;
        kill(); return false;
    }
#endif

    return true;
}

bool Player::run(TPosition &pos, vector<TPosition> &posSelected, BaseMap* map)
{

#if (!defined _MSC_VER) || (defined _DEBUG)
    try {
        pos = birthplace(posSelected, map);
    }
    catch(exception e) {
        cout << "[ERROR] Player " <<  (int)id << " raised an exception in birthplace()." <<  endl;
        cout << e.what() << endl;
        kill(); return false;
    }
#else
    __try {
        pos = birthplace(posSelected, map);
    }
    __except(EXCEPTION_EXECUTE_HANDLER) {
        cout << "[ERROR] Player " <<  (int)id << " raised an exception in birthplace()." <<  endl;
        kill(); return false;
    }
#endif

    return true;
}

}
