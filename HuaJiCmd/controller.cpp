/* XiangGuHuaJi 2016, controller.cpp
 *
 */

#include "controller.h"

#include <fstream>

namespace XGHJ
{
    using namespace std;

    static ofstream ofs;

	void Controller::run()
	{

        if (!ofs.is_open()){
            char buffer[1024];
            time_t t = time(0);
            strftime(buffer, sizeof(buffer), "log_%Y%m%d_%H%M%S.txt",localtime(&t));
            ofs.open(buffer);
        }

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
                DiplomaticCommandMap[id] = info.DiplomaticCommandList;	
                MilitaryCommandMap[id] = info.MilitaryCommandList;
                NewCapitalList[id] = info.newCapital;

                for (int i=0; i<playerSize; ++i) ofs<<" "<<(int)info.DiplomaticCommandList[i];
                for (int i=0; i<info.MilitaryCommandList.size(); ++i) 
                    ofs<<" "<<(int)info.MilitaryCommandList[i].place.x 
                       <<" "<<(int)info.MilitaryCommandList[i].place.y
                       <<" "<<(int)info.MilitaryCommandList[i].bomb_size; 
                ofs << " -1 " << (int)info.newCapital.x << " " << (int)info.newCapital.y << "\t" ;
			}
            ofs << endl;

			if (!game_.Run(MilitaryCommandMap, DiplomaticCommandMap, NewCapitalList))
			{
                cout << "-=-=-=-=-=-=-=-=-=-=-= GAME ENDS ! =-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
                //TODO:GET WINNER
			}
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
                    players_[id].run(price, &(game_.map));
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
                TPosition pos = INVALID_POSITION;
                if (player.isValid())
                {
                    cout << "Calling Player " << (int)id << "'s birthplacePrice() method" << endl;
                    players_[id].run(pos, posChoosed, &(game_.map));
                }
                else
                {
                    ;
                }
                posChoosed.push_back(pos);
            }

            vector<TPosition> posChoosedSorted(playerSize);
            for (TId i=0; i<playerSize; ++i)
            {
                TId id = std::get<1>(bidPriceTuple[i]);
                posChoosedSorted[id] = posChoosed[i];
            }

            ofs << (int)playerSize << endl;
            for (int i=0; i<playerSize; ++i) ofs<<" "<<(int)bidPrice[i];
            ofs << endl;
            for (int i=0; i<playerSize; ++i) ofs<<" "<<(int)posChoosedSorted[i].x << " " << (int)posChoosedSorted[i].y;
            ofs << endl;

			// choose start pos
			game_.Start(bidPrice, posChoosedSorted);
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