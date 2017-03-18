/* XiangGuHuaJi 2016, player.h
 *
 */

#ifndef _XIANGGUHUAJI_PLAYER_H__
#define _XIANGGUHUAJI_PLAYER_H__


#include<string>

#include"definition.h"

#include "ai.h"

#ifdef _MSC_VER
// Du 2017.2.6
// EXCLUDE windows sockets
#define WIN32_LEAN_AND_MEAN
#include"windows.h"
#define __FUCK_LOADDLL(x) LoadLibraryA(x)
#define __FUCK_CLOSEDLL(x) FreeLibrary(x)
#define __FUCK_GETFUNC GetProcAddress
typedef HMODULE __FUCK_DLLHANDLE;
#endif
#ifdef __GNUC__
#include<dlfcn.h>
#define __FUCK_LOADDLL(x) dlopen(x, RTLD_LAZY)
#define __FUCK_CLOSEDLL(x) dlclose(x)
#define __FUCK_GETFUNC dlsym
typedef void* __FUCK_DLLHANDLE;
#endif

namespace XGHJ {

using namespace std;

typedef void (*TPlayerAi)(Info& info); 
typedef TMoney (*TBirthplacePrice)(BaseMap* map);
typedef TPosition (*TBirthplace)(vector<TPosition>& posSelected, BaseMap* map);

class Player
{
public:
    Player();
    Player(string file_name, int id);
    ~Player();

    bool run(Info &info);
    bool run(TMoney &price, BaseMap* map);
    bool run(TPosition &pos, vector<TPosition> &posSelected, BaseMap* map);

    bool load();

    inline void kill() {Valid = false;}
    
    inline string getName() { return name;}
    inline bool isValid() { return Valid; }

private:
    TPlayerAi        player_ai;
    TBirthplacePrice  birthplacePrice;
    TBirthplace       birthplace;
    string           file_name;
    string           name;
    TId              id;
    bool             Valid;
    __FUCK_DLLHANDLE hDLL;
};

}


#endif
