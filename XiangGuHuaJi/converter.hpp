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

using std::vector;

template<typename T> void convertMat(cv::Mat mat, vector<vector<T> >& vec)
{
    vec.resize(mat.cols);
	for (int j=0; j<mat.cols; ++j)
	{
		vec[j].resize(mat.rows);
		for (int i=0; i<mat.rows; ++i)
		{
			vec[j][i]=mat.at<T>(i, j);
		}
	}
}

template<typename T> void convertMat(vector<cv::Mat> vecmat, vector<vector<vector<T> > >& vec)
{
	int c = vecmat.size();
	vec.resize(c);
	for (int k=0; k<c; ++k)
	{
		vec[k].resize(vecmat[k].cols);
		for (int j=0; j<vecmat[k].cols; ++j)
		{
			vec[k][j].resize(vecmat[k].rows);
			for (int i=0; i<vecmat[k].rows; ++i)
			{
				vec[k][j][i]=vecmat[k].at<T>(i, j);
			}
		}
	}
}

// convert cv::Mat to vector<vector<T> >
template<typename T> vector<vector<T> > getConvertedMat(cv::Mat mat)
{
	vector<vector<T> > vec;
	convertMat(mat, vec);
	return vec;
}
// convert vector<cv::Mat> to vector<vector<vector<T> > >
template<typename T> vector<vector<vector<T> > > getConvertedMat(vector<cv::Mat> vecmat)
{
	vector<vector<vector<T> > > vec;
	convertMat(vecmat, vec);
	return vec;
}

template<typename T> void resizeVector(vector<vector<T> >&vec, cv::Mat mat)
{
	vec.resize(mat.cols);
	for (TMapSize j=0; j<mat.cols; ++j)
		vec[j].resize(mat.rows);
}
template<typename T> void resizeVector(vector<vector<T> >&vec, int cols, int rows)
{
    vec.resize(cols);
	for (TMapSize j=0; j<cols; ++j)
		vec[j].resize(rows);
}

template<typename T> vector<vector<T> > getResizedVector(int cols, int rows)
{
	vector<vector<T> > vec;
	resizeVector(vec, cols, rows);
	return vec;
}

template<typename T> vector<vector<vector<T> > > getResizedVector(int cols, int rows, int count)
{
	vector<vector<vector<T> > > vec;
	vec.resize(count);
	for (int i=0; i<count; i++)
	{
		vec[i] = getResizedVector(cols, rows);
	}
	return vec;
}

template<typename T> void fillVector(vector<vector<T> > vec, T val)
{
	for (size_t i=0; i<vec.size(); i++)
	{
		for (size_t j=0; j<vec[i].size(); j++)
		{
			vec[i][j] = val;
		}
	}
}

template<typename T> void fillVector(vector<vector<vector<T> > > vec, T val)
{
	for (size_t i=0; i<vec.size(); i++)
	{
		fillVector(vec[i], val);
	}
}

template<typename T> void resizeVector(vector<vector<T> >&vec, cv::Size size)
{
    vec.resize(size.width);
    for (TMapSize j=0; j<size.width; ++j)
        vec[j].resize(size.height);
}

template<typename T> bool convertVector(vector<vector<T> >& vec, cv::Mat mat)
{
    try
    {
		for (TMapSize j=0; j<mat.cols; ++j)
			for (TMapSize i=0; i<mat.rows; ++i) mat.at<T>(i, j)=vec[j][i];
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
