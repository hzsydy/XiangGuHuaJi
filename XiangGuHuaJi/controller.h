/* XiangGuHuaJi 2016, controller.h
 *
 */

#ifndef _XIANGGUHUAJI_CONTROLLER_H__
#define _XIANGGUHUAJI_CONTROLLER_H__

#include <vector>
#include <iostream>
#include "definition.h"
#include "game.h"
#include "info.hpp"
#include "player.h"
#include "converter.hpp"

using std::vector;


namespace XGHJ
{
	class Controller 
	{
	public:
		Controller(Game& g, vector<Player>& p) 
			: game_(g), players_(p) 
		{
			rows = g.map.getRows();
			cols = g.map.getCols();
			MapRes = getConvertedMat<TMapPara>(g.map.getMapRes());
			MapDef = getConvertedMat<TMapPara>(g.map.getMapDef());
			MapAtk = getConvertedMat<TMapPara>(g.map.getMapAtk());
			PlayerSize = game_.getPlayerSize();
		}

		void Run();
		inline Info& getInfo(TId playerid) {return infos_[playerid];}
		inline bool isValid(){return isValid_;}

	protected:
		
	private:
		Game& game_;
		vector<Player>& players_;
		Info generateInfo(TId playerid);
		vector<Info> infos_;

		bool isValid_;

		TMapSize rows;
		TMapSize cols;
		vector<vector<TMapPara> > MapRes;
		vector<vector<TMapPara> > MapDef;
		vector<vector<TMapPara> > MapAtk;

		//first_class statistics
		vector<cv::Mat>     MilitaryMap_;
		cv::Mat/*CV_TId*/	GlobalMap_;
		vector<TPlayerInfo>	PlayerInfoList;
		vector<vector<TDiplomaticStatus> >	Diplomacy;
		TRound              Round;
		TId                 PlayerSize;
		//second_class statistics resolved
		vector<cv::Mat>     OwnershipMasks_;
		vector<cv::Mat>     AttackPointsMap_;
		cv::Mat             DefensePointsMap_;
        //cached vectors
        vector<vector<vector<TMilitary> > > MilitaryMap;
        vector<vector<TId> >                GlobalMap;
        vector<vector<vector<TAttack> > >   AttackPointsMap;
        vector<vector<TDefense> >           DefensePointsMap;
	};
}

#endif