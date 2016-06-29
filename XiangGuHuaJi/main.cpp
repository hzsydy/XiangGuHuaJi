/* XiangGuHuaJi 2016, main.cpp
 *
 */

#include<iostream>  
#include<fstream>
#include<string>
#include<opencv2/opencv.hpp>

#include "map.h"
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

    Map m = Map();
	//m.load("test.json");	
    
    Player player(TestAi_filename, 0);
    
#ifdef _MSC_VER
    system("pause");
#endif

	return 0;

} 
