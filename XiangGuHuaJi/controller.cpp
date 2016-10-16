/* XiangGuHuaJi 2016, controller.cpp
 *
 */

#include "controller.h"


namespace XGHJ
{
	void Controller::run()
	{
		TId playerSize = game_.getPlayerSize();
		TRound round = game_.getRound();

        cout << "-=-=-=-=-=-=-=-=-=-=-= Controller: Round[" << round << "] =-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;

		if (round != 0)
		{
			vector<vector<TMilitaryCommand> > MilitaryCommandMap(playerSize);
			vector<vector<TDiplomaticCommand> > DiplomaticCommandMap(playerSize);
			vector<TPosition > NewCapitalList(playerSize);

            // each player run 
			for (TId id=0; id<playerSize; ++id)
			{
                Player& player = players_[id];

                Info info = game_.generateInfo(id);
                if (player.isValid())
                {
                    cout << "Calling Player " << (int)id << "'s Run() method" << endl;
                    players_[id].run(info);
                }
                else
                {
                    ;
                }
                MilitaryCommandMap[id] = info.MilitaryCommandList;
                DiplomaticCommandMap[id] = info.DiplomaticCommandList;	
                NewCapitalList[id] = info.newCapital;
			}
			game_.Run(MilitaryCommandMap, DiplomaticCommandMap, NewCapitalList);
			isValid_ = game_.isValid;
		}
		else
        {
            // bid
            cout << "Bidding......" << endl;
            vector<TMoney> bidPrice;
            vector<std::tuple<TMoney, TId> > bidPriceTuple;
            for (TId id=0; id<playerSize; ++id)
            {
                Player& player = players_[id];
                TMoney price = 0;
                if (player.isValid())
                {
                    cout << "Calling Player " << (int)id << "'s birthplacePrice() method" << endl;
                    players_[id].run(price);
                }
                else
                {
                    ;
                }
                if (price>INITIAL_PLAYER_MONEY)
                {
                    price = INITIAL_PLAYER_MONEY;
                }
                bidPriceTuple.push_back(std::make_tuple(price, id));
                bidPrice.push_back(price);
            }
            //sort
            std::sort(bidPriceTuple.begin(), bidPriceTuple.end(), 
                [](const std::tuple<TMoney, TId> &a, const std::tuple<TMoney, TId> &b) -> bool {
                    return std::get<0>(a) > std::get<0>(b);
                }
            );
            // choose start pos
            cout << "Choosing birthplace......" << endl;
            vector<TPosition> posChoosed;
            for (TId i=0; i<playerSize; ++i)
            {
                TId id = std::get<1>(bidPriceTuple[i]);
                Player& player = players_[id];
                TPosition pos = invalidPos;
                if (player.isValid())
                {
                    cout << "Calling Player " << (int)id << "'s birthplacePrice() method" << endl;
                    players_[id].run(pos, posChoosed);
                }
                else
                {
                    ;
                }
                posChoosed.push_back(pos);
            }

			// choose start pos
			game_.Start(bidPrice, posChoosed);
		}

        //check if killed
        for (TId i=0; i<playerSize; ++i)
        {
            if (!game_.isPlayerAlive[i])
            {
                players_[i].kill();
            }
        }

	}
}