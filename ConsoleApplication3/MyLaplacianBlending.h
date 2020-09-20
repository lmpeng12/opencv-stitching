#pragma once
//#ifndef MYLAPLACIANBLENDING_H
//#define MYLAPLACIANBLENDING_H
//#endif
//������Ҫ���ڶ�ֱ����ں��㷨��ʵ��
//���ڻ��ٴν��н�һ��������޸�
//��������ʱ������̫���ʵʱ��
//���Ƚ��ص������ͼ������ȥOutput/temp���
//�ںϺ�ͨ����ȡͼƬ���в���


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
//2019-4-11 ���и����޸Ĵ���
//���ڶ�ֱ��ʣ���Ƶ�ף�ͼ���ں�
class MyLaplacianBlending {
	public:
		//������˹ͼ���ں�
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
	void buildGaussianPyramid() {//����������Ϊÿһ�����ģ
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

	//����������ֵĲ���
	void buildLaplacianPyramid(const Mat& img, vector<Mat_<Vec3f> >& lapPyr, Mat& HighestLevel) {
		lapPyr.clear();
		Mat currentImg = img;
		for (int l = 0; l < levels; l++) {
			Mat down, up;
			pyrDown(currentImg, down);
			pyrUp(down, up, currentImg.size());
			//������ʵ��������˹���ͼ��
			Mat lap = currentImg - up;
			lapPyr.push_back(lap);
			currentImg = down;
		}
		currentImg.copyTo(HighestLevel);
	}
	void blendLapPyrs()
	{
		resultHighestLevel = leftHighestLevel.mul(maskGaussianPyramid.back()) + rightHighestLevel.mul(Scalar(1.0, 1.0, 1.0) - maskGaussianPyramid.back());
		//���ϵõ�������߽׵���һ�׵��ں�ͼ
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
		//������laplacianͼ��ƴ�ɵ�resultLapPyr��������ÿһ��
		//���ϵ��²�ֵ�Ŵ���ӣ�����blendͼ����
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
	//��������
	~MyLaplacianBlending();
#endif // !__debug
