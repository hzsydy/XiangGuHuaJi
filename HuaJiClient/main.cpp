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

void load_web_file(string filename, string& server_ip, int& server_port) {
    ifstream ifs(filename);
    
    if (!ifs.is_open()){
        server_ip = "127.0.0.1";
        server_port = 9999;
        return;
    }

    try{
        ifs >> server_ip;
        ifs >> server_port;
    }
    catch(exception e){
        cerr << e.what() << endl;
        return;
    }

}

int main(int argc, char** argv) 
{
    string server_ip = "127.0.0.1";
    int server_port = 9999;
    TId playerSize = 8;

    string web_filename = "../web.ini";
    string config_filename = "../config_msvc.ini";

    if (argc ==2) {
        config_filename = argv[1];
    }
    else if (argc>=2) {
		cout<<"usage:												"<<endl
			<<"HuaJiClient						Load config file	"<<endl
		;
	}


    load_web_file(web_filename, server_ip, server_port);

    // ------------------------------------------------------------------------------------

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
    if (!map.easy_load(map_filename)) {
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
    XghjObject obj(XghjObject::Player, XghjObject::NewGame, player_filename);
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

    // waiting for the start
    cout << "[Info] I'm ready. Waiting for the start." << endl;
    obj = xs.getObj();
    if (obj.action!=XghjObject::NewGame) {
        cout << "[Error] Sorry I missed the start." << endl;
        cout << "[Error][recv] "<< obj.content << endl;
        SHUTDOWN;
    }
    playerSize = atoi(obj.content.c_str());
    ofs << obj.content << endl;


    // load game
    Game game(map, militaryKernel, playerSize);

    // ---------------------------------------------------
    TId count = 0;

    // Round 0
    {
        vector<TMoney> bidPrice;
        vector<TPosition> bidPosition;
        vector<TPosition> posUnaccessible;

        TMoney my_price = 0;
        TPosition my_pos = INVALID_POSITION;


        cout << "[Game] bidding" << endl;

        // calc price
        my_player.run(my_price, &(game.map));

        // send my price
        sprintf(buffer, "%d", my_price);
        obj = XghjObject(XghjObject::Player, XghjObject::BidPrice, buffer);
        obj.round = 0; obj.sender_id = my_player_id;
        xs.send(obj);

        // recv full bidPrice
        obj = xs.getObj();
        ofs << obj.content << endl;
        if (!obj.getBidPrice(bidPrice, playerSize)) {
            cerr << "[Error] get bid price list" << endl;
            SHUTDOWN;
        }
        
        // wait for posUnaccessible
        obj = xs.getObj();  
        if (!obj.getBidPosUnaccessible(posUnaccessible)) {
            cerr << "[Error] get pos unaccessible" << endl;
            SHUTDOWN;
        }
        my_player.run(my_pos, posUnaccessible, &(game.map));

        // send my_pos
        sprintf(buffer, "%d %d", my_pos.x, my_pos.y);
        obj = XghjObject(XghjObject::Player, XghjObject::BidPosition, buffer);
        xs.send(obj);

        // recv full bidPosition
        obj = xs.getObj(); 
        ofs << obj.content << endl;
        if (!obj.getBidPosition(bidPosition, playerSize)) {
            cerr << "[Error] get bid position list" << endl;
            SHUTDOWN;
        }

        // Round 0 finished !!!
        game.Start(bidPrice, bidPosition);

    }


    while (true) {


        cout << "[Game] Round " << game.getRound() << endl;

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

        vector<vector<TMilitaryCommand> > MilitaryCommandMap;
		vector<vector<TDiplomaticCommand> > DiplomaticCommandMap;
		vector<TPosition > NewCapitalList;
        if (!obj.getMilitaryCommand(MilitaryCommandMap, DiplomaticCommandMap, NewCapitalList, playerSize)) {
            cerr << "[Error] get military command" << endl;
            SHUTDOWN;
        }

		if (!game.Run(MilitaryCommandMap, DiplomaticCommandMap, NewCapitalList))
		{
            cout << "-=-=-=-=-=-=-=-=-=-=-= GAME OVER ! =-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
            break;
		}
    }

    system("pause");

	return 0;

} 
