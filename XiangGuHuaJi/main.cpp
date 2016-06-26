#include<iostream>  
#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp>   

#include "MapParser.h"
using namespace XGHJ;

int main() 
{
	MapParser mp;
	mp.readjson("test.json");
	system("pause");
	return 0;
} 