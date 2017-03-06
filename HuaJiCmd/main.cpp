/* XiangGuHuaJi 2016, main.cpp
 *
 */

#include<iostream>  
#include<fstream>
#include<string>
#include<sstream>

#include "military_kernel.h"
#include "controller.h"

using namespace std;
using namespace XGHJ;


void outputResult(Game& game, vector<string> players_filename) {
    ofstream ofs("result.txt");

    vector<int> rank = game.getPlayerRanking();
    for (size_t i=0; i<rank.size(); ++i) {
        ofs << players_filename[rank[i]] << endl;
    }
}

int main(int argc, char** argv) 
{

    string  config_filename = 
#ifdef _MSC_VER
        "../config_msvc.ini";
#endif
#ifdef __GNUC__
        "../config_gnu.ini";
#endif

    if (argc ==2) {
        config_filename = argv[1];
    }
    else if (argc>=2) {
		cout<<"usage:												"<<endl
			<<"XiangGuHuaji						Load config file	"<<endl;
	}


	vector<Player>  players;
    string          map_filename; 
    string          mknl_filename; 
	vector<string>  players_filename;

    // load config file
    ifstream ifs(config_filename.c_str());    
    if (!ifs.is_open()) {
        cout<<"Failed to load \""<< config_filename << "\". Aborted. " << endl;
        return -1;
    }

    // read config file
    ifs >> map_filename;   
    ifs >> mknl_filename;   
    while (!ifs.eof()) {
        string player_filename;
        ifs >> player_filename; 
        if (!player_filename.empty()) players_filename.push_back(player_filename);
    }
    if (players_filename.size() == 0) {
        cout << "[Error] player_ai file names expected." << endl;
        return -1;
    }

    // load map
    Map map = Map();
	if (!map.easy_load(map_filename)) {
        cout << "[Error] failed to load map." << map_filename << endl;
        return -1;
    }
    
    // load kernel
    vector<vector<float> > militaryKernel;
    if (!loadMilitaryKernel(militaryKernel, mknl_filename)) {
        cout << "[Error] failed to load military kernel. " << mknl_filename << endl;
        return -1;
    }

    // load players
    for (size_t i=0; i<players_filename.size(); ++i) {
        Player player(players_filename[i], i);
        if (player.isValid()) 
            players.push_back(player); 
        else 
            cout << "[Warning] failed to load player_ai " << players_filename[i] << endl;
    }
    if (players.size() <= 1) {
        cout << "[Error] Not enough player_ais to start the game." << endl;
        return -1;
    }
    cout << "[Info] " << players.size() << " players loaded." << endl;
    
    // game and controller
    Game game(map, militaryKernel, players.size());
	Controller controller(game, players);
    
    // main
    while (controller.isValid()) controller.run();

    // output the result
    outputResult(game, players_filename);

	return 0;

} 
