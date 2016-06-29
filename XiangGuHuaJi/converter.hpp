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
    vec.resize(mat.rows);
    for (TMapSize i=0; i<mat.rows; ++i)
    {
        vec[i].resize(mat.cols);
        for (TMapSize j=0; j<mat.cols; ++j) vec[i][j]=mat.at<T>(i, j);
    }
}

template<typename T> void resizeVector(vector<vector<T> >&vec, cv::Mat mat)
{
    vec.resize(mat.rows);
    for (TMapSize i=0; i<mat.rows; ++i) vec[i].resize(mat.cols);
}

template<typename T> bool convertVector(vector<vector<T> >& vec, cv::Mat mat)
{
    try
    {
        for (TMapSize i=0; i<mat.rows; ++i)
            for (TMapSize j=0; j<mat.cols; ++j)
                mat.at<T>(i,j) = vec[i][j];
    }
    catch(exception e)
    {
        cout<<"[ERROR] Failed to convert vector<vector<T> > to cv::Mat. "<<endl;
        throw e;
    }
    return true;
}

}

#endif