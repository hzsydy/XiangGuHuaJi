/* XiangGuHuaJi 2016, main.cpp
 *
 */

#include<iostream>  
#include<fstream>
#include<string>
#include<opencv2/opencv.hpp>

#include "MapParser.h"

#include"player.h"

using namespace std;
using namespace XGHJ;


string TestAi_filename =
#ifdef _MSC_VER
    "TestAi.dll";
#endif
#ifdef __GNUC__
    "./libTestAi.so";
#endif    

int main() 
{
	MapParser mp;
	mp.readjson("test.json");
	
    
    Player player(TestAi_filename, 0);
    
#ifdef _MSC_VER
    system("pause");
#endif

	return 0;

} 
