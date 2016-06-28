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

    HMODULE hDLL = LoadLibraryA(file_name.c_str());
    if (NULL != hDLL) player_ai = (TPlayerAi)GetProcAddress(hDLL, "player_ai");

    Valid = NULL != player_ai;

    if (isValid())
        cout << "Player" << id << " has been loaded at "<< player_ai << endl;
    else
        cout << "ERROR: FAILED TO LOAD Player"<< id << " !" << endl;
}

Player::~Player()
{
}

//void Player::Run(
//    Game&               game,
//    Map&                map,
//    TId **              Ownership,
//    vector<TMilitary**> MilitaryMap,
//    vector<TAttack**>   AttckPointsMap,
//    TDefense**          DefensePointsMap,
//    cv::Mat&            MilitaryCommamd,
//    vector<TDiplomaticCommand>& DiplomaticCommandList)
//{
//    if (!Valid) return;
//
//    // convert Game to Info 
//
//    // generate mask, control 
//    TMapSize x = map.getX(), y = map.getY();
//
//    unsigned char ** mask =  newMatrix<unsigned char>(x,y);
//    unsigned char ** control = newMatrix<unsigned char>(x,y);
//
//    vector<vector<TMilitary> > VMilitaryCommand;
//
//    for (TMapSize i=0; i<x; ++i)
//        for (TMapSize j=0; j<y; ++j)
//        {
//            TId owner = Ownership[i][j]; 
//            mask[i][j] = (owner == id) ? 255 : 0;
//            control[i][j] = (game.DiplomaticMap[id][owner] == Union) ? 255 : 0;
//        }
//    Info info(
//        id,
//        game.round,
//        x, y, 
//        game.PlayerSize,
//        mask, control, 
//        map.getMMapResource(), map.getMMapDefenseRatio(), map.getMMapAttackRatio(),
//        Ownership,
//        MilitaryMap, AttckPointsMap, DefensePointsMap,
//        game.PlayerInfoList, 
//        game.DiplomaticMap,
//        DiplomaticCommandList,
//        VMilitaryCommand);
//
//    releaseMatrix<unsigned char>(mask, x,y);
//    releaseMatrix<unsigned char>(control, x,y);
//
//    // translate 
//}

}