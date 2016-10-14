/* XiangGuHuaJi 2016, ai.h
 *
 */

#ifndef _XIANGGUHUAJI_AI_H__
#define _XIANGGUHUAJI_AI_H__

#include "definition.h"

//AI需要实现的函数
#ifdef _MSC_VER
extern "C" _declspec(dllexport) void player_ai(Info& info);
extern "C" _declspec(dllexport) TMoney birthplacePrice(void);
extern "C" _declspec(dllexport) TPosition birthplace(vector<TPosition> posSelected);
#endif
#ifdef __GNUC__
extern "C" void player_ai(Info& info);
extern "C" TMoney birthplacePrice(void);
extern "C" TPosition birthplace(vector<TPosition> posSelected);
#endif

#endif