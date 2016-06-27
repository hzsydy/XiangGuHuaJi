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

int main() 
{
	MapParser mp;
	mp.readjson("test.json");
	

    Player player("TestAi.dll", 0);
    

    system("pause");
	return 0;

} 