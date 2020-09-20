#pragma once
#ifndef MYIMAGEFUSION_H
#define MYIMAGEFUSION_H

#include"CImg.h"
#include"MyLaplacianBlending.h"
#include <iostream>
using namespace cimg_library;
//ͼ����ں��࣬������ѷ����
class MyImageFusion
{
public:
	MyImageFusion();
	
	CImg<int> bestSeamLineGetImage(CImg<int> leftImage, CImg<int> rightImage, int matchVecdx, int matchVecdy);
	//������ѷ���ߵ�ƴ������
	//���ڿ��Լ���ά��
	//3-29�޶��棬������ƫ��������ƫ�ƣ�ͬʱֻ���ص㼯����ʡ�ռ�
	//ͬʱ�Ż�����ṹ
	CImg<int> bestSeamLineGetLinePoint(CImg<int> leftImage1, CImg<int> rightImage2, int matchVecdx, int matchVecdy);
	
	//�滭��ѷ���ߣ���Ҫ�����ص������ͼ���Լ���ѷ���ߵĵ㼯��
//rectImage�ص������ͼ��bestSeamLinePathһά����ѷ���ߵ㼯
	CImg<int> drawbestSeamLine(CImg<int> rectImage, CImg<int> bestSeamLinePath);
	
	//���ط��� ����������ѷ�����ںϣ�������õ�ʱ���������룬��߼�����
	//2019-4-15��ȫ���ϰ棬������ɶ�ֱ����ںϣ��Լ���ѷ�����ںϣ����Ǵ��ڶ�δ�ȡͼ����������������޸�
	//������ͼ���ͨ������Ϊ4�㣬���Ĳ�ר�����������ѷ���ߵ���ɫ�ռ�仯����
	//4-17 ����GUI�������ģ�
	//1.����string����ж�ѡ����Ҫֱ��ͼ���ںϣ�����Ҫ��ֱ����ںϣ��Ƿ񻭳���ѷ����
	//isLaplacianBlending �Ƿ��ֱ����ں� Ĭ�Ͽ���
	//isDrawbestSeamLine �Ƿ񻭳���ѷ���� Ĭ�ϹرնԷ��������ع�
	//isLaplacianBlending == "use LaplacianBlending"  ������˹�ں�
	//isLaplacianBlending == "no use LaplacianBlending"  ������������˹�ں�
	//isDrawbestSeamLine == "DrawbestSeamLine" ������ѷ����
	//isDrawbestSeamLine == "not DrawbestSeamLine" ��������ѷ����
	CImg<int> bestSeamLineGetLinePoint1(char* inputTestAddr1, char* inputTestAddr2, int matchVecdx, int matchVecdy);

	CImg<int> bestSeamLineGetLinePoint1(char* inputTestAddr1, char* inputTestAddr2, int matchVecdx, int matchVecdy, char* isLaplacianBlending, char* isDrawbestSeamLine);
	
	~MyImageFusion();




};


#endif

