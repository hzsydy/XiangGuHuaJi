/* XiangGuHuaJi 2016, player.h
 *
 */

#ifndef _XIANGGUHUAJI_PLAYER_H__
#define _XIANGGUHUAJI_PLAYER_H__


#include<string>

#include"definition.h"

#include "ai.h"

#ifdef _MSC_VER
#include"windows.h"
#define __FUCK_LOADDLL(x) LoadLibraryA(x)
#define __FUCK_CLOSEDLL(x) FreeLibrary(x)
#define __FUCK_GETFUNC GetProcAddress
typedef HMODULE __FUCK_DLLHANDLE;
#endif
#ifdef __GNUC__
#include"dlfcn.h"
#define __FUCK_LOADDLL(x) dlopen(x, RTLD_LAZY)
#define __FUCK_CLOSEDLL(x) dlclose(x)
#define __FUCK_GETFUNC dlsym
typedef void* __FUCK_DLLHANDLE;
#endif

namespace XGHJ {

using namespace std;

typedef void (*TPlayerAi)(Info& info); 
typedef TMoney (*TBirthplacePrice)(void);
typedef TPosition (*TBirthplace)(vector<TPosition> posSelected);

class Player
{
public:
    Player();
    Player(string file_name, int id);
    ~Player();

    bool run(Info &info);
    bool run(TMoney &price);
    bool run(TPosition &pos, vector<TPosition> &posSelected);

    bool load();

    inline void kill() {Valid = false;}

    inline bool isValid() { return Valid; }

private:
    TPlayerAi        player_ai;
    TBirthplacePrice  birthplacePrice;
    TBirthplace       birthplace;
    string           file_name;
    TId              id;
    bool             Valid;
    __FUCK_DLLHANDLE hDLL;
};

}


#endif
