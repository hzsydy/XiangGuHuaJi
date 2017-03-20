/* XiangGuHuaJi 2016, ai.h
 *
 */

#ifndef _XIANGGUHUAJI_AI_H__
#define _XIANGGUHUAJI_AI_H__

#include "definition.h"

#ifdef _MSC_VER
extern "C" _declspec(dllexport) TMoney birthplacePrice(BaseMap* map);
extern "C" _declspec(dllexport) TPosition birthplace(vector<TPosition>& posSelected, BaseMap* map);

extern "C" _declspec(dllexport) void player_ai(Info& info);
#endif


#ifdef __GNUC__

extern "C" TMoney birthplacePrice(BaseMap* map);
extern "C" TPosition birthplace(vector<TPosition>& posSelected, BaseMap* map);

extern "C" void player_ai(Info& info);

#endif


#endif
