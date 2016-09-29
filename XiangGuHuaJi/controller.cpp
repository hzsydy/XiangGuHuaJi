/* XiangGuHuaJi 2016, controller.cpp
 *
 */

#include "controller.h"


namespace XGHJ
{
	void Controller::Run()
	{
		TId playerSize = game_.getPlayerSize();
		TRound round = game_.getRound();

        cout << "-=-=-=-=-=-=-=-=-=-=-= Controller: Round[" << round << "] =-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;

		if (round != 0)
		{
			vector<vector<TMilitaryCommand> > MilitaryCommandMap(playerSize);
			vector<vector<TDiplomaticCommand> > DiplomaticCommandMap(playerSize);

            // each player run 
			for (TId id=0; id<playerSize; ++id)
			{
                Player& player = players_[id];
                bool runwell=true;

                if (!player.isValid()) runwell = false;
				Info info = game_.generateInfo(id);
                if (runwell) runwell = players_[id].Run(info);
				
				MilitaryCommandMap[id] = info.MilitaryCommandList;
				DiplomaticCommandMap[id] = info.DiplomaticCommandList;				
			}
			game_.Run(MilitaryCommandMap, DiplomaticCommandMap);
		}
		else
		{
			//choose start pos
			game_.Start();
		}

		if (!game_.isValid) return;
	}
}