/* XiangGuHuaJi 2016, player.cpp
 *
 * Game <-- Player ---> AI
 *
 */

#include"player.h"

#ifdef _MSC_VER
#include"windows.h"
#endif
#ifdef __GNUC__
#include"dlfcn.h"
#endif

namespace XGHJ {

Player::Player()
{ 
}

Player::Player(string file_name, int id)
    : id(id), file_name(file_name)
{
    player_ai = NULL;

#ifdef _MSC_VER    
    HMODULE hDLL = LoadLibraryA(file_name.c_str());
    if (NULL != hDLL) player_ai = (TPlayerAi)GetProcAddress(hDLL, "player_ai");
#endif
#ifdef __GNUC__
    void* hDLL = dlopen(file_name.c_str(), RTLD_LAZY);
    if (NULL != hDLL) player_ai = (TPlayerAi)dlsym(hDLL, "player_ai");
#endif
    
    Valid = NULL != player_ai;
    
    if (NULL == hDLL) cout << "[ERROR] failed to load \"" << file_name << "\"" << endl;
                 else cout << "Succeed to load \"" << file_name << "\". ";

    if (isValid())
        cout << "Player{" << id << "} has been loaded. " << endl;
    else
        cout << "[ERROR] failed to load Player{"<< id << "} !" << endl;
}

Player::~Player()
{
}

bool Player::Run(Info &info)
{
    try
    {
        player_ai(info);
    }
    catch(exception e)
    {
        cout << "[ERROR] Player " << id << " raised an exception." <<  endl;
        cout << e.what() << endl;
        Valid = false;
		return false;
    }
    return true;
}

}
