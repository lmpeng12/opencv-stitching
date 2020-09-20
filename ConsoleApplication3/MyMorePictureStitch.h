#pragma once

#ifndef MYMOREPICTURESTITCH_H
#define MYMOREPICTURESTITCH_H

#include"MyImageFusion.h"
#include "CImg.h"

#include <iostream>

using namespace cimg_library;
using namespace std;

//struct TransVector {
//	int dx;
//	int dy;
//	TransVector() : dx(-1), dy(-1) {}
//	TransVector(int _dx, int _dy) : dx(_dx), dy(_dy) {}
//};

//记录高度变化的结构体，相差的高度，同时
//存储两个图像的名字
//左节点
//右节点
struct HeightRecord
{
	//dx就相当于重叠区域的宽度
	//dy就相当于重叠区域的相上下图像偏移量
	//同时关系式子重叠区域的高度=待拼接图像高度-dy
	int dy;
	int dx;
	char* img1_name;
	char* img2_name;
	struct HeightRecord *rightRecord;
	struct HeightRecord *left;
};

class MyMorePictureStitch
{
public:
	MyMorePictureStitch();
	~MyMorePictureStitch();
	MyMorePictureStitch(int sx, int sy);

	//输入上一个拼接好的图片，同时输入两个匹配的图片
	void blendingMainProcess(char* _filenameA, char* _filenameB,CImg<int> lastblendedImg);

	//自己测试的拼接图像的方法
	CImg<int> MyStitchImage(char* _filenameA, char* _filenameB, CImg<int> lastblendedImg,int matchVecdx, int matchVecdy);

	////自己测试的拼接图像的方法v2
	//渐入渐出融合
	CImg<int> MyStitchImageByGradualFusionBlend(char* _filenameA, char* _filenameB, CImg<int> lastblendedImg, int matchVecdx, int matchVecdy);

	////自己测试的拼接图像的方法v3 2019-3-31
	//多分辨率融合
	CImg<int> MyStitchImageByOptimalSeamBlend(char* _filenameA, char* _filenameB, CImg<int> lastblendedImg, int matchVecdx, int matchVecdy, vector<char*> Other);

	//2019-4-17 v4版本
	//控制总类，外部接口可通过字符串的方式，判断哪种拼接方式，以及后续的拼接操作
	//char* StitchType 拼接类型
	//vector<char*> Others跟随拼接类型中传递的其它参数，将其构成一个数组
	CImg<int> MyStitchImage4(char* _filenameA, char* _filenameB, CImg<int> lastblendedImg, int matchVecdx, int matchVecdy
		,char* StitchType, vector<char*> Others
	);

	//重载代码

	CImg<int> MyStitchImage4(char* _filenameA, char* _filenameB, CImg<int> lastblendedImg, int matchVecdx, int matchVecdy, char* StitchType);
	CImg<int> MyStitchImage4(char* _filenameA, char* _filenameB, CImg<int> lastblendedImg, int matchVecdx, int matchVecdy);


	void saveBlendedImg(char* blendedImgAddr);
	//存储两幅图像之间的差值
	void SaveSequentialImagesDifference(char* _filenameA, char* _filenameB,int sx, int sy);

	CImg<int> getBlendedImg();
	//记录下当前拼接的图像的数量
	int StitchPictureNum = 0;

	MyImageFusion myImageFusio = MyImageFusion();

private:
//	TransVector matchVec;    //x为合并图上的水平距离，y
	
	CImg<int> blendedImg;
	//头结点，以及当前节点
	struct HeightRecord *head, *nowRecord;

	//记录最下，最上偏移量，最左，最右节点
	int MaxDown = 0;
	
	int MaxUp = 0;
	int MaxLeft = 0;
	int MaxRight = 0;

	//记录上次的高度偏移叠加计算
	//记录上次的宽度偏移叠加量
	int lastcalureHeight = 0;
	int lastcalureWeight = 0;
	
	int oriImageWidth = 0;
	int oriImageHeight = 0;
};

#endif