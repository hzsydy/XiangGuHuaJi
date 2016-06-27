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
{
    player_ai = NULL;

    HMODULE hDLL = LoadLibraryA(file_name.c_str());
    if (NULL != hDLL) player_ai = (TPlayerAi)GetProcAddress(hDLL, "player_ai");

    if (NULL != player_ai) 
        cout << "Player" << id << " has been loaded at "<< player_ai << endl;
    else
        cout << "FAILED TO LOAD Player"<< id << " !" << endl;
}

Player::~Player()
{
}

void Player::Run(Game& game, cv::Mat& MilitaryCommamd, DiplomaticCommand* DiplomaticCommandList)
{

}

}