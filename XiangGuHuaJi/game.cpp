/* XiangGuHuaJi 2016, game.cpp
 * 
 */

#include "game.h"

namespace XGHJ 
{

const TMilitary     Game::MAX_MILITARY = 255;
const TRound        Game::MAX_ROUND = 20;
const TSaving       Game::UNIT_SAVING = 10;
const TRound        Game::TRUCE_TIME = 5;
const float			Game::CORRUPTION_COEF = 0.001f;
const float			Game::DEPRECIATION_COEF = 0.3f;
const int			Game::MILITARY_KERNEL_SIZE = 5;
const float			Game::MILITARY_KERNEL_SIGMA_2 = 2.25f;
const float			Game::MILITARY_KERNEL_GAUSS_COEF = 100.0f;
const float			Game::MILITARY_KERNEL_BASE_EFF = 1.0f;

#ifdef GAME_DEBUG
const TSaving       Game::UNIT_CITY_INCOME = 100; 
#else
const TSaving       Game::UNIT_CITY_INCOME = 1;
#endif


Game::Game(Map& map, int playersize)
    : map(map), PlayerSize(playersize),
      Round(0), isValid_(true)
{
    MilitaryMap_.resize(PlayerSize);
    AttackProcMap_.resize(PlayerSize);
    for (TId id=0; id<PlayerSize; ++id)
    {
        MilitaryMap_[id] = cv::Mat::zeros(map.size(), CV_TMilitary);
        AttackProcMap_[id] = cv::Mat::zeros(map.size(), CV_TAttack);        
    }
    DefensePointsMap_ = cv::Mat::zeros(map.size(), CV_TDefense);
    GlobalMap_ = cv::Mat(map.size(), CV_TId, cv::Scalar::all(255));

    //PlayerInfoList
    PlayerInfoList_.resize(PlayerSize);
    for (TId id=0; id<PlayerSize; ++id)     
    {
        PlayerInfoList_[id].id = id;
        PlayerInfoList_[id].MapArea = 1;
        PlayerInfoList_[id].MilitarySummary = 1; // it should be refreshed later.
    }

    //OwnershipMasks_
    OwnershipMasks_.resize(PlayerSize);
    for (TId id=0; id<PlayerSize; ++id) 
		OwnershipMasks_[id]=cv::Mat::zeros(map.size(), CV_8UC1);
    
    //Diplomacy
    resizeVector<TDiplomaticStatus>(Diplomacy_, PlayerSize, PlayerSize);
	fillVector<TDiplomaticStatus>(Diplomacy_, Undiscovered);
    for (TId id=0; id<PlayerSize; ++id) Diplomacy_[id][id] = Allied;
    //TruceTreaty_
    resizeVector<unsigned char>(TruceTreaty, PlayerSize, PlayerSize);
	//init MilitaryKernel
	const float pi = 3.1416;
	MilitaryKernel = cv::Mat::zeros(
		2*MILITARY_KERNEL_SIZE+1, 
		2*MILITARY_KERNEL_SIZE+1, 
		CV_32FC1
	);
	for (int i=0; i<MILITARY_KERNEL_SIZE+1; i++)
	{
		for (int j=0; j<MILITARY_KERNEL_SIZE+1; j++)
		{
			float f = exp(-(float)(i^2+j^2)/2/MILITARY_KERNEL_SIGMA_2);
			f /= 2*MILITARY_KERNEL_SIGMA_2*pi;
			MilitaryKernel.at<float>(MILITARY_KERNEL_SIZE+i, MILITARY_KERNEL_SIZE+j) = f;
			MilitaryKernel.at<float>(MILITARY_KERNEL_SIZE+i, MILITARY_KERNEL_SIZE-j) = f;
			MilitaryKernel.at<float>(MILITARY_KERNEL_SIZE-i, MILITARY_KERNEL_SIZE+j) = f;
			MilitaryKernel.at<float>(MILITARY_KERNEL_SIZE-i, MILITARY_KERNEL_SIZE-j) = f;
		}
	}
}

Game::~Game()
{
    
}

// Round<0>
bool Game::Start()
{
	Round = 0;
	++Round;

    //TODO  init the player, call each player to select their birthplace
    for (TId i=0; i<PlayerSize; ++i) 
        if (0==i%2) OwnershipMasks_[i].at<TMask>(i,i)=255;
        else OwnershipMasks_[i].at<TMask>(map.getRows()-1-i,map.getCols()-1-i)=255;
    for (TId id=0; id<PlayerSize; ++id)
        for (TMapSize j=0; j<map.getRows(); ++j)
            for (TMapSize i=0; i<map.getCols(); ++i)
                if (OwnershipMasks_[id].at<TMask>(j,i)) GlobalMap_.at<TId>(j,i)=id;

	return true;
}

//Game Logic
bool Game::Run(vector<cv::Mat/*TMatMilitary*/> & MilitaryCommandList,
			   vector<vector<TDiplomaticCommand> > & DiplomaticCommandMap)
{
    DiplomacyPhase(DiplomaticCommandMap);
    ConstructionPhase(MilitaryCommandList);
    MilitaryPhase(MilitaryCommandList);
    ProducingPhase();

    ++Round;
    if (Round>=MAX_ROUND) isValid_=false;

    return isValid_;    
}

//Diplomacy Phase (Deal with DiplomaticCommandMap)
bool Game::DiplomacyPhase(vector<vector<TDiplomaticCommand> > & DiplomaticCommandMap)
{
    for (TId i=0; i<PlayerSize; ++i)
        for (TId j=0; j<PlayerSize; ++j)        
            if (TruceTreaty[i][j]>0) 
            {
                --TruceTreaty[i][j];
                --TruceTreaty[i][j];
                if (0==TruceTreaty[i][j])
                    Diplomacy_[i][j] = Diplomacy_[j][i] = Neutral;
            }
	
    for (TId i=0; i<PlayerSize; i++)
	{
		for (TId j=0; j<i; j++)
		{
			{
				switch (Diplomacy_[i][j])
				{
				case Neutral:
                    if (DiplomaticCommandMap[i][j] == FormAlliance 
						&& DiplomaticCommandMap[j][i] == FormAlliance)
					{
						//allied
						Diplomacy_[i][j] = Allied;
						Diplomacy_[j][i] = Allied;
					}
					else if (DiplomaticCommandMap[i][j] != DeclareWar 
						&& DiplomaticCommandMap[j][i] != DeclareWar)
					{
						//neutral
						Diplomacy_[i][j] = Neutral;
						Diplomacy_[j][i] = Neutral;
					}
					else
					{
						//at war
						Diplomacy_[i][j] = AtWar;
                        Diplomacy_[j][i] = AtWar;
					}
					break;
				case AtTruce:
					if (DiplomaticCommandMap[i][j] == FormAlliance
                        && DiplomaticCommandMap[j][i] == FormAlliance)
                    {
                        Diplomacy_[i][j]  = Diplomacy_[j][i]  = Allied;
                        TruceTreaty[i][j] = TruceTreaty[j][i] = 0;
                    }
                    //at truce
					//solved at begin
					break;
				case Undiscovered:
					//Discovery will be solved in Military Phase
					break;
				case Allied:
					if (DiplomaticCommandMap[i][j] == FormAlliance 
						&& DiplomaticCommandMap[j][i] == FormAlliance)
					{
						//allied
						;
					}
					else
					{
						//at truce
						Diplomacy_[i][j]  = Diplomacy_[j][i]  = AtTruce;
						TruceTreaty[i][j] = TruceTreaty[j][i] = TRUCE_TIME;
						//TODO units on the used-to-be allied islands should be destroyed
					}
					break;
				case AtWar:
					if (DiplomaticCommandMap[i][j] == DeclareWar 
						|| DiplomaticCommandMap[j][i] == DeclareWar)
					{
						//at war
						;
					}
					else
					{
						//at truce
                        Diplomacy_[i][j]  = Diplomacy_[j][i]  = AtTruce; 
                        TruceTreaty[i][j] = TruceTreaty[j][i] = TRUCE_TIME;
					}
					break;
				}
			}
		}
	}
    return false; //TODO
}

//Construction Phase (Deal with MilitaryCommandList)
bool Game::ConstructionPhase(vector<cv::Mat/*TMatMilitary*/> & MilitaryCommandList)
{
    for (TId id=0; id<PlayerSize; ++id)
    {
        if (PlayerInfoList_[id].Saving==0) continue;

        // Check validity of MilitaryCommandList
        cv::Mat & mat = MilitaryCommandList[id];
        bool valid = true;
        // check the area
        cv::Mat control_mask = cv::Mat::zeros(map.size(), CV_TMask);
        for (TId i=0; i<PlayerSize; ++i)
            if (Allied==Diplomacy_[id][i]) control_mask+=OwnershipMasks_[i];
        // check the money
        TMilitarySummary sum = 0; 
        for (TMapSize j=0; j<map.getRows(); ++j) 
            for (TMapSize i=0; i<map.getCols(); ++i)
            {
                TMilitary& point_military = mat.at<TMilitary>(j, i);
                if (!control_mask.at<TMask>(j, i)) point_military = 0;
                if (point_military + MilitaryMap_[id].at<TMilitary>(j, i) > MAX_MILITARY) 
                    point_military = MAX_MILITARY - MilitaryMap_[id].at<TMilitary>(j, i);
                sum+=point_military;
            }
        if (sum * UNIT_SAVING > PlayerInfoList_[id].Saving) 
        {
            // spend too much money
            valid = false;
            cout << "[Warning] Player {" << (int)id << "} tried to spend more money than he has!!! " << endl;
            unsigned int ratio = sum * UNIT_SAVING / PlayerInfoList_[id].Saving, mult = 0;
            while (ratio>0) { ++mult; ratio >>= 1; }
            for (TMapSize j=0; j<map.getRows(); ++j)
                for (TMapSize i=0; i<map.getCols(); ++i)
                    mat.at<TMilitary>(j, i) >>= mult;
        }
        // sum 
        if (!valid)
        {
            sum = 0;
            for (TMapSize j=0; j<map.getRows(); ++j) 
                for (TMapSize i=0; i<map.getCols(); ++i)
                {
                    TMilitary& point_military = mat.at<TMilitary>(j, i);
                    sum+=point_military;
                }
        }
        // execute
        if ( sum * UNIT_SAVING <= PlayerInfoList_[id].Saving)
        {
            PlayerInfoList_[id].Saving -= sum * UNIT_SAVING;
            MilitaryMap_[id] += mat;
        }
    }

	cout <<"now in Game::ConstructionPhase, for id "<< 1<<endl;
	printMat<TMilitary>(MilitaryMap_[1], "MilitaryMap_[1]");

    return true; //TODO
}

//Military Phase (Deal with DefensePointsMap ,AttackPointsMap)
bool Game::MilitaryPhase(vector<cv::Mat/*TMatMilitary*/> & MilitaryCommandList)
{
	//load MilitaryMap_
	for (TId id=0; id<PlayerSize; ++id)
	{
		cv::Mat mat, mat_dilate, mat_erode;
		cv::Mat mat_ownermask;
		mat = MilitaryMap_[id].clone();
		cv::dilate(mat, mat_dilate, cv::Mat());
		cv::erode(mat, mat_erode, cv::Mat());
		cv::threshold(mat, mat_ownermask, 0, 255, cv::THRESH_BINARY);
		cv::threshold(mat_dilate, mat_dilate, 0, 255, cv::THRESH_BINARY);
		cv::threshold(mat_erode, mat_erode, 0, 255, cv::THRESH_BINARY);
		cv::Mat mat_inner_contour = cv::Mat::zeros(map.size(), CV_TMask);
		cv::Mat mat_outer_contour = cv::Mat::zeros(map.size(), CV_TMask);
		for (int i=0; i<mat.rows; i++)
		{
			for (int j=0; j<mat.cols; j++)
			{
				if (mat_erode.at<TMask>(i, j) == 0 && mat_ownermask.at<TMask>(i, j) == 255)
				{
					mat_inner_contour.at<TMask>(i, j) = 255;
				}
				else if (mat_dilate.at<TMask>(i, j) == 255 && mat_ownermask.at<TMask>(i, j) == 0)
				{
					mat_outer_contour	.at<TMask>(i, j) = 255;
				}
			}
		}
	}

	/*
	// refresh OwnershipMask_
	for (TId id=0; id<PlayerSize; ++id)
	{
		cv::Mat mat, mat_ownermask;
		mat = MilitaryMap_[id].clone();
		cv::threshold(mat, mat_ownermask, 0, 255, cv::THRESH_BINARY);
		OwnershipMasks_[id] = mat_ownermask.clone();
	}
	*/
    // refresh GlobalMap
    for (TId id=0; id<PlayerSize; ++id)
        for (TMapSize j=0; j<map.getRows(); ++j)
            for (TMapSize i=0; i<map.getCols(); ++i)
                if (OwnershipMasks_[id].at<TMask>(j,i)) 
					GlobalMap_.at<TId>(j,i)=id;
	
    return false; //TODO
}

//Producing Phase (Deal with MapResource, PlayerInfoList)
bool Game::ProducingPhase()
{
    for (TId id=0; id<PlayerSize; ++id)
	{
		// lowest income

		TSaving new_saving = 1;

        // map income 
        for (TMapSize j=0; j<map.getRows(); ++j)
            for (TMapSize i=0; i<map.getCols(); ++i)
                if (OwnershipMasks_[id].at<TMask>(j, i)) new_saving+=map.getMapRes().at<TMapPara>(j, i);
        // city income
        new_saving += UNIT_CITY_INCOME*PlayerInfoList_[id].MilitarySummary;

        // corruption 
        PlayerInfoList_[id].Saving+=
			(TSaving)((1-(float)(PlayerInfoList_[id].MapArea)*CORRUPTION_COEF) * (float)new_saving);
    }

    return false; //TODO
}

//Check the winner and the loser (Deal with PlayerInfoList)
bool Game::CheckWinner()
{
    // refresh PlayerInfoList

    return false; //TODO
}

}
