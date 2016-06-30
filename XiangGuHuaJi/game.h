/* XiangGuHuaJi 2016, game.h
 * 
 */

#ifndef _XIANGGUHUAJI_GAME_H__
#define _XIANGGUHUAJI_GAME_H__


#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<opencv2/opencv.hpp>

#include"definition.h"
#include"map.h"


namespace XGHJ {

using namespace std;

class Player;

class Game
{
public:
    Game(Map& map, int playersize);
    ~Game();

	bool Run(vector<cv::Mat/*TMatMilitary*/> & MilitaryCommandList,
		vector<vector<TDiplomaticCommand> > & DiplomaticCommandMap);
    //bool Run_0();
    bool Run_1(vector<cv::Mat/*TMatMilitary*/> &       MilitaryCommandList,
               vector<vector<TDiplomaticCommand> > &   DiplomaticCommandMap);
    bool Run_2(vector<cv::Mat/*TMatMilitary*/> &       MilitaryCommandList);
    bool Run_3(vector<vector<TDiplomaticCommand> > &   DiplomaticCommandMap);
    bool Run_4(vector<cv::Mat/*TMatMilitary*/> &       MilitaryCommandList);
    bool Run_5();
    bool Run_6();

	bool Start();

    Map&                map;
	const static TRound MAX_ROUND;

	//first_class statistics
	inline vector<cv::Mat>		getMilitaryMap()	 {return  MilitaryMap_		;}
	inline cv::Mat/*CV_TId*/	getGlobalMap()		 {return  GlobalMap_		;}
	inline vector<TPlayerInfo>	getPlayerInfoList()	 {return  PlayerInfoList_	;}
	inline vector<vector<TDiplomaticStatus> >									
								getDiplomacy()		 {return  Diplomacy_		;}
	inline TRound				getRound()			 {return  Round  			;}
	inline TId					getPlayerSize() 	 {return  PlayerSize		;}
	inline bool					isValid() 			 {return  isValid_      	;}
	//second_class statistics resolved											
	inline vector<cv::Mat>		getOwnershipMasks()	 {return  OwnershipMasks_	;}
	inline vector<cv::Mat>		getAttackPointsMap() {return  AttackPointsMap_	;}
	inline cv::Mat				getDefensePointsMap(){return  DefensePointsMap_	;}
protected:
	//first_class statistics
	vector<cv::Mat>     MilitaryMap_;
	cv::Mat/*CV_TId*/	GlobalMap_;
	vector<TPlayerInfo>	PlayerInfoList_;
	vector<vector<TDiplomaticStatus> >	Diplomacy_;
	TRound              Round;
	TId                 PlayerSize;
	bool                isValid_;
	//second_class statistics resolved
	vector<cv::Mat>     OwnershipMasks_;
    vector<cv::Mat>     AttackPointsMap_;
    cv::Mat             DefensePointsMap_;
};

}


#endif
