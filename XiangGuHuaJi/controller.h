/* XiangGuHuaJi 2016, controller.h
 *
 */

#ifndef _XIANGGUHUAJI_CONTROLLER_H__
#define _XIANGGUHUAJI_CONTROLLER_H__

#include <vector>
#include <iostream>
#include "definition.h"
#include "game.h"
#include "player.h"

using std::vector;


namespace XGHJ
{
	class Controller 
	{
	public:
		Controller(Game& g, vector<Player>& p) 
			: game_(g), players_(p) 
		{
			playerSize_ = game_.getPlayerSize();
		}

		void Run();
		inline bool isValid(){return isValid_;}

	protected:
		Game& game_;
		vector<Player>& players_;
		bool isValid_;
		TId playerSize_;
	};
}

#endif