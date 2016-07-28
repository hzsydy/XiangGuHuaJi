/* XiangGuHuaJi 2016, debug.hpp
 * wtf
 * 
 */

#ifndef _XIANGGUHUAJI_DEBUG_HPP__
#define _XIANGGUHUAJI_DEBUG_HPP__
#pragma once

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>

using std::cout;
using std::endl;
using std::vector;
using std::string;


const char separator  = ' ';

#include <opencv2/opencv.hpp>

#define GAME_DEBUG

//肮脏的辅助函数 用来打印一个矩阵
template<typename T> 
inline void printElement(T t)
{
	cout << std::left << std::setw(6) << std::setfill(separator) << t;
}

template<> 
inline void printElement<float>(float t)
{
	const static char separator  = ' ';
	cout << std::left << std::setw(6) << std::setprecision(2) << std::setfill(separator) << t;
}

template<> 
inline void printElement<unsigned char>(unsigned char t)
{
	const static char separator  = ' ';
	cout << std::left << std::setw(6) << std::setfill(separator) << (int)t;
}

template <typename T>
void printMat(cv::Mat m, string name)
{
	cout<<name<<endl;
	for (int i=0; i<m.rows; i++)
	{
		for (int j=0; j<m.cols; j++)
		{
			printElement<T>(m.at<T>(i, j));
		}
		cout<<endl;
	}
}

template <typename vecT>
void printVecMat(vector<vector<vecT>> m, string name)
{
	cout<<name<<endl;
	for (int i=0; i<m.size(); i++)
	{
		for (int j=0; j<m[0].size(); j++)
		{
			printElement<vecT>(m[i][j]);
		}
		cout<<endl;
	}
}

#endif
