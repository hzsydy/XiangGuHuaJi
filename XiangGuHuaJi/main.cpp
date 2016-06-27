/* XiangGuHuaJi 2016, main.cpp
 *
 */

#include<iostream>  
#include<fstream>
#include<string>
#include<opencv2/opencv.hpp>

#include "MapParser.h"

using namespace std;
using namespace XGHJ;


int main() 
{
	MapParser mp;
	mp.readjson("test.json");
	system("pause");
	return 0;

} 