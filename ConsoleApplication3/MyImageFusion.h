#pragma once
#ifndef MYIMAGEFUSION_H
#define MYIMAGEFUSION_H

#include"CImg.h"
#include"MyLaplacianBlending.h"
#include <iostream>
using namespace cimg_library;
//图像的融合类，加上最佳缝合线
class MyImageFusion
{
public:
	MyImageFusion();
	
	CImg<int> bestSeamLineGetImage(CImg<int> leftImage, CImg<int> rightImage, int matchVecdx, int matchVecdy);
	//返回最佳缝合线的拼合区域
	//后期可以继续维护
	//3-29修订版，加上下偏移与左右偏移，同时只返回点集，节省空间
	//同时优化代码结构
	CImg<int> bestSeamLineGetLinePoint(CImg<int> leftImage1, CImg<int> rightImage2, int matchVecdx, int matchVecdy);
	
	//绘画最佳缝合线，需要传入重叠区域的图像以及最佳缝合线的点集，
//rectImage重叠区域的图像，bestSeamLinePath一维的最佳缝合线点集
	CImg<int> drawbestSeamLine(CImg<int> rectImage, CImg<int> bestSeamLinePath);
	
	//重载方法 都是依靠最佳缝合线融合，方便调用的时候忘记输入，提高兼容性
	//2019-4-15完全整合版，初步完成多分辨率融合，以及最佳缝合线融合，但是存在多次存取图像操作，可以慢慢修改
	//并且其图像的通道设置为4层，第四层专门用来存放最佳缝合线的颜色空间变化操作
	//4-17 继续GUI代码整改，
	//1.增加string语句判断选择是要直接图像融合，还是要多分辨率融合，是否画出最佳缝合线
	//isLaplacianBlending 是否多分辨率融合 默认开启
	//isDrawbestSeamLine 是否画出最佳缝合线 默认关闭对方法进行重构
	//isLaplacianBlending == "use LaplacianBlending"  拉普拉斯融合
	//isLaplacianBlending == "no use LaplacianBlending"  不进行拉普拉斯融合
	//isDrawbestSeamLine == "DrawbestSeamLine" 画出最佳缝合线
	//isDrawbestSeamLine == "not DrawbestSeamLine" 不画出最佳缝合线
	CImg<int> bestSeamLineGetLinePoint1(char* inputTestAddr1, char* inputTestAddr2, int matchVecdx, int matchVecdy);

	CImg<int> bestSeamLineGetLinePoint1(char* inputTestAddr1, char* inputTestAddr2, int matchVecdx, int matchVecdy, char* isLaplacianBlending, char* isDrawbestSeamLine);
	
	~MyImageFusion();




};


#endif

