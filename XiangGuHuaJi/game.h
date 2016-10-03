/* XiangGuHuaJi 2016, game.h
 * 
 */

#ifndef _XIANGGUHUAJI_GAME_H__
#define _XIANGGUHUAJI_GAME_H__


#include <string>
#include <cmath>
#include <vector>

#include "definition.h"
#include "map.h"


using std::vector;

namespace XGHJ {
class Game
{
public:
    Game(Map& map, int playersize);
    ~Game();

    bool Start();
	bool Run(vector<vector<TMilitaryCommand> > & MilitaryCommandMap,
		vector<vector<TDiplomaticCommand> > & DiplomaticCommandMap);
    bool DiplomacyPhase(vector<vector<TDiplomaticCommand> > & DiplomaticCommandMap);
    bool MilitaryPhase(vector<vector<TMilitaryCommand> > & MilitaryCommandList);
    bool ProducingPhase();
    bool CheckWinner();

    Map& map;  

	PlayerInfo getPlayerInfo(TId id) const;
	MapPointInfo getMapPointInfo(TPosition pos) const;
	Info generateInfo(TId id) const;

	inline TRound getRound() {return round ;}
	inline TId getPlayerSize() {return playerSize ;}
	bool                isValid;
protected:
	TMap				rows,cols;
	TId                 playerSize;
	//需要保存到下回合的中间变量
	TRound              round;
	vector<vector<TId> > globalMap;//ownership of the lands
	vector<vector<bool> > isSieged;
	vector<TPosition>	playerCapital;
	vector<TMoney>		playerSaving;
	vector<int>			playerArea;
	vector<vector<TDiplomaticStatus> >	diplomacy;
	//你们还需要一个变量来实现justify war，我就懒的写了@pierre
private:
	//供军事部分使用的高斯核
	vector<vector<float> > MilitaryKernel;
	//你们自己添加的小函数请写在这里
};

}


#endif
