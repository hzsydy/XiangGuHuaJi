/* XiangGuHuaJi 2016, main.cpp
 * Du 17-02-28
 * 采用瑞士赛制 https://zh.wikipedia.org/wiki/%E7%91%9E%E5%A3%AB%E5%88%B6
 */

#include<iostream>  
#include<fstream>
#include<string>
#include<sstream>

#include "controller.h"

using namespace std;
using namespace XGHJ;

const int PLAYERS_PER_GAME = 8;
const int score_by_rank[PLAYERS_PER_GAME] = {5,3,2,2,1,1,0,0};

struct scoredAI
{
    Player p;
    string name;
    int score;
    scoredAI(Player &_p, const string& _name)
        :score(0), name(_name), p(_p)
    {
        ;
    }
};

std::string replaceAll( std::string const& original, std::string const& from, std::string const& to )
{
    std::string results;
    std::string::const_iterator end = original.end();
    std::string::const_iterator current = original.begin();
    std::string::const_iterator next = std::search( current, end, from.begin(), from.end() );
    while ( next != end ) {
        results.append( current, next );
        results.append( to );
        current = next + from.size();
        next = std::search( current, end, from.begin(), from.end() );
    }
    results.append( current, next );
    return results;
}


void usage()
{
    cout<<"the tournament eats a specific config path"<<endl;
    cout<<"../config_tournament_msvc.ini in MSVC"<<endl;
    cout<<"OR ../config_tournament_gnu.ini in GNU"<<endl;
}

int main(int argc, char** argv) 
{
    string  config_tournament_filename = 
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

    ifstream ifs(config_tournament_filename.c_str());    
    if(!ifs){
        cout<<"Failed to load \""<< config_tournament_filename << "\". Aborted. " << endl;
        return -1;
    }
    ifs >> map_filename;   
    ifs >> mknl_filename;   
    ifs >> weak_ai_filename;   
    while (!ifs.eof())         
    {
        string player_filename;
        ifs >> player_filename; 
        if (!player_filename.empty()) players_filename.push_back(player_filename);
    }


    if (players_filename.size()==0) 
    {
        cout << "No player ai file's names inputed. Aborted. " << endl;
        return -1;
    }

    Map map = Map();
	if (!map.easy_load(map_filename)) {
        cout << "Map failed" << endl;
        return -1;
    }

    for (size_t i=0; i<players_filename.size(); ++i)
    {
        Player player(players_filename[i], i);
        if (player.isValid()) 
        {
            players.push_back(scoredAI(player, players_filename[i]+"#"+std::to_string(i)));
        }
    }

    if (players.size() == 0)
    {
        cout << "No valid player ai Loaded. Aborted." << endl;
        return -1;
    }
    cout <<  players.size() << " players loaded." << endl;

    cout <<  "loading weak players" << endl;
    for (size_t i=0; i<PLAYERS_PER_GAME; ++i)
    {
        Player player(weak_ai_filename, i);
        if (player.isValid()) 
        {
            players.push_back(scoredAI(player, "weak_ai_dummy#"+std::to_string(i)));
        }
    }

    vector<vector<float> > militaryKernel;
    {
        ifstream ifs(mknl_filename);
        if(!ifs){
            cout<<"Failed to load \""<< mknl_filename << "\". Aborted. " << endl;
            return -1;
        }
        string line, linereplaced;
        stringstream ss;

        militaryKernel.resize(2*MILITARY_KERNEL_SIZE-1);
        for (int i=0; i<2*MILITARY_KERNEL_SIZE-1; i++)
        {
            while (!ifs.eof())
            {
                getline(ifs, line);
                if (line.c_str()[0] == '|')
                {
                    break;
                }
            }

            linereplaced = replaceAll(line, "|", "");
            ss = stringstream(linereplaced);

            militaryKernel[i].resize(2*MILITARY_KERNEL_SIZE-1);
            for (int j=0; j<2*MILITARY_KERNEL_SIZE-1; j++)
            {
                float f = 0;
                ss>>f;
                militaryKernel[i][j] = f;
            }
        }
    }

    for (int tour_round=1; tour_round<10; tour_round++)
    {
        cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
        cout << "-=-=-=-=-=-=-=-=-= SWISS-SYSTEM TOURNAMENT ROUND "<<tour_round << endl;
        cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;


        vector<Player> game_players;
        game_players.clear();
        int c=0;
        for (int i=0; i<players.size(); i++)
        {
            players[i].p.load();
            game_players.push_back(players[i].p);
            if (game_players.size() == PLAYERS_PER_GAME)
            {
                // sheng si kan dan bu fu jiu gan
                Game game(map, militaryKernel, PLAYERS_PER_GAME);
                Controller controller(game, game_players);
                while (controller.isValid())
                {
                    controller.run();
                } 
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
        //sort the scores
        std::sort(players.begin(), players.end(), 
            [](const scoredAI &a, const scoredAI &b) -> bool {
                return a.score > b.score;
        }
        );
        for (int i=0; i<players.size(); i++)
        {
            cout<<setw(30)<<players[i].name<<" : "<<players[i].score<<endl;
        }
    }


    

	return 0;

} 
