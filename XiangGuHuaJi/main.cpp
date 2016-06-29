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

int main() 
{

    Player player("TestAi.dll", 0);
    Map m();
	m.load("fuck.json");

    system("pause");
	return 0;

} 