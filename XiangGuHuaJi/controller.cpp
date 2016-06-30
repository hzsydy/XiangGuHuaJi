/* XiangGuHuaJi 2016, controller.cpp
 *
 */

#include "controller.h"

namespace XGHJ
{
	void Controller::Run()
	{
		Round			 = 	game_.getRound()			;
		if (Round != 0)
		{
			vector<cv::Mat/*cv_Military*/> MilitaryCommandList(PlayerSize);
			vector<vector<TDiplomaticCommand> > DiplomaticCommandMap 
				= getResizedVector<TDiplomaticCommand>(PlayerSize, PlayerSize);
			for (TId id=0; id<PlayerSize; ++id)
			{
                Player& player = players_[id];
                if (!player.isValid()) continue;

				Info info = getInfo(id);

				bool runwell = true;
                runwell = runwell && players_[id].Run(info);
                if (!runwell) continue;
				cv::Mat mat = cv::Mat::zeros(rows, cols, CV_TMilitary);
				runwell = runwell && convertVector<TMilitary>(info.MilitaryCommand, mat);
                if (!runwell) continue;
					
				MilitaryCommandList[id] = mat;
				DiplomaticCommandMap[id] = info.DiplomaticCommandList;				
			}
			game_.Run(MilitaryCommandList, DiplomaticCommandMap);
		}
		else
		{
			//choose start pos

			game_.Start();
		}

		isValid_ = game_.isValid();	
		if (!isValid_) return;

		Round			 = 	game_.getRound()			;
		MilitaryMap		 = 	game_.getMilitaryMap()		;
		GlobalMap		 = 	game_.getGlobalMap()		;
		PlayerInfoList	 = 	game_.getPlayerInfoList()	;
		Diplomacy		 = 	game_.getDiplomacy()		;	
		OwnershipMasks	 = 	game_.getOwnershipMasks()	;
		AttackPointsMap	 = 	game_.getAttackPointsMap()	;
		DefensePointsMap = 	game_.getDefensePointsMap()	;

		infos_.clear();
		for (TId i=0; i<PlayerSize; i++)
		{
			Info info = generateInfo(i);
			infos_.push_back(info);
		}
	}

	Info Controller::generateInfo(TId playerid)
	{
		// MilitaryCommand
		vector<vector<TMilitary> >      MilitaryCommand;
		resizeVector(MilitaryCommand, PlayerSize, PlayerSize);

		// Mask
		cv::Mat OwnershipMask_ = cv::Mat::zeros(rows, cols, CV_8UC1),
			VisibleMask_ = cv::Mat::zeros(rows, cols, CV_8UC1),
			ControlMask_ = cv::Mat::zeros(rows, cols, CV_8UC1);
		vector<vector<unsigned char> >	OwnershipMask, VisibleMask, ControlMask;

		OwnershipMask_ = OwnershipMasks[playerid].clone();
		for (TId target=0; target<PlayerSize; ++target)
		{
			TDiplomaticStatus status = Diplomacy[playerid][target];
			if (status!=Undiscovered && status!=AtWar) 
				VisibleMask_ += OwnershipMasks[target];
			if (status==Allied) 
				ControlMask_ += OwnershipMasks[target];
		}
		return Info(
			playerid,
			Round,
			rows,
			cols,
			PlayerSize,
			getConvertedMat<TMask>(OwnershipMask_),
			getConvertedMat<TMask>(VisibleMask_),
			getConvertedMat<TMask>(ControlMask_),
			getConvertedMat<TId>(GlobalMap),
			MapRes,
			MapDef,
			MapAtk,
			getConvertedMat<TMilitary>(MilitaryMap),
			getConvertedMat<TAttack>(AttackPointsMap),
			getConvertedMat<TDefense>(DefensePointsMap),
			PlayerInfoList,
			Diplomacy,
			getResizedVector<TMilitary>(cols, rows),
			vector<TDiplomaticCommand>(PlayerSize)
		);
	}
}