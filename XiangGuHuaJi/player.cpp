/* XiangGuHuaJi 2016, player.cpp
 *
 * Game <-- Player ---> AI
 *
 */

#include"player.h"

namespace XGHJ {

Player::Player()
{ 
}

Player::Player(string file_name, int id)
    : id(id), file_name(file_name)
{
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
#ifdef _MSC_VER
    __try
#endif
#ifdef __GNUC__
    try
#endif
    {
        player_ai(info);
    }
#ifdef _MSC_VER
    __except(EXCEPTION_EXECUTE_HANDLER)
#endif
#ifdef __GNUC__
    catch(...)
#endif
    {
        cout << "[ERROR] Player " <<  (int)id << " raised an exception in run()." <<  endl;
        //cout << e.what() << endl;
        Valid = false;
		return false;
    }
    return true;
}

bool Player::run(TMoney &price, BaseMap* map)
{
#ifdef _MSC_VER
    __try
#endif
#ifdef __GNUC__
    try
#endif
    {
        price = birthplacePrice(map);
        if (price>INITIAL_PLAYER_MONEY) price = INITIAL_PLAYER_MONEY;
    }
#ifdef _MSC_VER
    __except(EXCEPTION_EXECUTE_HANDLER)
#endif
#ifdef __GNUC__
    catch(...)
#endif
    {
        cout << "[ERROR] Player " <<  (int)id << " raised an exception in birthplacePrice()." <<  endl;
        //cout << e.what() << endl;
        Valid = false;
        return false;
    }
    return true;
}

bool Player::run(TPosition &pos, vector<TPosition> &posSelected, BaseMap* map)
{

//#ifdef _MSC_VER
//    __try
//#endif
//#ifdef __GNUC__
    try
//#endif
    {
        pos = birthplace(posSelected, map);
    }
//#ifdef _MSC_VER
//    __except(EXCEPTION_EXECUTE_HANDLER)
//#endif
//#ifdef __GNUC__
    catch(...)
//#endif
    {
        cout << "[ERROR] Player " <<  (int)id << " raised an exception in birthplace()." <<  endl;
        //cout << e.what() << endl;
        Valid = false;
        return false;
    }
    return true;
}

}
