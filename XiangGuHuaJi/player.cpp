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
        cout << "Player{" << id << "} has been loaded. " << endl;
    else
        cout << "[ERROR] failed to load Player{"<< id << "} !" << endl;

    return Valid;
}

bool Player::run(Info &info)
{
    try
    {
        player_ai(info);
    }
    catch(exception e)
    {
        cout << "[ERROR] Player " << id << " raised an exception in run()." <<  endl;
        cout << e.what() << endl;
        Valid = false;
		return false;
    }
    return true;
}

bool Player::run(TMoney &price)
{
    try
    {
        price = birthplacePrice();
    }
    catch(exception e)
    {
        cout << "[ERROR] Player " << id << " raised an exception in birthplacePrice()." <<  endl;
        cout << e.what() << endl;
        Valid = false;
        return false;
    }
    return true;
}

bool Player::run(TPosition &pos, vector<TPosition> &posSelected)
{
    try
    {
        pos = birthplace(posSelected);
    }
    catch(exception e)
    {
        cout << "[ERROR] Player " << id << " raised an exception in birthplace()." <<  endl;
        cout << e.what() << endl;
        Valid = false;
        return false;
    }
    return true;
}

}
