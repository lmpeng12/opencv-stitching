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

//��¼�߶ȱ仯�Ľṹ�壬���ĸ߶ȣ�ͬʱ
//�洢����ͼ�������
//��ڵ�
//�ҽڵ�
struct HeightRecord
{
	//dx���൱���ص�����Ŀ��
	//dy���൱���ص������������ͼ��ƫ����
	//ͬʱ��ϵʽ���ص�����ĸ߶�=��ƴ��ͼ��߶�-dy
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

	//������һ��ƴ�Ӻõ�ͼƬ��ͬʱ��������ƥ���ͼƬ
	void blendingMainProcess(char* _filenameA, char* _filenameB,CImg<int> lastblendedImg);

	//�Լ����Ե�ƴ��ͼ��ķ���
	CImg<int> MyStitchImage(char* _filenameA, char* _filenameB, CImg<int> lastblendedImg,int matchVecdx, int matchVecdy);

	////�Լ����Ե�ƴ��ͼ��ķ���v2
	//���뽥���ں�
	CImg<int> MyStitchImageByGradualFusionBlend(char* _filenameA, char* _filenameB, CImg<int> lastblendedImg, int matchVecdx, int matchVecdy);

	////�Լ����Ե�ƴ��ͼ��ķ���v3 2019-3-31
	//��ֱ����ں�
	CImg<int> MyStitchImageByOptimalSeamBlend(char* _filenameA, char* _filenameB, CImg<int> lastblendedImg, int matchVecdx, int matchVecdy, vector<char*> Other);

	//2019-4-17 v4�汾
	//�������࣬�ⲿ�ӿڿ�ͨ���ַ����ķ�ʽ���ж�����ƴ�ӷ�ʽ���Լ�������ƴ�Ӳ���
	//char* StitchType ƴ������
	//vector<char*> Others����ƴ�������д��ݵ��������������乹��һ������
	CImg<int> MyStitchImage4(char* _filenameA, char* _filenameB, CImg<int> lastblendedImg, int matchVecdx, int matchVecdy
		,char* StitchType, vector<char*> Others
	);

	//���ش���

	CImg<int> MyStitchImage4(char* _filenameA, char* _filenameB, CImg<int> lastblendedImg, int matchVecdx, int matchVecdy, char* StitchType);
	CImg<int> MyStitchImage4(char* _filenameA, char* _filenameB, CImg<int> lastblendedImg, int matchVecdx, int matchVecdy);


	void saveBlendedImg(char* blendedImgAddr);
	//�洢����ͼ��֮��Ĳ�ֵ
	void SaveSequentialImagesDifference(char* _filenameA, char* _filenameB,int sx, int sy);

	CImg<int> getBlendedImg();
	//��¼�µ�ǰƴ�ӵ�ͼ�������
	int StitchPictureNum = 0;

	MyImageFusion myImageFusio = MyImageFusion();

private:
//	TransVector matchVec;    //xΪ�ϲ�ͼ�ϵ�ˮƽ���룬y
	
	CImg<int> blendedImg;
	//ͷ��㣬�Լ���ǰ�ڵ�
	struct HeightRecord *head, *nowRecord;

	//��¼���£�����ƫ�������������ҽڵ�
	int MaxDown = 0;
	
	int MaxUp = 0;
	int MaxLeft = 0;
	int MaxRight = 0;

	//��¼�ϴεĸ߶�ƫ�Ƶ��Ӽ���
	//��¼�ϴεĿ��ƫ�Ƶ�����
	int lastcalureHeight = 0;
	int lastcalureWeight = 0;
	
	int oriImageWidth = 0;
	int oriImageHeight = 0;
};

#endif