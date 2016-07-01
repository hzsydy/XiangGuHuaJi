/* XiangGuHuaJi 2016, controller.cpp
 *
 */

#include "controller.h"

namespace XGHJ
{
	void Controller::Run()
	{
        Round			  = 	game_.getRound()			;
		MilitaryMap_	  = 	game_.getMilitaryMap()		;
		GlobalMap_		  = 	game_.getGlobalMap()		;
		PlayerInfoList	  = 	game_.getPlayerInfoList()	;
		Diplomacy		  = 	game_.getDiplomacy()		;	
		OwnershipMasks_	  = 	game_.getOwnershipMasks()	;
		AttackPointsMap_  = 	game_.getAttackPointsMap()	;
		DefensePointsMap_ = 	game_.getDefensePointsMap()	;

        GlobalMap       = getConvertedMat<TId>(GlobalMap_);
        MilitaryMap     = getConvertedMat<TMilitary>(MilitaryMap_);
        AttackPointsMap = getConvertedMat<TAttack>(AttackPointsMap_);
        DefensePointsMap= getConvertedMat<TDefense>(DefensePointsMap_);

        cout << "-=-=-=-=-=-=-=-=-=-=-= Controller: Round[" << Round << "] =-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;

		infos_.clear();
		for (TId i=0; i<PlayerSize; i++)
		{
			Info info = generateInfo(i);
			infos_.push_back(info);
		}

		Round = game_.getRound();
		if (Round != 0)
		{
			vector<cv::Mat/*cv_Military*/> MilitaryCommandList(PlayerSize);
			vector<vector<TDiplomaticCommand> > DiplomaticCommandMap 
				= getResizedVector<TDiplomaticCommand>(PlayerSize, PlayerSize);
            // each player run 
			for (TId id=0; id<PlayerSize; ++id)
			{
                Player& player = players_[id];
                Info info = getInfo(id);
                cv::Mat mat = cv::Mat::zeros(rows, cols, CV_TMilitary);
                
                bool runwell=true;

                if (!player.isValid()) runwell = false;
                if (runwell) runwell = players_[id].Run(info);
                if (runwell) runwell = convertVector<TMilitary>(info.MilitaryCommand, mat);
				
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

#ifdef GAME_DEBUG
        cv::Mat m = MilitaryMap_[0].clone();
        cv::resize(m, m, cv::Size(), 20.0, 20.0);
        cv::imshow("status of Player{0}", m);
        cout<<"mm: " << (int)MilitaryMap_[0].at<TMask>(0,0) << endl;;
        cv::waitKey(0);
#endif

	}

	Info Controller::generateInfo(TId playerid)
	{
		// MilitaryCommand
		vector<vector<TMilitary> >      MilitaryCommand;
		resizeVector(MilitaryCommand, PlayerSize, PlayerSize);

		// Mask
        cv::Mat OwnershipMask_ = cv::Mat::zeros(rows, cols, CV_TMask),
			VisibleMask_ = cv::Mat::zeros(rows, cols, CV_TMask),
			ControlMask_ = cv::Mat::zeros(rows, cols, CV_TMask);
		vector<vector<unsigned char> >	OwnershipMask, VisibleMask, ControlMask;

		OwnershipMask_ = OwnershipMasks_[playerid].clone();
		for (TId target=0; target<PlayerSize; ++target)
		{
			TDiplomaticStatus status = Diplomacy[playerid][target];
			if (status!=Undiscovered && status!=AtWar) 
				VisibleMask_ += OwnershipMasks_[target];
			if (status==Allied) 
				ControlMask_ += OwnershipMasks_[target];
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
            GlobalMap,
			MapRes,
			MapDef,
			MapAtk,
			MilitaryMap,
			AttackPointsMap,
			DefensePointsMap,
			PlayerInfoList,
			Diplomacy,
			getResizedVector<TMilitary>(cols, rows),
			vector<TDiplomaticCommand>(PlayerSize)
		);
	}
}