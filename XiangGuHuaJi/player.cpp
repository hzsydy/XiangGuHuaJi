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

    if (isValid())
        cout << "Player{" << id << "} has been loaded at "<< player_ai << endl;
    else
        cout << "[ERROR] failed to load Player{"<< id << "} !" << endl;
}

Player::~Player()
{
}

void Player::Run(
    Game&                       game,
    cv::Mat&                    MilitaryCommand_,
    vector<TDiplomaticCommand>& DiplomaticCommandList)
{
    if (!isValid()) return;

    vector<vector<TMilitary> >      MilitaryCommand;
    vector<vector<unsigned char> >	OwnershipMask, VisibleMask, ControlMask;
    vector<vector<TId> >	        GlobalMap;

    Info info(
        id,
        game.Round,
        game.map.x,
        game.map.y,
        game.PlayerSize,
        OwnershipMask,
        VisibleMask,
        ControlMask,
        GlobalMap,
        game.map.MapResource,
        game.map.MapDefenseRatio,
        game.map.MapAttackRatio,
        game.MilitaryMap,
        game.AttackPointsMap,
        game.DefensePointsMap,
        game.PlayerInfoList,
        game.Diplomatic,
        MilitaryCommand,
        DiplomaticCommandList
        );

    try
    {
        player_ai(info);
        convertVector<TMilitary>(MilitaryCommand, MilitaryCommand_);
    }
    catch(...)
    {
        cout << "[ERROR] Player " << id << " raised an exception." <<  endl;
        Valid = false;
    }
    
}

}
