/* XiangGuHuaJi 2016, main.cpp
 *
 * HuaJiClient
 */

#define SHUTDOWN { system("pause"); return -1; }
#define min(x,y) ((x)<(y)?(x):(y))
#define max(x,y) ((x)>(y)?(x):(y))

#include<ctime>
#include<cstdio>

#include<iostream>  
#include<fstream>
#include<string>
#include<sstream>

#include "definition.h"
#include "game.h"
#include "player.h"

#include "xghj_protocol.h"
#include "military_kernel_reader.h"


using namespace std;
using namespace XGHJ;
using namespace XGHJ_Client;


//const string server_ip = "166.111.72.13";
const string server_ip = "127.0.0.1";
const int server_port = 9999;
const TId playerSize = 4;

int main(int argc, char** argv) 
{
    string config_filename = "../config_msvc.ini";

    if (argc ==2) {
        config_filename = argv[1];
    }
    else if (argc>=2) {
		cout<<"usage:												"<<endl
			<<"HuaJiClient						Load config file	"<<endl
		;
	}

    char buffer[1024];
    string          map_filename;       // map filename 
    string          mknl_filename;      // military kernel filename
    string          player_filename;    // player dll/so filename 
    int             my_player_id = -1;

    // load config file 
    ifstream ifs;
    if (argc==2) config_filename = argv[1];
    ifs.open(config_filename.c_str());    
    if(!ifs) {
        cout<<"[Error] Failed to load config file \""<< config_filename << "\". Aborted. " << endl;
        SHUTDOWN;
    }
    ifs >> map_filename;   
    ifs >> mknl_filename; 
    ifs >> player_filename;

    // log file output
    time_t t = time(0);
    strftime(buffer, sizeof(buffer), "log_%Y%m%d_%H%M%S.txt",localtime(&t));
    ofstream  ofs(buffer);

    // load map
    Map map = Map();
    if (!map.load(map_filename)) {
        cout << "[Error] Failed to load map \"" << map_filename << "\". Aborted. " << endl;
        SHUTDOWN;
    }

    // load military kernel
    vector<vector<float> > militaryKernel;
    if (!loadMilitaryKernel(militaryKernel, mknl_filename)) {
        cout<<"[Error] Failed to load Military Kernel \""<< mknl_filename << "\". Aborted. " << endl;
        SHUTDOWN;
    }

    // connect the server 
    boost::asio::io_service io_service;
    XghjProtocolSocket xs(io_service, server_ip, server_port);
    if (!xs.isValid()) {
        cout<<"[Error] Failed to connect server "<< server_ip << ":" << server_port << ". Aborted. " << endl;
        SHUTDOWN;
    }
    io_service.run();

    // and start/join a new game
    XghjObject obj(XghjObject::Player, XghjObject::NewGame, "simple client");
    xs.send(obj);
    // recv echo and get my_player_id
    obj = xs.getObj();
    cout << "[Info] receive: "<< obj.toString() << endl;
    if (obj.action==XghjObject::OK) {
        my_player_id = getNumber(obj.content.c_str());
        cout << "[Info] I'm P" << my_player_id << " in the game." << endl;
    }
    else {
        cout << "[Error] player unaccepted by server." << endl;
        SHUTDOWN;
    }

    // load player dll/so
    Player my_player(player_filename, my_player_id);
    if (!my_player.isValid()) {
        cout << "[Error] Failed to load player \"" << player_filename << "\". Aborted. " << endl;
        SHUTDOWN;
    }

    // load game
    Game game(map, militaryKernel, 4);

    // waiting for the start
    cout << "[Info] I'm ready. Waiting for the start." << endl;
    obj = xs.getObj();
    if (obj.action!=XghjObject::NewGame) {
        cout << "[Error] Sorry I missed the start." << endl;
        cout << "[Error][recv] "<< obj.content << endl;
        SHUTDOWN;
    }
    ofs << obj.content << endl;


    // ---------------------------------------------------
    char *p;
    TId count = 0;

    // Round 0
    {
        vector<TMoney> bidPrice(playerSize);
        vector<TPosition> bidPosition(playerSize);
        
        vector<TPosition> posUnaccessible;
        TMoney my_price = 0;
        TPosition my_pos = invalidPos;


        cout << "[Game] bidding" << endl;

        // calc price
        my_player.run(my_price, &(game.map));
        bidPrice[my_player_id] = my_price;

        // send my price
        sprintf(buffer, "%d", my_price);
        obj = XghjObject(XghjObject::Player, XghjObject::BidPrice, buffer);
        obj.round = 0; obj.sender_id = my_player_id;
        xs.send(obj);

        // recv full bidPrice
        obj = xs.getObj();
        ofs << obj.content << endl;
        sprintf(buffer, "%s", obj.content.c_str());
        p = buffer;
        for (TId id=0; id<playerSize; ++id){
            bidPrice[id] = getNumber(&p);
        }
        
        // wait for posUnaccessible
        obj = xs.getObj();  
        sprintf(buffer, "%s", obj.content.c_str());
        p = buffer;
        int n = getNumber(&p);
        for (int i=0; i<n; ++i) {
            TPosition pos;
            pos.x = getNumber(&p);
            pos.y = getNumber(&p);
            posUnaccessible.push_back(pos);
        }
        my_player.run(my_pos, posUnaccessible, &(game.map));

        // send my_pos
        sprintf(buffer, "%d %d", my_pos.x, my_pos.y);
        obj = XghjObject(XghjObject::Player, XghjObject::BidPosition, buffer);
        xs.send(obj);

        // recv full bidPosition
        obj = xs.getObj(); 
        ofs << obj.content << endl;
        sprintf(buffer, "%s", obj.content.c_str());
        p = buffer;
        for (int player_id=0; player_id<playerSize; ++player_id) {
            TPosition pos;
            pos.x = getNumber(&p);
            pos.y = getNumber(&p);
            bidPosition[player_id] = pos;
        }

        // Round 0 finished !!!
        game.Start(bidPrice, bidPosition);

    }


    // wait and calc pos


    while (true) {

        cout << "[Game] Round " << game.getRound() << endl;

        vector<vector<TMilitaryCommand> > MilitaryCommandMap(playerSize);
		vector<vector<TDiplomaticCommand> > DiplomaticCommandMap(playerSize);
		vector<TPosition > NewCapitalList(playerSize);

        // my turn!
        Info my_info = game.generateInfo(my_player_id);
        if (my_player.isValid()) {
            my_player.run(my_info);
        }

        // send my choice
        stringstream ss;
        for (int i=0; i<my_info.DiplomaticCommandList.size(); ++i) 
            ss<<" "<<(int)my_info.DiplomaticCommandList[i];
        
        for (int i=0; i<min(my_info.MilitaryCommandList.size(),16); ++i) 
            ss<<" "<<(int)my_info.MilitaryCommandList[i].place.x
              <<" "<<(int)my_info.MilitaryCommandList[i].place.y
              <<" "<<(int)my_info.MilitaryCommandList[i].bomb_size;
        ss<<" -1";
        ss<<" "<<(int)my_info.newCapital.x
          <<" "<<(int)my_info.newCapital.y;

        obj = XghjObject(XghjObject::Player, XghjObject::OK, ss.str());
        obj.sender_id = my_player_id;
        obj.round = game.getRound();
        obj.map_checksum = game.getMapChecksum();
        xs.send(obj);

        // recv full command
        do {
            obj = xs.getObj();

            if (obj.action==XghjObject::GameOver) {
                cout << "-=-=-=-=-=-=-=-=-=-=-= GAME OVER ! =-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
                SHUTDOWN;
            }
            if (obj.action!=XghjObject::Invalid)
                cout << "[Info] recv: " << obj.content << endl;

        } while (obj.action!=XghjObject::NextRound);

        ofs << obj.content << endl;

        ss.str("");
        ss << obj.content;
        for (int player_id=0; player_id<playerSize; ++player_id) {
            vector<TDiplomaticCommand> player_dipt_command(playerSize);
            vector<TMilitaryCommand> player_military_command;
            TPosition player_new_capital;
            int t;

            for (int i=0; i<playerSize; ++i){
                ss >> t; player_dipt_command[i] = (TDiplomaticCommand)t;
            }

            while (true) {
                TMilitaryCommand tmc;
                ss >> t;
                if (t==-1) break; tmc.place.x = t;
                ss >> t; tmc.place.y = t;
                ss >> t; tmc.bomb_size = t;
                player_military_command.push_back(tmc);
            }

            ss >> t; player_new_capital.x = t;
            ss >> t; player_new_capital.y = t;

            DiplomaticCommandMap[player_id] = player_dipt_command;
            MilitaryCommandMap[player_id] = player_military_command;
            NewCapitalList[player_id] = player_new_capital;
        }

		if (!game.Run(MilitaryCommandMap, DiplomaticCommandMap, NewCapitalList))
		{
            cout << "-=-=-=-=-=-=-=-=-=-=-= GAME OVER ! =-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
            break;
		}
    }


#ifdef _MSC_VER
    system("pause");
#endif

	return 0;

} 
