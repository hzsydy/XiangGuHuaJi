/* XiangGuHuaJi 2016, main.cpp
 *
 * HuaJiClient
 */

#include<iostream>  
#include<fstream>
#include<string>
#include<sstream>

#include "xghj_protocol.h"
#include "controller.h"
#include "military_kernel_reader.h"

using namespace std;
using namespace XGHJ;
using namespace XGHJ_Client;


const string server_ip = "127.0.0.1";
const int server_port = 9999;

string config_filename =
#ifdef _MSC_VER
    "../config_msvc.ini";
#endif
#ifdef __GNUC__
    "../config_linux.ini";
#endif    



int main(int argc, char** argv) 
{
    if (argc>=2) {
		cout<<"usage:												"<<endl
			<<"XiangGuHuaji						Load config file	"<<endl
		;
        return -1;
	}

	vector<Player>  players;            // player list 
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
        return -1;
    }
    ifs >> map_filename;   
    ifs >> mknl_filename; 
    ifs >> player_filename;

    // load map
    Map map = Map();
    if (!map.load(map_filename)) {
        cout << "[Error] Failed to load map \"" << map_filename << "\". Aborted. " << endl;
        return -1;
    }

    // load military kernel
    vector<vector<float> > militaryKernel;
    if (!loadMilitaryKernel(militaryKernel, mknl_filename)) {
        cout<<"[Error] Failed to load Military Kernel \""<< mknl_filename << "\". Aborted. " << endl;
        return -1;
    }

    // connect the server 
    boost::asio::io_service io_service;
    XghjProtocolSocket xs(io_service, server_ip, server_port);
    if (!xs.isValid()) {
        cout<<"[Error] Failed to connect server "<< server_ip << ":" << server_port << ". Aborted. " << endl;
        return -1;
    }
    io_service.run();

    // and start/join a new game
    XghjObject obj(XghjObject::Player, XghjObject::NewGame, "simple client");
    xs.send(obj);

    //
    
    while (true) {
        _sleep(50);
        if (xs.recv()) {
            XghjObject obj = xs.getObj();
            cout << "[Info] receive: "<< obj.toString() << endl;

            if (obj.action==XghjObject::OK) {
                my_player_id = getNumber(obj.content.c_str());
                cout << "[Info] I'm P" << my_player_id << " in the game." << endl;
            }
            else {
                cout << "[Error] player unaccepted by server." << endl;
            }

            break;
        }
    }

    if (my_player_id>=0){

        Player my_player(player_filename, my_player_id);
        
        if (!my_player.isValid()) {
            cout << "[Error] Failed to load player \"" << player_filename << "\". Aborted. " << endl;
            return -1;
        }

        Game game(map, militaryKernel, players.size());

        // listen to the server  ...

        // while (..) {

        // my_player.run();
        // send my command
        
        // listen to the server  ...

        // game.run();

        // }

    }

    


#ifdef _MSC_VER
    system("pause");
#endif

	return 0;

} 
