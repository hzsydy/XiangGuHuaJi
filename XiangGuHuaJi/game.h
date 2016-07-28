/* XiangGuHuaJi 2016, game.h
 * 
 */

#ifndef _XIANGGUHUAJI_GAME_H__
#define _XIANGGUHUAJI_GAME_H__




#include <string>
#include <cmath>
#include <vector>
#include <opencv2/opencv.hpp>

#include "definition.h"
#include "map.h"
#include "converter.hpp"


using std::vector;

namespace XGHJ {
class Game
{
public:
    Game(Map& map, int playersize);
    ~Game();

    bool Start();
	bool Run(vector<cv::Mat/*TMilitary*/> &         MilitaryCommandList,
		vector<vector<TDiplomaticCommand> > &       DiplomaticCommandMap);
    bool ConstructionPhase(vector<cv::Mat/*TMilitary*/> &       MilitaryCommandList);
    bool DiplomacyPhase(vector<vector<TDiplomaticCommand> > &   DiplomaticCommandMap);
    bool MilitaryPhase(vector<cv::Mat/*TMilitary*/> &           MilitaryCommandList);
    bool ProducingPhase();
    bool CheckWinner();

    Map& map;

    //TODO move these constant to somewhere else
	const static TRound     MAX_ROUND;
    const static TMilitary  MAX_MILITARY;
    const static TSaving    UNIT_SAVING;
    const static TSaving    UNIT_CITY_INCOME;
	const static TRound     TRUCE_TIME;
	const static float		CORRUPTION_COEF;//收获资源的腐败系数
	const static float		DEPRECIATION_COEF;//撤回盟友投资的折旧系数
	const static int		MILITARY_KERNEL_SIZE;//影响力核心的大小
	const static float		MILITARY_KERNEL_SIGMA_2;//影响力核心的高斯函数的系数sigma的平方
	const static float		MILITARY_KERNEL_GAUSS_COEF;//影响力核心的高斯函数前面的系数
	const static float		MILITARY_KERNEL_BASE_EFF;//基础影响力

	//first_class statistics
	inline vector<cv::Mat>		getMilitaryMap()	 {return  MilitaryMap_		;}
	inline cv::Mat/*CV_TId*/	getGlobalMap()		 {return  GlobalMap_		;}
	inline vector<TPlayerInfo>	getPlayerInfoList()	 {return  PlayerInfoList_	;}
	inline vector<vector<TDiplomaticStatus> >									
								getDiplomacy()		 {return  Diplomacy_		;}
	inline TRound				getRound()			 {return  Round  			;}
	inline TId					getPlayerSize() 	 {return  PlayerSize		;}
	inline bool					isValid() 			 {return  isValid_      	;}
	inline vector<cv::Mat>		getOwnershipMasks()	 {return  OwnershipMasks_	;}
	inline vector<cv::Mat>		getAttackProcMap() {return  AttackProcMap_	;}
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
	vector<cv::Mat>     OwnershipMasks_;
    vector<cv::Mat>     AttackProcMap_;	//额 这并不是这个点攻击力的值的意思 而是攻击进程的意思
    cv::Mat             DefensePointsMap_;	//是这个点防御力的值
private:
    vector<vector<TId> >  TruceTreaty;
	cv::Mat MilitaryKernel;
};

}


#endif
