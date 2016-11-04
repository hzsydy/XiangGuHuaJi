/* XiangGuHuaJi 2016, main.cpp
 *
 */

#include<iostream>  
#include<fstream>
#include<string>

#include "controller.h"

using namespace std;
using namespace XGHJ;


const string  config_filename =
#ifdef _MSC_VER
    "../config_msvc.ini";
#endif
#ifdef __GNUC__
    "../config_linux.ini";
#endif    

int main(int argc, char** argv) 
{
	vector<Player>  players;
	string          map_filename; 
	vector<string>  players_filename;
	if (argc == 1) 
    {
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
    }
	else if(argc >= 3)
	{
		map_filename = string(argv[1]);
		for (int i=2; i<argc; i++)
		{
			string player_filename;
			player_filename = string(argv[argc]);
			players_filename.push_back(player_filename);
		}
	}
	else
	{
		cout<<"usage:												"<<endl
			<<"XiangGuHuaji						Load config file	"<<endl
			<<"XiangGuHuaji	map	dll1 dll2 ...	assign file to load	"<<endl
		;
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
        controller.run();
    } 

#ifdef _MSC_VER
    system("pause");
#endif

	return 0;

} 
