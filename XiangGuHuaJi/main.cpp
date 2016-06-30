/* XiangGuHuaJi 2016, main.cpp
 *
 */

#include<iostream>  
#include<fstream>
#include<string>
#include<opencv2/opencv.hpp>

#include "controller.h"

using namespace std;
using namespace XGHJ;


string  config_filename =
#ifdef _MSC_VER
    "../config_msvc.ini";
#endif
#ifdef __GNUC__
    "../config_linux.ini";
#endif    

int main(int argc, char** argv) 
{
    if (argc>=2) 
    {
        config_filename = argv[1];
    }

    string          map_filename; 
    vector<string>  players_filename;
    vector<Player>  players;

    try 
    { 
        ifstream ifs(config_filename.c_str());    
        ifs >> map_filename;        
        while (!ifs.eof())         
        {
            string player_filename;
            ifs >> player_filename; 
            if (!player_filename.empty()) players_filename.push_back(player_filename);
        }
    }
    catch(...) 
    {
        cout<<"Failed to load \""<< config_filename << "\". Aborted. " << endl;
        return -1;
    }
    if (players_filename.size()==0) 
    {
        cout << "No player ai file's names inputed. Aborted. " << endl;
        return -1;
    }

    Map map = Map();
	map.load(map_filename);    

    for (size_t i=0; i<players_filename.size(); ++i)
    {
        Player player(players_filename[i], i);
        if (player.isValid()) players.push_back(player);
    }
    if (players.size() == 0)
    {
        cout << "No valid player ai Loaded. Aborted." << endl;
        return -1;
    }
    cout <<  players.size() << " players loaded." << endl;

    Game game(map, players.size());
	Controller controller(game, players);

    
    while (controller.isValid())
    {
        controller.Run();
    } 

#ifdef _MSC_VER
    system("pause");
#endif

	return 0;

} 
