#pragma once
//#ifndef MYLAPLACIANBLENDING_H
//#define MYLAPLACIANBLENDING_H
//#endif
//此类主要用于多分辨率融合算法的实现
//后期会再次进行进一步的理解修改
//这里先暂时不考虑太多的实时性
//首先将重叠区域的图像存入进去Output/temp里边
//融合后通过读取图片进行操作


#include "gaussianpyramid.hpp"
#include "laplacianpyramid.hpp"
#include <string.h>

#include "CImg.h"
#include <iostream>
#include <string>
#include <vector>

#define cimg_use_magick
using namespace std;
using namespace cimg_library;
//2019-4-11 进行复制修改代码
//基于多分辨率（多频谱）图像融合
class MyLaplacianBlending {
	public:
		//拉普拉斯图像融合
		CImg<double> MyLaplacianBlend(std::string leftImageString, std::string rightImageString, std::string maskNameString);
		MyLaplacianBlending();
		~MyLaplacianBlending();

};

#ifdef __debug

#include <opencv2/opencv.hpp>
using namespace cv;
private:
	Mat_<Vec3f> left;
	Mat_<Vec3f> right;
	Mat_<float> blendMask;
	vector<Mat_<Vec3f> > leftLapPyr, rightLapPyr, resultLapPyr;//Laplacian Pyramids
	Mat leftHighestLevel, rightHighestLevel, resultHighestLevel;
	vector<Mat_<Vec3f> > maskGaussianPyramid; //masks are 3-channels for easier multiplication with RGB
	int levels;
	void buildPyramids() {
		buildLaplacianPyramid(left, leftLapPyr, leftHighestLevel);
		buildLaplacianPyramid(right, rightLapPyr, rightHighestLevel);
		buildGaussianPyramid();
	}
	void buildGaussianPyramid() {//金字塔内容为每一层的掩模
		assert(leftLapPyr.size() > 0);
		maskGaussianPyramid.clear();
		Mat currentImg;
		cvtColor(blendMask, currentImg, CV_GRAY2BGR);//store color img of blend mask into maskGaussianPyramid
		maskGaussianPyramid.push_back(currentImg); //0-level
		currentImg = blendMask;
		for (int l = 1; l < levels + 1; l++) {
			Mat _down;
			if (leftLapPyr.size() > l)
				pyrDown(currentImg, _down, leftLapPyr[l].size());
			else
				pyrDown(currentImg, _down, leftHighestLevel.size()); //lowest level
			Mat down;
			cvtColor(_down, down, CV_GRAY2BGR);
			maskGaussianPyramid.push_back(down);//add color blend mask into mask Pyramid
			currentImg = _down;
		}
	}

	//这里有做差分的操作
	void buildLaplacianPyramid(const Mat& img, vector<Mat_<Vec3f> >& lapPyr, Mat& HighestLevel) {
		lapPyr.clear();
		Mat currentImg = img;
		for (int l = 0; l < levels; l++) {
			Mat down, up;
			pyrDown(currentImg, down);
			pyrUp(down, up, currentImg.size());
			//在这里实现拉普拉斯差分图像
			Mat lap = currentImg - up;
			lapPyr.push_back(lap);
			currentImg = down;
		}
		currentImg.copyTo(HighestLevel);
	}
	void blendLapPyrs()
	{
		resultHighestLevel = leftHighestLevel.mul(maskGaussianPyramid.back()) + rightHighestLevel.mul(Scalar(1.0, 1.0, 1.0) - maskGaussianPyramid.back());
		//以上得到的是最高阶的上一阶的融合图
		for (int l = 0; l < levels; l++)
		{
			Mat A = leftLapPyr[l].mul(maskGaussianPyramid[l]);
			Mat antiMask = Scalar(1.0, 1.0, 1.0) - maskGaussianPyramid[l];
			Mat B = rightLapPyr[l].mul(antiMask);
			Mat_<Vec3f> blendedLevel = A + B;
			resultLapPyr.push_back(blendedLevel);
		}
	}
	Mat_<Vec3f> reconstructImgFromLapPyramid() {
		//将左右laplacian图像拼成的resultLapPyr金字塔中每一层
		//从上到下插值放大并相加，即得blend图像结果
		Mat currentImg = resultHighestLevel;
		for (int l = levels - 1; l >= 0; l--) {
			Mat up;
			pyrUp(currentImg, up, resultLapPyr[l].size());
			currentImg = up + resultLapPyr[l];
		}
		return currentImg;
	}
public:
	MyLaplacianBlending(const Mat_<Vec3f>& _left, const Mat_<Vec3f>& _right, const Mat_<float>& _blendMask, int _levels) ://construct function, used in MyLaplacianBlending lb(l,r,m,4);
		left(_left), right(_right), blendMask(_blendMask), levels(_levels)
	{
		assert(_left.size() == _right.size());
		assert(_left.size() == _blendMask.size());
		buildPyramids(); //construct Laplacian Pyramid and Gaussian Pyramid
		blendLapPyrs(); //blend left & right Pyramids into one Pyramid
	};
	Mat_<Vec3f> blend() {
		return reconstructImgFromLapPyramid();//reconstruct Image from Laplacian Pyramid
	}

	
	char* getLaplacianBlendRectImage(char* leftImage, char* rightImage, char* templateImage);
	MyLaplacianBlending();
	//析构函数
	~MyLaplacianBlending();
#endif // !__debug
