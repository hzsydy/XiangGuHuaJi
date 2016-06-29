/* XiangGuHuaJi 2016, converter.h
 *
 */

#ifndef _XIANGGUHUAJI_CONVERTER_H__
#define _XIANGGUHUAJI_CONVERTER_H__

#include<vector>
#include<iostream>
#include<opencv2/opencv.hpp>
#include"definition.h"

namespace XGHJ
{

using namespace std;

template<typename T> void convertMat(cv::Mat mat, vector<vector<T> >& vec)
{
	int fuck = mat.at<T>(0, 0);
    vec.resize(mat.cols);
	for (TMapSize j=0; j<mat.cols; ++j)
	{
		vec[j].resize(mat.rows);
		for (TMapSize i=0; i<mat.rows; ++i)
		{
			vec[j][i]=mat.at<T>(i, j);
		}
	}
    
}

template<typename T> void resizeVector(vector<vector<T> >&vec, cv::Mat mat)
{
	vec.resize(mat.cols);
	for (TMapSize j=0; j<mat.cols; ++j)
		vec[j].resize(mat.rows);
}

template<typename T> bool convertVector(vector<vector<T> >& vec, cv::Mat mat)
{
    try
    {
		for (TMapSize j=0; j<mat.cols; ++j)
		{
			for (TMapSize i=0; i<mat.rows; ++i)
			{
				vec[j][i]=mat.at<T>(i, j);
			}
		}
    }
    catch(exception e)
    {
        cerr<<"[ERROR] Failed to convert vector<vector<T> > to cv::Mat. "<<endl;
        throw e;
    }
    return true;
}

}

#endif