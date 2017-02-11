/* XiangGuHuaJi 2016, main.cpp
 *
 */

#include<iostream>  
#include<fstream>
#include<string>
#include<sstream>

#include "controller.h"

using namespace std;
using namespace XGHJ;

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

int main(int argc, char** argv) 
{
    string  config_filename = "../config_msvc.ini";

    if (argc ==2) {
        config_filename = argv[1];
    }
    else if (argc>=2) {
		cout<<"usage:												"<<endl
			<<"XiangGuHuaji						Load config file	"<<endl
		;
	}


	vector<Player>  players;
    string          map_filename; 
    string          mknl_filename; 
	vector<string>  players_filename;

    ifstream ifs(config_filename.c_str());    
    if(!ifs){
        cout<<"Failed to load \""<< config_filename << "\". Aborted. " << endl;
        return -1;
    }
    ifs >> map_filename;   
    ifs >> mknl_filename;   
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
        system("pause");
        return -1;
    }

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

    Game game(map, militaryKernel, players.size());
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
