
// DIRTYMATRIX!!!

#ifndef __XIANGGUHUAJI_MATRIX_H__
#define __XIANGGUHUAJI_MATRIX_H__

#include <opencv2/opencv.hpp>
#include <vector>

template<typename T> std::vector<std::vector<T> > copyMatToVector(cv::Mat mat)
{
    std::vector v ( mat.rows, vector<T> ( mat.cols ) );
    for (TMapSize i=0; i<mat.rows; ++i)
        for (TMapSize j=0; j<mat.cols; ++j)
            v[i][j] = mat.at<T>(i, j);
    return v;
}

//template<typename T> T** newMatrix(TMapSize x, TMapSize y)
//{
//    T** p0 = new T*[x];
//    for (TMapSize i=0; i<x; ++i) p0[i] = new T[y];
//    return p0;
//}
//template<typename T> void copyMatrix(T** p0, cv::Mat mat, TMapSize x, TMapSize y)
//{
//    for (TMapSize i=0; i<x; ++i)
//        for (TMapSize j=0; j<y; ++j)
//            p0[i][j] = mat.at<T>(i,j);
//}
//template<typename T> T** newCopyMatrix(cv::Mat mat)
//{
//    T** p0 = newMatrix<T>(mat.rows, mat.cols);
//    copyMatrix(p0, mat, mat.rows, mat.cols);
//    return p0;
//}
//template<typename T> void releaseMatrix(T** p0, TMapSize x, TMapSize y)
//{
//    for (TMapSize i=0; i<x; ++i) delete[] p0[i];
//    delete[] p0;
//}

#endif