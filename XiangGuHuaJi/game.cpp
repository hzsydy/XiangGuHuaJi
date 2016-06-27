/* XiangGuHuaJi 2016, game.cpp
 *
 */

#include"game.h"

namespace XGHJ 
{

Game::Game(Map& map, vector<Player>& players)
    : map(map), players(players), PlayerSize(players.size()),
    round(0), x(map.getX()), y(map.getY()),
    DiplomaticMap(newMatrix<DiplomaticStatus>(x, y))
{
}

Game::~Game()
{
    releaseMatrix<DiplomaticStatus>(DiplomaticMap, x, y);
}

bool Game::Run()
{
    vector<cv::Mat> MilitaryCommandList;
    vector<vector<DiplomaticCommand> > DiplomaticCommandMap;

    //0. 预处理 & 每一个玩家行动
    TId ** Ownership = newCopyMatrix<TId>(this->Ownership);
    vector<TMilitary**> MilitaryMap;  for (TId i=0; i<PlayerSize; ++i) MilitaryMap.push_back(newCopyMatrix<TMilitary>(this->MilitaryMap[i]));
    vector<TAttack**> AttckPointsMap; for (TId i=0; i<PlayerSize; ++i) AttckPointsMap.push_back(newCopyMatrix<TAttack>(this->AttackPointsMap[i]));
    TDefense** DefensePointsMap = newCopyMatrix<TDefense>(this->DefensePointsMap);

    for (TId id=0; id<PlayerSize; ++id)
    {
        cv::Mat MilitaryCommamd = cv::Mat(map.size(), TMatMilitary);
        vector<DiplomaticCommand> DiplomaticCommandList;

        players[id].Run(
            *this,
            this->map,
            Ownership,
            MilitaryMap,
            AttckPointsMap,
            DefensePointsMap,
            MilitaryCommamd,
            DiplomaticCommandList);

        MilitaryCommandList.push_back(MilitaryCommamd);
        DiplomaticCommandMap.push_back(DiplomaticCommandList);
    }

    releaseMatrix(Ownership, x, y);

    //1.建造 处理MilitaryMapList

    //2.外交 处理DiplomaticCommandMap

    //3.军事 更新DefensePoints,AttackPointsMap

    //4.经济 更新PlayerInfoList

    //5.胜负判定

    return true;    
}

}