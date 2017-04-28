/* XiangGuHuaJi 2016, controller.h
 *
 */

#ifndef _XIANGGUHUAJI_CONTROLLER_H__
#define _XIANGGUHUAJI_CONTROLLER_H__

#include <vector>
#include <iostream>
#include <tuple>
#include <algorithm>
#include <fstream>

#include "definition.h"
#include "game.h"
#include "player.h"

namespace XGHJ
{
	class Controller 
	{
	public:
		Controller(Game& g, std::vector<Player>& p) 
            : game_(g), players_(p)
            , silent_mode_(false), file_output_enabled_(true)
            , isValid_(true)
            , playerSize_(game_.getPlayerSize())

		{ }

		void run();

        inline void setSilentMode(bool flag) { silent_mode_ = flag; } 
        inline void setFileOutputEnabled(bool flag) { file_output_enabled_ = flag; }

		inline bool isValid() const {return isValid_;}
        
        inline int getExcitingGameScore() const { return game_.getExcitingGameScore(); }
        inline std::string getLogFilename() const { return log_filename; }

	protected:
		Game& game_;
		std::vector<Player>& players_;

        std::ofstream ofs;

        std::string log_filename;

        bool file_output_enabled_;
        bool silent_mode_;
		bool isValid_;
		TId playerSize_;
	};
}

#endif
