/* XiangGuHuaJi 2016, main.cpp
 * Du 17-02-28
 * 采用瑞士赛制 https://zh.wikipedia.org/wiki/%E7%91%9E%E5%A3%AB%E5%88%B6
 */

#include<ctime>
#include<cstdlib>

#include<iostream>  
#include<fstream>
#include<string>
#include<sstream>


#include "military_kernel.h"
#include "controller.h"

using namespace std;
using namespace XGHJ;

const int TOTAL_ROUND = 30;
const int PLAYERS_PER_GAME = 8;
const int score_by_rank[PLAYERS_PER_GAME] = {5,3,2,2,1,1,0,0};

struct scoredAI
{
    Player p;
    string name;
    float score;
    scoredAI(Player &_p, const string& _name)
        :score(0), name(_name), p(_p)
    { }
};

float rand_f(){
    return (float)rand()/RAND_MAX;
}

void usage()
{
    cout<<"the tournament neads a specific config path"<<endl;
    cout<<"../config_tournament_msvc.ini in MSVC"<<endl;
    cout<<"OR ../config_tournament_gnu.ini in GNU"<<endl;
}

int main(int argc, char** argv) 
{
    string  config_filename = 
#ifdef _MSC_VER
        "../config_tournament_msvc.ini";
#endif
#ifdef __GNUC__
        "../config_tournament_gnu.ini";
#endif
    if (argc != 1)
    {
        usage();
        return -1;
    }

	vector<scoredAI>  players;
    string          map_filename; 
    string          mknl_filename; 
    string          weak_ai_filename;
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
    ifs >> weak_ai_filename;
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

    // load player
    for (size_t i=0; i<players_filename.size(); ++i) {
        Player player(players_filename[i], i);
        if (player.isValid()) {
            players.push_back(scoredAI(player, players_filename[i]+"#"+std::to_string(i)));
        }
    }
    if (players.size() == 0) {
        cout << "[Error] No valid player ai Loaded. Aborted." << endl;
        return -1;
    }
    cout << "[Info] " << players.size() << " players loaded." << endl;

    cout << "[Info] loading dummy players" << endl;
    int cnt_dummy = PLAYERS_PER_GAME - players.size() % PLAYERS_PER_GAME;
    if (cnt_dummy == PLAYERS_PER_GAME) cnt_dummy = 0; 
    for (size_t i=0; i<cnt_dummy; ++i) {
        Player player(weak_ai_filename, i);
        if (player.isValid()) {
            players.push_back(scoredAI(player, "weak_ai_dummy#"+std::to_string(i)));
        }
    }

    srand(time(NULL));
    for (scoredAI& player : players) player.score = rand_f();
    //sort the scores
    std::sort(players.begin(), players.end(), 
        [](const scoredAI &a, const scoredAI &b) -> bool { return a.score > b.score; }
    );
    for (scoredAI& player : players) player.score = 0;

    for (int tour_round = 0; tour_round<TOTAL_ROUND; tour_round++)
    {
        cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
        cout << "-=-=-=-=-=-=-=-=-= SWISS-SYSTEM TOURNAMENT ROUND "<<tour_round << endl;
        cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;


        vector<Player> game_players;
        game_players.clear();
        int c=0;
        for (int i=0; i<players.size(); i++) {
            players[i].p.load();
            game_players.push_back(players[i].p);
            if (game_players.size() == PLAYERS_PER_GAME) {
                // sheng si kan dan bu fu jiu gan
                Game game(map, militaryKernel, PLAYERS_PER_GAME);
                Controller controller(game, game_players);
                controller.setSilentMode(true);

                cout << "--- Game ---" << endl;
                while (controller.isValid()) controller.run();

                vector<int> rank = game.getPlayerRanking();
                for (size_t j=0; j<rank.size(); ++j) {
                    players[8*c+rank[j]].score += score_by_rank[j];
                }
                c++;
                game_players.clear();
            }
        }

        game_players.clear();

        cout<<"RESULT:"<<endl;
        for (scoredAI& player : players)
            player.score += rand_f();
        //sort the scores
        std::sort(players.begin(), players.end(), 
            [](const scoredAI &a, const scoredAI &b) -> bool { return a.score > b.score; }
        );
        for (scoredAI& player : players) {
            player.score = (int)player.score;
            cout<<setw(30)<<player.name<<" : "<<(int)player.score<<endl;
        }
    }

    system("pause");

	return 0;

} 
