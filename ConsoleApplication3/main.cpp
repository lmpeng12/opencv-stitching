#pragma warning(disable:4996)
#include "stdafx.h"
#include "MyMatching.h"
#include "MyBlending.h"
#include "MyMorePictureStitch.h"
#include "MyLaplacianBlending.h"
#include "CImg.h"

#include<string.h>
#include<iostream>
//#include<opencv2/opencv.hpp>
//#include"MyLaplacianBlending.h"

// 3-14�Ÿ������黰��ȡͼƬ�Ĳ���

//���Է���ʹ��4��ͼƬ���д���
#define N 2
//using namespace cv;
CImg<int> bestlinefusion1(CImg<int> leftImage, CImg<int> rightImage, int rdata1)
{
	//ͨ���۲������Կ�����rdata1���ص������ˮƽ����ȷ���ĳ��ȣ�
	//�ȸ���֮ǰ�õ���H��������ص�����Rect
	int  Weight;
	//Weight���,Height�߶�
	Weight = leftImage._width;
	int Height = leftImage._height;
	//rdata1=-118; ���Ӧ�����ұ���չ�Ŀ��
	//rdata1=fix(-rdata1);
	rdata1 = -rdata1;

	//�����ͼ������Ͻ�Ϊ����
	//rectLeftBorder�ص��������߽��x����
	//rectRightBorder�ص��������߽��x���꼴���ͼ��ı߽�
	int rectLeftBorder = Weight + rdata1;
	//rectRightBorder=Weight;
	int rectRightBorder = Weight;
	//n=rectRightBorder-rectLeftBorder+1;
	//����ֻ�Ǽ����ص������n���ѣ������������壬����������
	int n = rectRightBorder - rectLeftBorder;


	//�����ں�ͼ�Ĵ�С
	//rdata1=-118;
	//rdata2=3;
	//�˴���YӦ�������ºϳ�ͼ��Ŀ��
	int Y = 2 * Weight + rdata1 ;

	CImg<int> D = CImg<int>(Y,Height , 1, 3, 0);

	//����õ��ص�����Ĳ�ֵͼ��  ��ʵ�Ҳ��������ֵͼ�����  ֻҪ�����ص�����Ĵ�С�ͺ��� Ϊʲô��Ҫ�����ֵͼ ������û�õ�
	//Rect=zeros(Height,n);
	//Rectf Ϊ�ص�����Ĳ�ֵͼ��
	CImg<int> Rectf = CImg<int>(leftImage._width, leftImage._height, 1, 3, 0);
	//#define leftImage(x,y) leftImage(x,y)[0] ��ʵ���ֻ����������ȶ��ѣ�����matlab
	//#define rightImage(x,y) rightImage(x,y)[0]

		//imageChannelNum ͼ���ͨ����
	int imageChannelNum = 3;
	//����Ӧ�����ص���������ֵ������ɣ���Ҫ�޸���
	//for i=1:Height
	//����û�и߶�ƫ�ƣ��еĻ�������Ҫ�޸�

	for (int i = 0;i < Height;i++) {
		//	for j=rectLeftBorder:rectRightBorder
		//rectLeftBorder�ص��������ʼλ��
		for (int j = rectLeftBorder;j < rectRightBorder;j++) {
			for (int k = 0;k < imageChannelNum;k++)
			{
				Rectf(i, j - rectLeftBorder + 1, 0, k) = leftImage(i, j, 0, k) - rightImage(i, j - rectLeftBorder + 1, 0, k);
			}
			//Rectf(i,j-rectLeftBorder)=A(i,j)[0]-B(i,j-rectLeftBorder+1)[0];
		}//end
	}//end
	//Rectf=uint8(Rectf);//���Ҫ��Ҫ�أ�




	//path��·���ľ���
	//����Ϊ�����ά����,��path����Ϊ2ά���飬��ֻ���һά��ֵ
	CImg<int> path = CImg<int>(n, Height, 1, 1, 0);
	//��ǿ��ֵ ÿ��·����ǿ��ֵstrength=color^2+geometry
	CImg<int> color = CImg<int>(n, 1, 1, 1, 0);
	CImg<int> geometry = CImg<int>(n, 1, 1, 1, 0);

	//leftImageStrength���ͼ���ǿ��
	//rightImageStrength�ұ�ͼ���ǿ��
	CImg<int> leftImageStrength = CImg<int>(n, 1, 1, 3, 0);
	CImg<int> rightImageStrength = CImg<int>(n, 1, 1, 3, 0);

	float Bxdao;
	float Bydao;
	float Aydao;
	float Axdao;

	//�����һ�м���ʼ����ǿ��ֵ
	//for j=rectLeftBorder:rectRightBorder
	for (int j = rectLeftBorder;j < rectRightBorder;j++)
	{
		int y = j - rectLeftBorder;
		color(y, 0, 0, 0) = leftImage(1, j, 0, 0) - rightImage(1, y, 0, 0);
		if (y == 0)
		{
			Bxdao = rightImage(y + 1, 0, 0, 0) + 2 * rightImage(y + 1, 1, 0, 0);
			Bydao = rightImage(y, 1, 0, 0) + 2 * rightImage(y + 1, 1, 0, 0);
			Aydao = 2 * leftImage(j - 1, 1, 0, 0) + leftImage(j, 1, 0, 0) + 2 * leftImage(j + 1, 1, 0, 0);
			Axdao = leftImage(j + 1, 0, 0, 0) + 2 * leftImage(j + 1, 1, 0, 0) - leftImage(j - 1, 0, 0, 0) - 2 * leftImage(j - 1, 1, 0, 0);
			geometry(y, 0, 0, 0) = (Axdao - Bxdao)*(Aydao - Bydao);
			leftImageStrength(y, 0, 0, 0) = sqrt(color(y, 0, 0, 0)) + geometry(y, 0, 0, 0);
			path(y, 0, 0, 0) = y;
			continue;
		}
		//end
		if (j == rectRightBorder-1)
		{
			Axdao = leftImage(j - 1, 0, 0, 0) - 2 * leftImage(j - 1, 1, 0, 0);
			Aydao = 2 * leftImage(j - 1, 1, 0, 0) + leftImage(j, 1, 0, 0);
			Bxdao = rightImage(y + 1, 0, 0, 0) + 2 * rightImage(y + 1, 1, 0, 0) - rightImage(y - 1, 0, 0, 0) - 2 * rightImage(y - 1, 1, 0, 0);
			Bydao = 2 * rightImage(y - 1, 1, 0, 0) + rightImage(y, 1, 0, 0) + 2 * rightImage(y + 1, 1, 0, 0);
			geometry(y, 0, 0, 0) = (Axdao - Bxdao)*(Aydao - Bydao);
			leftImageStrength(y, 0, 0, 0) = sqrt(color(y, 0, 0, 0)) + geometry(y, 0, 0, 0);
			path(y, 0, 0, 0) = y;
			continue;
		}
		//end
		Axdao = rightImage(y + 1, 0, 0, 0) + 2 * rightImage(y + 1, 1, 0, 0) - leftImage(j - 1, 0, 0, 0) - 2 * leftImage(j - 1, 1, 0, 0);
		Bxdao = rightImage(y + 1, 0, 0, 0) + 2 * rightImage(y + 1, 1, 0, 0) - rightImage(y - 1, 0, 0, 0) - 2 * rightImage(y - 1, 1, 0, 0);
		Aydao = 2 * leftImage(j - 1, 1, 0, 0) + leftImage(j, 1, 0, 0) + 2 * leftImage(j + 1, 1, 0, 0);
		Bydao = 2 * rightImage(y - 1, 1, 0, 0) + rightImage(y, 1, 0, 0) + 2 * rightImage(y + 1, 1, 0, 0);
		geometry(y, 0, 0, 0) = (Axdao - Bxdao)*(Aydao - Bydao);
		leftImageStrength(y, 0, 0, 0) = sqrt(color(y, 0, 0, 0)) - geometry(y, 0, 0, 0);
		path(y, 0) = y;
	}
	//end
	color = CImg<int>(n, 1, 1, 1, 0);
	geometry = CImg<int>(n, 1, 1, 1, 0);
	float smallnum = 0;

	//��ʼ��չ ����һ�� �ӵڶ��е������ڶ��� ���һ�е����ó��� ���һ��һ�� ��Ϊ���Ľṹ��ֵgeometry������
	//for i=2:Height-1
	for (int j = 1;j < Height - 1;j++) {
		//�Ȱ���һ�е�ǿ��ֵȫ��������� ��ʱ����Ҫ�Ƚ����������ó�������
		//for j=rectLeftBorder:rectRightBorder
		for (int i = rectLeftBorder;i < rectRightBorder;i++) {
			int x = i - rectLeftBorder;
			int y = j;
			int rectStartXPoint = i - rectLeftBorder;
			color(y, 0, 0, 0) = leftImage(i, j, 0, 0) - rightImage(x, y, 0, 0);
			if (x == 0)
			{
				Axdao = 2 * leftImage(i + 1, j - 1, 0, 0) + leftImage(i + 1, j, 0, 0) + 2 * leftImage(i + 1, j + 1, 0, 0) - 2 * leftImage(i - 1, j - 1, 0, 0) - leftImage(i - 1, j, 0, 0) - 2 * leftImage(i - 1, j + 1, 0, 0);
				Bxdao = 2 * rightImage(x + 1, y - 1, 0, 0) + rightImage(x + 1, y, 0, 0) + 2 * rightImage(x + 1, y + 1, 0, 0);
				Aydao = -2 * leftImage(i - 1, j - 1, 0, 0) - leftImage(i, j - 1, 0, 0) - 2 * leftImage(i + 1, j - 1, 0, 0) + 2 * leftImage(i - 1, j + 1, 0, 0) + leftImage(i, j + 1, 0, 0) + 2 * leftImage(i + 1, j + 1, 0, 0);
				Bydao = -rightImage(x, y - 1, 0, 0) - 2 * rightImage(x + 1, y - 1, 0, 0) + rightImage(x, y + 1, 0, 0) + 2 * rightImage(x + 1, y + 1, 0, 0);
				geometry(rectStartXPoint, 0, 0, 0) = (Axdao - Bxdao)*(Aydao - Bydao);
				rightImageStrength(rectStartXPoint) = sqrt(color(rectStartXPoint, 0, 0, 0)) + geometry(rectStartXPoint, 0, 0, 0);
				continue;
			}//end
			if (i == rectRightBorder - 1)
			{
				Axdao = -2 * leftImage(i - 1, j - 1, 0, 0) - leftImage(i - 1, j, 0, 0) - 2 * leftImage(i - 1, j + 1, 0, 0);
				Bxdao = 2 * rightImage(x + 1, y - 1, 0, 0) + rightImage(x + 1, y, 0, 0) + 2 * rightImage(x + 1, y + 1, 0, 0) - 2 * rightImage(x - 1, y - 1, 0, 0) - rightImage(x - 1, y, 0, 0) - 2 * rightImage(x - 1, y + 1, 0, 0);
				Aydao = -2 * leftImage(i - 1, j - 1, 0, 0) - leftImage(i, j - 1, 0, 0) + 2 * leftImage(i - 1, j + 1, 0, 0) + leftImage(i, j + 1, 0, 0);
				Bydao = -2 * rightImage(x - 1, y - 1, 0, 0) - rightImage(x - 1, y, 0, 0) - 2 * rightImage(x + 1, y - 1, 0, 0) + 2 * rightImage(x - 1, y + 1, 0, 0) + rightImage(x, y + 1, 0, 0) + 2 * rightImage(x + 1, y + 1, 0, 0);
				geometry(rectStartXPoint, 0, 0, 0) = (Axdao - Bxdao)*(Aydao - Bydao);
				rightImageStrength(rectStartXPoint, 0, 0, 0) = sqrt(color(rectStartXPoint, 0, 0, 0)) + geometry(rectStartXPoint, 0, 0, 0);
				continue;
			}//end
			Axdao = 2 * leftImage(i + 1, j - 1, 0, 0) + leftImage(i + 1, j, 0, 0) + 2 * leftImage(i + 1, j + 1, 0, 0) - 2 * leftImage(i - 1, j - 1, 0, 0) - leftImage(i - 1, j, 0, 0) - 2 * leftImage(i - 1, j + 1, 0, 0);
			Bxdao = 2 * rightImage(x + 1, y - 1, 0, 0) + rightImage(x + 1, y, 0, 0) + 2 * rightImage(x + 1, y + 1, 0, 0) - 2 * rightImage(x - 1, y - 1, 0, 0) - rightImage(x - 1, y, 0, 0) - 2 * rightImage(x - 1, y + 1, 0, 0);
			Aydao = -2 * leftImage(i - 1, j - 1, 0, 0) - leftImage(i, j - 1, 0, 0) - 2 * leftImage(i + 1, j - 1, 0, 0) + 2 * leftImage(i - 1, j + 1, 0, 0) + leftImage(i, j + 1, 0, 0) + 2 * leftImage(i + 1, j + 1, 0, 0);
			Bydao = -2 * rightImage(x - 1, y - 1, 0, 0) - rightImage(x, y - 1, 0, 0) - 2 * rightImage(x + 1, y - 1, 0, 0) + 2 * rightImage(x - 1, y + 1, 0, 0) + rightImage(x, y + 1, 0, 0) + 2 * rightImage(x + 1, y + 1, 0, 0);
			geometry(rectStartXPoint, 0, 0, 0) = (Axdao - Bxdao)*(Aydao - Bydao);
			rightImageStrength(rectStartXPoint, 0, 0, 0) = sqrt(color(rectStartXPoint, 0, 0, 0)) + geometry(rectStartXPoint, 0, 0, 0);

		}//end
		//for j=1:n
		for (int i = 0;i < n;i++)
		{
			if (path(i, j - 1, 0, 0) == 0)
			{
				if (rightImageStrength(0,0,0,0) < rightImageStrength(1, 0, 0, 0))
				{
					leftImageStrength(i, 0, 0, 0) = leftImageStrength(i, 0, 0, 0) + rightImageStrength(0, 0, 0, 0);
					path(i, j, 0, 0) = 0;
				}
				else
				{
					leftImageStrength(i, 0, 0, 0) = leftImageStrength(i, 0, 0, 0) + rightImageStrength(1, 0, 0, 0);
					path(i, j, 0, 0) = 1;
				}//end
			}
			else
			{
				if (path(i, j - 1, 0, 0) == n-1)
				{
					if (rightImageStrength(n - 2, 0, 0, 0) < rightImageStrength(n-1, 0, 0, 0))
					{
						leftImageStrength(i, 0, 0, 0) = leftImageStrength(i, 0, 0, 0) + rightImageStrength(n - 2, 0, 0, 0);
						path(i, j, 0, 0) = n - 2;
					}
					else
					{
						leftImageStrength(i, 0, 0, 0) = leftImageStrength(i, 0, 0, 0) + rightImageStrength(n-1, 0, 0, 0);
						path(i, j,0,0) = n-1;
					}//end
				}
				else
				{
					if (rightImageStrength(path(i, j - 1,0,0) - 1) < rightImageStrength(path(i, j - 1,0,0)))
					{
						if (rightImageStrength(path(i, j - 1, 0, 0) - 1) < rightImageStrength(path(i, j - 1, 0, 0) + 1))
						{
							smallnum = rightImageStrength(path(i, j - 1, 0, 0) - 1);
							path(i, j, 0, 0) = path(i, j - 1, 0, 0) - 1;
						}
						else
						{
							smallnum = rightImageStrength(path(i, j - 1, 0, 0) + 1);
							path(i, j, 0, 0) = path(i, j - 1, 0, 0) + 1;
						}//end
					}
					else
					{
						if (rightImageStrength(path(i, j - 1, 0, 0)) < rightImageStrength(path(i, j - 1, 0, 0) + 1))
						{
							smallnum = rightImageStrength(path(i, j - 1, 0, 0));
							path(i, j, 0, 0) = path(i, j - 1, 0, 0);
						}
						else
						{
							smallnum = rightImageStrength(path(i, j - 1, 0, 0) + 1);
							path(i, j, 0, 0) = path(i, j - 1, 0, 0) + 1;
						}//end
					}//end
					leftImageStrength(i, 0, 0, 0) = leftImageStrength(i, 0, 0, 0) + smallnum;
				}//end
			}//end
			smallnum = 0;
		}//end
	}
	//�Ƚ�leftImageStrength��ŵ�ÿ��·����ǿ��ֵ���ܺ� ˭��С ��ѡpath�ж�Ӧ����һ�е�·��
//[minNumber,minIndex]=min(leftImageStrength)
	//minNumber ��Сֵ
	//minIndex ��Сֵ���ڵ��±����
	float minNumber = leftImageStrength(0);
	int minIndex = 0;
	for (int i = 0;i < n;i++)
	{
		if (minNumber > leftImageStrength(i, 0, 0, 0))
		{
			minNumber = leftImageStrength(i, 0, 0, 0);
			minIndex = i;
		}
	}
	CImg<int> mypath = CImg<int>(Height, 1, 1, 1, 0);
	//mypath=path(:,minIndex);
	for (int i = 0;i < Height;i++)
	{
		mypath(i, 0, 0, 0) = path(minIndex, i, 0, 0);
	}

	//����MR_Radish666 ˵�Ͼ�Ӧ���ǣ�mypath=path.colRange(minIndex-1,minIndex)

	//cout<<"һ����С·����"<<endl<<mypath<<endl;



//mypath�ŵľ�����ѷ����ѡ����·�� ����·�������ǲο�ͼA �ұ���Ŀ��ͼB
//for i=1:Height
//����Ӧ���Ǹ�ֵ��ѷ���ߵ��������˵���ֵ
	for (int j = 0;j < Height;j++)
	{
		for (int i = 0;i < mypath(j, 0, 0, 0) + rectLeftBorder;i++)
		{
			D(i, j, 0, 0) = leftImage(i, j, 0, 0);
			D(i, j, 0, 1) = leftImage(i, j, 0, 1);
			D(i, j, 0, 2) = leftImage(i, j, 0, 2);
		}//end
		for (int i = mypath(j, 0, 0, 0) + rectLeftBorder;i < Y;i++)
		{
			int x = i- rectLeftBorder;
			int y = j;
			//if( y>Weight || x>Height )continue;//end
			D(i, j, 0, 0) = rightImage(x, y, 0, 0);
			D(i, j, 0, 1) = rightImage(x, y, 0, 1);
 			D(i, j, 0, 2) = rightImage(x, y, 0, 2);
		}//end
	}//end
//D=uint8(D);

// ����ѷ����
	rectLeftBorder = Weight + 1 + rdata1;
	//figure;imshow(D)
	//hold on;
	for (int j = 0;j < Height;j++)
	{
		D(rectLeftBorder + mypath(j, 0, 0, 0), j,0, 0) = 1;
		D(rectLeftBorder + mypath(j, 0, 0, 0), j,0, 1) = 1;
		D(rectLeftBorder + mypath(j, 0, 0, 0), j,0, 2) = 1;
	}
	//end
	D.save((char*)"fenghe.bmp");

	//���غϳɵ�ͼ��
	return D;


}

//4-17�������԰����
void bestSeamLineGetLinePoint1(char* inputTestAddr1, char* inputTestAddr2, int matchVecdx, int matchVecdy)
{
	//ͨ���۲������Կ�����matchVecdx���ص������ˮƽ���ȣ���ȷ���ĳ��ȣ�
	//matchVecdyΪ��ֱ�����ϵ�ƫ����
	//�ȸ���֮ǰ�õ���H��������ص�����Rect
	CImg<int> leftImage, rightImage;
	leftImage.load(inputTestAddr1);
	rightImage.load(inputTestAddr2);
	int  Weight;
	//Weight���,Height�߶�
	Weight = leftImage._width;
	int Height = leftImage._height;

	//�����ͼ������Ͻ�Ϊ����
	//rectLeftBorder�ص��������߽��x����
	//rectRightBorder�ص��������߽��x���꼴���ͼ��ı߽�
	//rectRightBorder=Weight;
	//�ص�����ĸ߶�
	int rectLeftBorder = Weight - matchVecdx;
	int rectRightBorder = Weight;
	int rectHeight = Height - abs(matchVecdy);

	//�ж�matchVecdy>0 ����ͼ����
	//matchVecdy < 0,����ͼ����
	//int rectTopBorder;

	//����ֻ�Ǽ����ص������n���ѣ������������壬����������
	int n = rectRightBorder - rectLeftBorder;

	//�����ں�ͼ�Ĵ�С2 * Weight - matchVecdx, leftImage._height + abs(matchVecdy);����ͼ��֮ʱ
	CImg<int> D = CImg<int>(2 * leftImage._width - abs(matchVecdx), leftImage._height + abs(matchVecdy), 1, 3, 0);

	//����õ��ص�����Ĳ�ֵͼ��  ��ʵ�Ҳ��������ֵͼ�����  ֻҪ�����ص�����Ĵ�С�ͺ��� Ϊʲô��Ҫ�����ֵͼ ������û�õ�
	//Rectf Ϊ�ص�����Ĳ�ֵͼ��
	CImg<int> Rectf = CImg<int>(n, rectHeight, 1, 3, 0);

	//imageChannelNum ͼ���ͨ����
	int imageChannelNum = 3;
	//����Ӧ�����ص���������ֵ������ɣ���Ҫ�޸���

	//�������㷨��ʵֻ��Ҫ�ص�������������ͼ
	CImg<int> rectLeftImage = CImg<int>(n, rectHeight, 1, 4, 0);
	CImg<int> rectRightImage = CImg<int>(n, rectHeight, 1, 4, 0);

	for (int j = 0;j < Rectf._height;j++)
	{
		//	for j=0:Rectf._height
		//rectLeftBorder�ص��������ʼλ��
		int leftY, rightY;
		if (matchVecdy > 0)
		{
			leftY = j;
			rightY = j + matchVecdy;
		}
		else
		{
			//��Ϊ��ʱ��matchVecdyΪ��ֵ���൱��ֱ�����,��Ҫȡ����ֵ
			leftY = j + abs(matchVecdy);
			rightY = j;
		}
		for (int i = rectLeftBorder;i < rectRightBorder;i++)
		{
			for (int k = 0;k < imageChannelNum;k++)
			{

				Rectf(i - rectLeftBorder, j, 0, k) = leftImage(i, leftY, 0, k) - rightImage(i - rectLeftBorder, rightY, 0, k);

				rectLeftImage(i - rectLeftBorder, j, 0, k) = leftImage(i, leftY, 0, k);
				rectRightImage(i - rectLeftBorder, j, 0, k) = rightImage(i - rectLeftBorder, rightY, 0, k);
			}
			rectLeftImage(i - rectLeftBorder, j, 0, 3) = 0.3*leftImage(i, leftY, 0, 0) + 0.59*leftImage(i, leftY, 0, 1)+ 0.11*leftImage(i, leftY, 0, 2);
			rectRightImage(i - rectLeftBorder, j, 0, 3) = 0.3*rightImage(i - rectLeftBorder, rightY, 0, 0)+ 0.59*rightImage(i - rectLeftBorder, rightY, 0, 1)+ 0.11*rightImage(i - rectLeftBorder, rightY, 0, 2);
		}//end
	}//end

	//path��·���ľ���
	//����Ϊ�����ά����,��path����Ϊ2ά���飬��ֻ���һά��ֵ
	CImg<int> path = CImg<int>(n, rectHeight, 1, 1, 0);
	//��ǿ��ֵ ÿ��·����ǿ��ֵstrength=color^2+geometry
	CImg<int> color = CImg<int>(n, 1, 1, 1, 0);
	CImg<int> geometry = CImg<int>(n, 1, 1, 1, 0);

	//leftImageStrength���ͼ���ǿ��
	//rightImageStrength�ұ�ͼ���ǿ��
	CImg<int> leftImageStrength = CImg<int>(n, 1, 1, 3, 0);
	CImg<int> rightImageStrength = CImg<int>(n, 1, 1, 3, 0);

	float Bxdao;
	float Bydao;
	float Aydao;
	float Axdao;

	//�����һ�м���ʼ����ǿ��ֵ
	//for j=rectLeftBorder:rectRightBorder
	for (int j = rectLeftBorder;j < rectRightBorder;j++)
	{
		int leftX = j - rectLeftBorder;
		int y = j - rectLeftBorder;
		color(y, 0, 0, 0) = rectLeftImage(0, y, 0, 3) - rectRightImage(0, y, 0, 3);
		if (y == 0)
		{
			Bxdao = rectRightImage(y + 1, 0, 0, 3) + 2 * rectRightImage(y + 1, 1, 0, 3);
			Bydao = rectRightImage(y, 1, 0, 3) + 2 * rectRightImage(y + 1, 1, 0, 3);
			//������Ҫ����ԭͼ�������д���
			//���Ǹ߶���һ����Ҫ�����ж����ϻ�����ƫ���޸�
			if (matchVecdy > 0)
			{
				Aydao = 2 * leftImage(j - 1, 1, 0, 0) + leftImage(j, 1, 0, 0) + 2 * leftImage(j + 1, 1, 0, 0);
				Axdao = leftImage(j + 1, 0, 0, 0) + 2 * leftImage(j + 1, 1, 0, 0) - leftImage(j - 1, 0, 0, 0) - 2 * leftImage(j - 1, 1, 0, 0);
			}
			else
			{//matchVecdy < 0
				Aydao = 2 * leftImage(j - 1, 1 + abs(matchVecdy), 0, 0) + leftImage(j, 1 + abs(matchVecdy), 0, 0) + 2 * leftImage(j + 1, 1 + abs(matchVecdy), 0, 0);
				Axdao = leftImage(j + 1, 0 + abs(matchVecdy), 0, 0) + 2 * leftImage(j + 1, 1 + abs(matchVecdy), 0, 0) - leftImage(j - 1, 0 + abs(matchVecdy), 0, 0) - 2 * leftImage(j - 1, 1 + abs(matchVecdy), 0, 0);
			}
			geometry(y, 0, 0, 0) = (Axdao - Bxdao)*(Aydao - Bydao);
			leftImageStrength(y, 0, 0, 0) = sqrt(color(y, 0, 0, 0)) + geometry(y, 0, 0, 0);
			path(y, 0, 0, 0) = y;
			continue;
		}
		//end
		if (j == rectRightBorder - 1)
		{

			Axdao = rectLeftImage(leftX - 1, 0, 0, 3) - 2 * rectLeftImage(leftX - 1, 1, 0, 3);
			Aydao = 2 * rectLeftImage(leftX - 1, 1, 0, 3) + rectLeftImage(leftX, 1, 0, 3);

			//����Խ��
			if (matchVecdy > 0)
			{
				//Bxdao = 2*
				Bxdao = rightImage(y + 1,0 + abs(matchVecdy),0,0) + 2 * rightImage(y + 1, 1 + abs(matchVecdy), 0, 0) - rightImage(y - 1, 0 + abs(matchVecdy), 0, 0) - 2 * rightImage(y - 1, 1 + abs(matchVecdy), 0, 0);
				Bydao = 2 * rightImage(y - 1, 1 + abs(matchVecdy), 0, 0) + rightImage(y, 1 + abs(matchVecdy), 0, 0) + 2 * rightImage(y + 1, 1 + abs(matchVecdy), 0, 0);
			}
			else
			{//matchVecdy < 0
				Bxdao = rightImage(y + 1, 0 , 0, 0) + 2 * rightImage(y + 1, 1 , 0, 0) - rightImage(y - 1, 0 , 0, 0) - 2 * rightImage(y - 1, 1 , 0, 0);
				Bydao = 2 * rightImage(y - 1, 1 , 0, 0) + rightImage(y, 1 , 0, 0) + 2 * rightImage(y + 1, 1 , 0, 0);

			}
	//		Bxdao = rectRightImage(y + 1, 0, 0, 0) + 2 * rectRightImage(y + 1, 1, 0, 0) - rectRightImage(y - 1, 0, 0, 0) - 2 * rectRightImage(y - 1, 1, 0, 0);
	//		Bydao = 2 * rectRightImage(y - 1, 1, 0, 0) + rectRightImage(y, 1, 0, 0) + 2 * rectRightImage(y + 1, 1, 0, 0);
			geometry(y, 0, 0, 0) = (Axdao - Bxdao)*(Aydao - Bydao);
			leftImageStrength(y, 0, 0, 0) = sqrt(color(y, 0, 0, 0)) + geometry(y, 0, 0, 0);
			path(y, 0, 0, 0) = y;
			continue;
		}
		//end
		Axdao = rectLeftImage(leftX + 1, 0, 0, 3) + 2 * rectLeftImage(leftX + 1, 1, 0, 3) - rectLeftImage(leftX - 1, 0, 0, 3) - 2 * rectLeftImage(leftX - 1, 1, 0, 3);
		Bxdao = rectRightImage(y + 1, 0, 0, 0) + 2 * rectRightImage(y + 1, 1, 0, 3) - rectRightImage(y - 1, 0, 0, 0) - 2 * rectRightImage(y - 1, 1, 0, 3);
		Aydao = 2 * rectLeftImage(leftX - 1, 1, 0, 3) + rectLeftImage(leftX, 1, 0, 3) + 2 * rectLeftImage(leftX + 1, 1, 0, 3);
		Bydao = 2 * rectRightImage(y - 1, 1, 0, 3) + rectRightImage(y, 1, 0, 3) + 2 * rectRightImage(y + 1, 1, 0, 3);
		geometry(y, 0, 0, 0) = (Axdao - Bxdao)*(Aydao - Bydao);
		leftImageStrength(y, 0, 0, 0) = sqrt(color(y, 0, 0, 0)) - geometry(y, 0, 0, 0);
		path(y, 0,0, 0) = y;
	}
	//end
	color = CImg<int>(n, 1, 1, 1, 0);
	geometry = CImg<int>(n, 1, 1, 1, 0);
	float smallnum = 0;

	//��ʼ��չ ����һ�� �ӵڶ��е������ڶ��� ���һ�е����ó��� ���һ��һ�� ��Ϊ���Ľṹ��ֵgeometry������




	for (int j = 1;j < rectHeight - 1;j++) {
		//�Ȱ���һ�е�ǿ��ֵȫ��������� ��ʱ����Ҫ�Ƚ����������ó�������
		//for j=rectLeftBorder:rectRightBorder

		for (int i = 0;i < rectRightBorder - rectLeftBorder;i++) {
			int x = i;
			int y = j;
			int rectStartXPoint = i;
#ifndef DEBUG	
			color(i, 0, 0, 0) = rectLeftImage(i, j, 0, 3) - rectRightImage(x, y, 0, 3);
		
			if (x == 0)
			{
				//��ʱ��Ҫ����ԭͼ����д������������Ŀ�ȵ���Ϣ
				if (matchVecdy > 0)
				{
					Axdao = 2 * (0.3 * leftImage(i + rectLeftBorder + 1, j - 1, 0, 0) + 0.59 * leftImage(i + rectLeftBorder + 1, j - 1, 0, 1) + 0.11  * leftImage(i + rectLeftBorder + 1, j - 1, 0, 2))
						+ (0.3*leftImage(i + rectLeftBorder + 1, j, 0, 0) + 0.59*leftImage(i + rectLeftBorder + 1, j, 0, 1) + 0.11*leftImage(i + rectLeftBorder + 1, j, 0, 2))
						+ 2 * (0.3*leftImage(i + rectLeftBorder + 1, j + 1, 0, 0) +0.59*leftImage(i + rectLeftBorder + 1, j + 1, 0, 1) +0.11* leftImage(i + rectLeftBorder + 1, j + 1, 0, 2))
						- 2 * (0.3*leftImage(i + rectLeftBorder - 1, j - 1, 0, 0) + 0.59* leftImage(i + rectLeftBorder - 1, j - 1, 0, 1) + 0.11*leftImage(i + rectLeftBorder - 1, j - 1, 0, 2))
						- (0.3*leftImage(i + rectLeftBorder - 1, j, 0, 0) + 0.59* leftImage(i + rectLeftBorder - 1, j, 0, 1) + 0.11 * leftImage(i + rectLeftBorder - 1, j, 0, 2))
						- 2 * (0.3*leftImage(i + rectLeftBorder - 1, j + 1, 0, 0) + 0.59 * leftImage(i + rectLeftBorder - 1, j + 1, 0, 0) + 0.11 * leftImage(i + rectLeftBorder - 1, j + 1, 0, 0));
					
					Aydao = -2 * (0.3*leftImage(i + rectLeftBorder - 1, j - 1, 0, 0) + 0.59* leftImage(i + rectLeftBorder - 1, j - 1, 0, 1) + 0.11*leftImage(i + rectLeftBorder - 1, j - 1, 0, 2))
						- (0.3 *leftImage(i + rectLeftBorder, j - 1, 0, 0) + 0.59*leftImage(i + rectLeftBorder, j - 1, 0, 1) + 0.11 *leftImage(i + rectLeftBorder, j - 1, 0, 2))
						- 2 * (0.3 * leftImage(i + rectLeftBorder + 1, j - 1, 0, 0) + 0.59 * leftImage(i + rectLeftBorder + 1, j - 1, 0, 1) + 0.11  * leftImage(i + rectLeftBorder + 1, j - 1, 0, 2))
						+ 2 * (0.3*leftImage(i + rectLeftBorder - 1, j + 1, 0, 0) + 0.59 * leftImage(i + rectLeftBorder - 1, j + 1, 0, 0) + 0.11 * leftImage(i + rectLeftBorder - 1, j + 1, 0, 0))
						+ (0.3*leftImage(i + rectLeftBorder, j + 1, 0, 0) + 0.59*leftImage(i + rectLeftBorder, j + 1, 0, 1) + 0.11*leftImage(i + rectLeftBorder, j + 1, 0, 2))
						+ 2 * (0.3*leftImage(i + rectLeftBorder + 1, j + 1, 0, 0) + 0.59*leftImage(i + rectLeftBorder + 1, j + 1, 0, 1) + 0.11* leftImage(i + rectLeftBorder + 1, j + 1, 0, 2));
				}
				else
				{//matchVecdy < 0
					//��Ҫ���ϸ߶ȵĲ����ܵ���ȷ����λ��
					Axdao = 2 * (0.3 * leftImage(i + rectLeftBorder + 1, j - 1 + abs(matchVecdy), 0, 0) + 0.59 * leftImage(i + rectLeftBorder + 1, j - 1 + abs(matchVecdy), 0, 1) + 0.11  * leftImage(i + rectLeftBorder + 1, j - 1 + abs(matchVecdy), 0, 2))
						+ (0.3*leftImage(i + rectLeftBorder + 1, j + abs(matchVecdy), 0, 0) + 0.59*leftImage(i + rectLeftBorder + 1, j + abs(matchVecdy), 0, 1) + 0.11*leftImage(i + rectLeftBorder + 1, j + abs(matchVecdy), 0, 2))
						+ 2 * (0.3*leftImage(i + rectLeftBorder + 1, j + 1 + abs(matchVecdy), 0, 0) + 0.59*leftImage(i + rectLeftBorder + 1, j + 1 + abs(matchVecdy), 0, 1) + 0.11* leftImage(i + rectLeftBorder + 1, j + 1 + abs(matchVecdy), 0, 2))
						- 2 * (0.3*leftImage(i + rectLeftBorder - 1, j - 1 + abs(matchVecdy), 0, 0) + 0.59* leftImage(i + rectLeftBorder - 1, j - 1 + abs(matchVecdy), 0, 1) + 0.11*leftImage(i + rectLeftBorder - 1, j - 1 + abs(matchVecdy), 0, 2))
						- (0.3*leftImage(i + rectLeftBorder - 1, j + abs(matchVecdy), 0, 0) + 0.59* leftImage(i + rectLeftBorder - 1, j + abs(matchVecdy), 0, 1) + 0.11 * leftImage(i + rectLeftBorder - 1, j + abs(matchVecdy), 0, 2))
						- 2 * (0.3*leftImage(i + rectLeftBorder - 1, j + 1 + abs(matchVecdy), 0, 0) + 0.59 * leftImage(i + rectLeftBorder - 1 , j + 1+ abs(matchVecdy), 0, 0) + 0.11 * leftImage(i + rectLeftBorder - 1, j + 1 + abs(matchVecdy), 0, 0));

					Aydao = -2 * (0.3*leftImage(i + rectLeftBorder - 1, j - 1 + abs(matchVecdy), 0, 0) + 0.59* leftImage(i + rectLeftBorder - 1, j - 1 + abs(matchVecdy), 0, 1) + 0.11*leftImage(i + rectLeftBorder - 1, j - 1 + abs(matchVecdy), 0, 2))
						- (0.3 *leftImage(i + rectLeftBorder, j - 1 + abs(matchVecdy), 0, 0) + 0.59*leftImage(i + rectLeftBorder, j - 1 + abs(matchVecdy), 0, 1) + 0.11 *leftImage(i + rectLeftBorder, j - 1 + abs(matchVecdy), 0, 2))
						- 2 * (0.3 * leftImage(i + rectLeftBorder + 1, j - 1 + abs(matchVecdy), 0, 0) + 0.59 * leftImage(i + rectLeftBorder + 1, j - 1 + abs(matchVecdy), 0, 1) + 0.11  * leftImage(i + rectLeftBorder + 1, j - 1 + abs(matchVecdy), 0, 2))
						+ 2 * (0.3*leftImage(i + rectLeftBorder - 1, j + 1 + abs(matchVecdy), 0, 0) + 0.59 * leftImage(i + rectLeftBorder - 1, j + 1 + abs(matchVecdy), 0, 0) + 0.11 * leftImage(i + rectLeftBorder - 1, j + 1 + abs(matchVecdy), 0, 0))
						+ (0.3*leftImage(i + rectLeftBorder, j + 1 + abs(matchVecdy), 0, 0) + 0.59*leftImage(i + rectLeftBorder, j + 1 + abs(matchVecdy), 0, 1) + 0.11*leftImage(i + rectLeftBorder, j + 1 + abs(matchVecdy), 0, 2))
						+ 2 * (0.3*leftImage(i + rectLeftBorder + 1, j + 1 + abs(matchVecdy), 0, 0) + 0.59*leftImage(i + rectLeftBorder + 1, j + 1 + abs(matchVecdy), 0, 1) + 0.11* leftImage(i + rectLeftBorder + 1, j + 1 + abs(matchVecdy), 0, 2));

				}

				Bxdao = 2 * rectRightImage(x + 1, y - 1, 0, 3) + rectRightImage(x + 1, y, 0, 3) + 2 * rectRightImage(x + 1, y + 1, 0, 3);
				Bydao = -rectRightImage(x, y - 1, 0, 3) - 2 * rectRightImage(x + 1, y - 1, 0, 3) + rectRightImage(x, y + 1, 0, 3) + 2 * rectRightImage(x + 1, y + 1, 0, 3);
				geometry(rectStartXPoint, 0, 0, 0) = (Axdao - Bxdao)*(Aydao - Bydao);
				rightImageStrength(rectStartXPoint) = sqrt(color(rectStartXPoint, 0, 0, 0)) + geometry(rectStartXPoint, 0, 0, 0);
				continue;
			}//end

			if (i == rectRightBorder - rectLeftBorder - 1)
			{
				Axdao = -2 * rectLeftImage(i - 1, j - 1, 0, 3) - rectLeftImage(i - 1, j, 0, 3) - 2 * rectLeftImage(i - 1, j + 1, 0, 3);
				Aydao = -2 * rectLeftImage(i - 1, j - 1, 0, 3) - rectLeftImage(i, j - 1, 0, 3) + 2 * rectLeftImage(i - 1, j + 1, 0, 3) + rectLeftImage(i, j + 1, 0, 3);
				
				if (matchVecdy > 0)
				{
					
					Bxdao = 2 * (0.3*rightImage(x + 1, y - 1 + abs(matchVecdy), 0, 0) + 0.59 *rightImage(x + 1, y - 1 + abs(matchVecdy), 0, 1) + 0.11 *rightImage(x + 1, y - 1 + abs(matchVecdy), 0, 2))
						+ (0.3*rightImage(x + 1, y + abs(matchVecdy), 0, 0) + 0.59*rightImage(x + 1, y + abs(matchVecdy), 0, 1) + 0.11*rightImage(x + 1, y + abs(matchVecdy), 0, 2))
						+ 2 * (0.3*rightImage(x + 1, y + 1 + abs(matchVecdy), 0, 0) + 0.59 *rightImage(x + 1, y + 1 + abs(matchVecdy), 0, 1) + 0.11*rightImage(x + 1, y + 1 + abs(matchVecdy), 0, 2))
						- 2 * (0.3*rightImage(x - 1, y - 1 + abs(matchVecdy), 0, 0) + 0.59*rightImage(x - 1, y - 1 + abs(matchVecdy), 0, 1) + 0.11*rightImage(x - 1, y - 1 + abs(matchVecdy), 0, 2))
						- (0.3 *rightImage(x - 1, y + abs(matchVecdy), 0, 0) + 0.59 *rightImage(x - 1, y + abs(matchVecdy), 0, 1) + 0.11 *rightImage(x - 1, y + abs(matchVecdy), 0, 2))
						- 2 * (0.3 *rightImage(x - 1, y + 1 + abs(matchVecdy), 0, 0) + 0.59*rightImage(x - 1, y + 1 + abs(matchVecdy), 0, 1) + 0.11 *rightImage(x - 1, y + 1 + abs(matchVecdy), 0, 2));
					Bydao = -2 * (0.3*rightImage(x - 1, y - 1 + abs(matchVecdy), 0, 0) + 0.59*rightImage(x - 1, y - 1 + abs(matchVecdy), 0, 1) + 0.11*rightImage(x - 1, y - 1 + abs(matchVecdy), 0, 2))
						- (0.3 *rightImage(x - 1, y + abs(matchVecdy), 0, 0) + 0.59 *rightImage(x - 1, y + abs(matchVecdy), 0, 1) + 0.11 *rightImage(x - 1, y + abs(matchVecdy), 0, 2))
						- 2 * (0.3*rightImage(x + 1, y - 1 + abs(matchVecdy), 0, 0) + 0.59 *rightImage(x + 1, y - 1 + abs(matchVecdy), 0, 1) + 0.11 *rightImage(x + 1, y - 1 + abs(matchVecdy), 0, 2))
						+ 2 * (0.3 *rightImage(x - 1, y + 1 + abs(matchVecdy), 0, 0) + 0.59*rightImage(x - 1, y + 1 + abs(matchVecdy), 0, 1) + 0.11 *rightImage(x - 1, y + 1 + abs(matchVecdy), 0, 2))
						+ (0.3 * rightImage(x, y + 1 + abs(matchVecdy), 0, 0) + 0.59 * rightImage(x, y + 1 + abs(matchVecdy), 0, 1) + 0.11 * rightImage(x, y + 1 + abs(matchVecdy), 0, 2))
						+ 2 * (0.3*rightImage(x + 1, y + 1 + abs(matchVecdy), 0, 0) + 0.59 *rightImage(x + 1, y + 1 + abs(matchVecdy), 0, 1) + 0.11*rightImage(x + 1, y + 1 + abs(matchVecdy), 0, 2));
					
				}
				else
				{//matchVecdy < 0
					Bxdao = 2 * (0.3*rightImage(x + 1, y - 1, 0, 0) + 0.59 *rightImage(x + 1, y - 1, 0, 1) +  0.11 *rightImage(x + 1, y - 1, 0, 2))
						+ (0.3*rightImage(x + 1, y, 0, 0) + 0.59*rightImage(x + 1, y, 0, 1) + 0.11*rightImage(x + 1, y, 0, 2))
						+ 2 * (0.3*rightImage(x + 1, y + 1, 0, 0) + 0.59 *rightImage(x + 1, y + 1, 0, 1)  +  0.11*rightImage(x + 1, y + 1, 0, 2))
						- 2 * (0.3*rightImage(x - 1, y - 1, 0, 0) + 0.59*rightImage(x - 1, y - 1, 0, 1) + 0.11*rightImage(x - 1, y - 1, 0, 2))
						- (0.3 *rightImage(x - 1, y, 0, 0) + 0.59 *rightImage(x - 1, y, 0, 1) + 0.11 *rightImage(x - 1, y, 0, 2))
						- 2 * (0.3 *rightImage(x - 1, y + 1, 0, 0) + 0.59*rightImage(x - 1, y + 1, 0, 1) + 0.11 *rightImage(x - 1, y + 1, 0, 2));
					Bydao = -2 * (0.3*rightImage(x - 1, y - 1, 0, 0) + 0.59*rightImage(x - 1, y - 1, 0, 1) + 0.11*rightImage(x - 1, y - 1, 0, 2))
						- (0.3 *rightImage(x - 1, y, 0, 0) + 0.59 *rightImage(x - 1, y, 0, 1) + 0.11 *rightImage(x - 1, y, 0, 2))
						- 2 * (0.3*rightImage(x + 1, y - 1, 0, 0) + 0.59 *rightImage(x + 1, y - 1, 0, 1) + 0.11 *rightImage(x + 1, y - 1, 0, 2))
						+ 2 * (0.3 *rightImage(x - 1, y + 1, 0, 0) + 0.59*rightImage(x - 1, y + 1, 0, 1) + 0.11 *rightImage(x - 1, y + 1, 0, 2))
						+ (0.3 * rightImage(x, y + 1, 0, 0) + 0.59 * rightImage(x, y + 1, 0, 1) + 0.11 * rightImage(x, y + 1, 0, 2))
						+ 2 * (0.3*rightImage(x + 1, y + 1, 0, 0) + 0.59 *rightImage(x + 1, y + 1, 0, 1) + 0.11*rightImage(x + 1, y + 1, 0, 2));

				}
				//Bxdao = 2 * rectRightImage(x + 1, y - 1, 0, 0) + rectRightImage(x + 1, y, 0, 0) + 2 * rectRightImage(x + 1, y + 1, 0, 0) - 2 * rectRightImage(x - 1, y - 1, 0, 0) - rectRightImage(x - 1, y, 0, 0) - 2 * rectRightImage(x - 1, y + 1, 0, 0);
				//Bydao = -2 * rectRightImage(x - 1, y - 1, 0, 0) - rectRightImage(x - 1, y, 0, 0) - 2 * rectRightImage(x + 1, y - 1, 0, 0) + 2 * rectRightImage(x - 1, y + 1, 0, 0) + rectRightImage(x, y + 1, 0, 0) + 2 * rectRightImage(x + 1, y + 1, 0, 0);
				geometry(rectStartXPoint, 0, 0, 0) = (Axdao - Bxdao)*(Aydao - Bydao);
				rightImageStrength(rectStartXPoint, 0, 0, 0) = sqrt(color(rectStartXPoint, 0, 0, 0)) + geometry(rectStartXPoint, 0, 0, 0);
				continue;
			}//end

			Axdao = 2 * rectLeftImage(i + 1, j - 1, 0, 3) + rectLeftImage(i + 1, j, 0, 3) + 2 * rectLeftImage(i + 1, j + 1, 0, 3) - 2 * rectLeftImage(i - 1, j - 1, 0, 3) - rectLeftImage(i - 1, j, 0, 3) - 2 * rectLeftImage(i - 1, j + 1, 0, 3);
			Bxdao = 2 * rectRightImage(x + 1, y - 1, 0, 3) + rectRightImage(x + 1, y, 0, 3) + 2 * rectRightImage(x + 1, y + 1, 0, 3) - 2 * rectRightImage(x - 1, y - 1, 0, 3) - rectRightImage(x - 1, y, 0, 3) - 2 * rectRightImage(x - 1, y + 1, 0, 3);
			Aydao = -2 * rectLeftImage(i - 1, j - 1, 0, 3) - rectLeftImage(i, j - 1, 0, 3) - 2 * rectLeftImage(i + 1, j - 1, 0, 3) + 2 * rectLeftImage(i - 1, j + 1, 0, 3) + rectLeftImage(i, j + 1, 0, 3) + 2 * rectLeftImage(i + 1, j + 1, 0, 3);
			Bydao = -2 * rectRightImage(x - 1, y - 1, 0, 3) - rectRightImage(x, y - 1, 0, 3) - 2 * rectRightImage(x + 1, y - 1, 0, 3) + 2 * rectRightImage(x - 1, y + 1, 0, 3) + rectRightImage(x, y + 1, 0, 3) + 2 * rectRightImage(x + 1, y + 1, 0, 3);
			geometry(rectStartXPoint, 0, 0, 0) = (Axdao - Bxdao)*(Aydao - Bydao);
			rightImageStrength(rectStartXPoint, 0, 0, 0) = sqrt(color(rectStartXPoint, 0, 0, 0)) + geometry(rectStartXPoint, 0, 0, 0);
#endif // DEBUG
		}//end

		for (int i = 0;i < n;i++)
		{
			if (path(i, j - 1, 0, 0) == 0)
			{
				if (rightImageStrength(0, 0, 0, 0) < rightImageStrength(1, 0, 0, 0))
				{
					leftImageStrength(i, 0, 0, 0) = leftImageStrength(i, 0, 0, 0) + rightImageStrength(0, 0, 0, 0);
					path(i, j, 0, 0) = 0;
				}
				else
				{
					leftImageStrength(i, 0, 0, 0) = leftImageStrength(i, 0, 0, 0) + rightImageStrength(1, 0, 0, 0);
					path(i, j, 0, 0) = 1;
				}//end
			}
			else
			{
				if (path(i, j - 1, 0, 0) == n - 1)
				{
					if (rightImageStrength(n - 2, 0, 0, 0) < rightImageStrength(n - 1, 0, 0, 0))
					{
						leftImageStrength(i, 0, 0, 0) = leftImageStrength(i, 0, 0, 0) + rightImageStrength(n - 2, 0, 0, 0);
						path(i, j, 0, 0) = n - 2;
					}
					else
					{
						leftImageStrength(i, 0, 0, 0) = leftImageStrength(i, 0, 0, 0) + rightImageStrength(n - 1, 0, 0, 0);
						path(i, j, 0, 0) = n - 1;
					}//end
				}
				else
				{
					if (rightImageStrength(path(i, j - 1, 0, 0) - 1) < rightImageStrength(path(i, j - 1, 0, 0)))
					{
						if (rightImageStrength(path(i, j - 1, 0, 0) - 1) < rightImageStrength(path(i, j - 1, 0, 0) + 1))
						{
							smallnum = rightImageStrength(path(i, j - 1, 0, 0) - 1);
							path(i, j, 0, 0) = path(i, j - 1, 0, 0) - 1;
						}
						else
						{
							smallnum = rightImageStrength(path(i, j - 1, 0, 0) + 1);
							path(i, j, 0, 0) = path(i, j - 1, 0, 0) + 1;
						}//end
					}
					else
					{
						if (rightImageStrength(path(i, j - 1, 0, 0)) < rightImageStrength(path(i, j - 1, 0, 0) + 1))
						{
							smallnum = rightImageStrength(path(i, j - 1, 0, 0));
							path(i, j, 0, 0) = path(i, j - 1, 0, 0);
						}
						else
						{
							smallnum = rightImageStrength(path(i, j - 1, 0, 0) + 1);
							path(i, j, 0, 0) = path(i, j - 1, 0, 0) + 1;
						}//end
					}//end
					leftImageStrength(i, 0, 0, 0) = leftImageStrength(i, 0, 0, 0) + smallnum;
				}//end
			}//end
			smallnum = 0;
		}//end

	}

	//�Ƚ�leftImageStrength��ŵ�ÿ��·����ǿ��ֵ���ܺ� ˭��С ��ѡpath�ж�Ӧ����һ�е�·��
//[minNumber,minIndex]=min(leftImageStrength)
	//minNumber ��Сֵ
	//minIndex ��Сֵ���ڵ��±����
	float minNumber = leftImageStrength(0);
	int minIndex = 0;
	for (int i = 0;i < n;i++)
	{
		if (minNumber > leftImageStrength(i, 0, 0, 0))
		{
			minNumber = leftImageStrength(i, 0, 0, 0);
			minIndex = i;
		}
	}
	CImg<int> mypath = CImg<int>(rectHeight, 1, 1, 1, 0);
	//mypath=path(:,minIndex);
	for (int i = 0;i < rectHeight;i++)
	{
		mypath(i, 0, 0, 0) = path(minIndex, i, 0, 0);
	}

	//����MR_Radish666 ˵�Ͼ�Ӧ���ǣ�mypath=path.colRange(minIndex-1,minIndex)

	//cout<<"һ����С·����"<<endl<<mypath<<endl;

//mypath�ŵľ�����ѷ����ѡ����·�� ����·�������ǲο�ͼA �ұ���Ŀ��ͼB
//����Ӧ���Ǹ�ֵ��ѷ���ߵ��������˵���ֵ
	//����ߵĸ�ֵ�ͽ�����֮ǰ�ĸ�ֵ����
	if (matchVecdy > 0) 
	{
		for (int y = 0;y < D._height;y++)
		{
			for (int x = 0;x < D._width;x++)
			{
				if (x < leftImage._width&&y>abs(matchVecdy))
				{
					if (x > rectLeftBorder
						&& y > abs(matchVecdy)
						&& y < rightImage._height)
						continue;
					D(x, y, 0, 0) = leftImage(x, y - abs(matchVecdy), 0, 0);
					D(x, y, 0, 1) = leftImage(x, y - abs(matchVecdy), 0, 1);
					D(x, y, 0, 2) = leftImage(x, y - abs(matchVecdy), 0, 2);
				}
				if(x > rectLeftBorder && y<rightImage._height)
				{
					if (x < rectRightBorder
						&& y > abs(matchVecdy)
						&& y < rightImage._height)
						continue;
					D(x, y, 0, 0) = rightImage(x - rectLeftBorder, y, 0, 0);
					D(x, y, 0, 1) = rightImage(x - rectLeftBorder, y, 0, 1);
					D(x, y, 0, 2) = rightImage(x - rectLeftBorder, y, 0, 2);
				}
			}
		}
	}
	else 
	{
		for (int y = 0;y < D._height;y++)
		{
			for (int x = 0;x < D._width;x++)
			{
				if (x < leftImage._width&&y< leftImage._height)
				{
					if (x > rectLeftBorder
						&&y > abs(matchVecdy)
						&& y < leftImage._height)
						continue;
					D(x, y, 0, 0) = leftImage(x, y, 0, 0);
					D(x, y, 0, 1) = leftImage(x, y, 0, 1);
					D(x, y, 0, 2) = leftImage(x, y, 0, 2);
				}
				if (x >= rectLeftBorder 
					&&x<D._width
					&& y > abs(matchVecdy))
				{
					if (y > abs(matchVecdy)
						&& y < leftImage._height
						&& x < rectRightBorder)
						continue;
					D(x, y, 0, 0) = rightImage(x - rectLeftBorder, y - abs(matchVecdy), 0, 0);
					D(x, y, 0, 1) = rightImage(x - rectLeftBorder, y - abs(matchVecdy), 0, 1);
					D(x, y, 0, 2) = rightImage(x - rectLeftBorder, y - abs(matchVecdy), 0, 2);
				}

			}
		}
	}
	
	//���ص�������1|0��������ͼ��
	CImg<int> blendWhiteBlackImage = CImg<int>(n, rectHeight, 1, 3, 0);
	for (int y = 0;y < Rectf._height;y++)
	{
		for (int x = 0;x < mypath(y, 0, 0, 0);x++)
		{
			blendWhiteBlackImage(x, y, 0, 0) = 255;
			blendWhiteBlackImage(x, y, 0, 1) = 255;
			blendWhiteBlackImage(x, y, 0, 2) = 255;
		}
		for (int x = mypath(y, 0, 0, 0);x < Rectf._width;x++)
		{
			blendWhiteBlackImage(x, y, 0, 0) = 0;
			blendWhiteBlackImage(x, y, 0, 1) = 0;
			blendWhiteBlackImage(x, y, 0, 2) = 0;
		}
	}
	//blendWhiteBlackImage.display((char*)"moban.bmp");
	//blendWhiteBlackImage.save((char*)"Output/moban.bmp");
#ifndef __DEBUG_Laplacian

	char* rectLeftImageName = (char*)"Output/temp/rectLeftImage.bmp";
	char* rectRightImageName = (char*)"Output/temp/rectRightImage.bmp";
	char* blendWhiteBlackImageName = (char*)"Output/temp/blendWhiteBlackImage.bmp";

	rectLeftImage.save((char*)"Output/temp/rectLeftImage.bmp");
	rectRightImage.save((char*)"Output/temp/rectRightImage.bmp");
	blendWhiteBlackImage.save((char*)"Output/temp/blendWhiteBlackImage.bmp");
	

	//����
	

	MyLaplacianBlending myreplacianoperatorObj = MyLaplacianBlending();
	//char* myreplacianoperatorImage = myreplacianoperatorObj.getLaplacianBlendRectImage(rectLeftImageName, rectRightImageName, blendWhiteBlackImageName);
	CImg<int> LaplacianBlendingImage;
	LaplacianBlendingImage = myreplacianoperatorObj.MyLaplacianBlend(rectLeftImageName, rectRightImageName, blendWhiteBlackImageName);
	for (int y = 0;y < Rectf._height;y++)
	{
		for (int x = 0;x < Rectf._width;x++)
		{
			D(x + rectLeftBorder, y + abs(matchVecdy), 0, 0) = LaplacianBlendingImage(x, y, 0, 0);
			D(x + rectLeftBorder, y + abs(matchVecdy), 0, 1) = LaplacianBlendingImage(x, y, 0, 1);
			D(x + rectLeftBorder, y + abs(matchVecdy), 0, 2) = LaplacianBlendingImage(x, y, 0, 2);
		}
		
	}
#endif // __DEBUG_Laplacian

#ifdef DEBUG�ر�ֱ�Ӹ�ֵ����

	for (int y = 0;y < Rectf._height;y++)
	{
		for (int x = 0;x < mypath(y, 0, 0, 0);x++)
		{
			D(x + rectLeftBorder, y + abs(matchVecdy), 0, 0) = rectLeftImage(x, y, 0, 0);
			D(x + rectLeftBorder, y + abs(matchVecdy), 0, 1) = rectLeftImage(x, y, 0, 1);
			D(x + rectLeftBorder, y + abs(matchVecdy), 0, 2) = rectLeftImage(x, y, 0, 2);
		}
		for (int x =mypath(y, 0, 0, 0) ;x < Rectf._width;x++)
		{
			D(x + rectLeftBorder, y + abs(matchVecdy), 0, 0) = rectRightImage(x, y, 0, 0);
			D(x + rectLeftBorder, y + abs(matchVecdy), 0, 1) = rectRightImage(x, y, 0, 1);
			D(x + rectLeftBorder, y + abs(matchVecdy), 0, 2) = rectRightImage(x, y, 0, 2);
		}
	}
#endif // DEBUG�ر�ֱ�Ӹ�ֵ����


#ifdef __DEBUG�رջ���ѷ����



// ����ѷ����
	//rectLeftBorder = Weight + matchVecdx;
	//figure;imshow(D)
	//hold on;
	for (int j = 0;j < rectHeight;j++)
	{
		D(rectLeftBorder + mypath(j, 0, 0, 0), j + abs(matchVecdy), 0, 0) ;
		D(rectLeftBorder + mypath(j, 0, 0, 0), j + abs(matchVecdy), 0, 1) ;
		D(rectLeftBorder + mypath(j, 0, 0, 0), j + abs(matchVecdy), 0, 2) ;
	}
	//end
	D.display((char*)"fenghe.bmp");

#endif // __DEBUG�رջ���ѷ����
	D.display("��Ϻ�");
}

int main() {

	//���Է���ߴ���
	//30m (22).bmp  30m (20).bmp
	char* inputTestAddr1 = (char*)"Input/3.bmp";
	char* inputTestAddr2 = (char*)"Input/4.bmp";

	CImg<int> leftImage;
	leftImage.load(inputTestAddr1);

	CImg<int> rightImage;
	rightImage.load(inputTestAddr2);

	bestSeamLineGetLinePoint1(inputTestAddr1, inputTestAddr2,179,-4);
/**/
	//30m (1)
	//����������
	char* inputAddr[] = { (char*)"Input/30m (14).bmp",(char*)"Input/30m (16).bmp", (char*)"Input/30m (18).bmp", (char*)"Input/30m (20).bmp"
	,(char*)"Input/30m (22).bmp",(char*)"Input/30m (24).bmp", (char*)"Input/30m (26).bmp", (char*)"Input/30m (28).bmp"
	,(char*)"Input/30m (30).bmp",(char*)"Input/30m (32).bmp", (char*)"Input/30m (34).bmp", (char*)"Input/30m (36).bmp"
	,(char*)"Input/30m (38).bmp",(char*)"Input/30m (40).bmp", (char*)"Input/30m (42).bmp", (char*)"Input/30m (44).bmp"
	,(char*)"Input/30m (46).bmp",(char*)"Input/30m (48).bmp", (char*)"Input/30m (50).bmp", (char*)"Input/30m (52).bmp"
	,(char*)"Input/30m (54).bmp",(char*)"Input/30m (56).bmp", (char*)"Input/30m (58).bmp", (char*)"Input/30m (60).bmp"
	,(char*)"Input/30m (62).bmp",(char*)"Input/30m (64).bmp", (char*)"Input/30m (66).bmp", (char*)"Input/30m (68).bmp"
	,(char*)"Input/30m (70).bmp",(char*)"Input/30m (72).bmp", (char*)"Input/30m (74).bmp", (char*)"Input/30m (76).bmp"
	,(char*)"Input/30m (78).bmp",(char*)"Input/30m (80).bmp", (char*)"Input/30m (82).bmp", (char*)"Input/30m (84).bmp"
	};
	//��ȡ����������
	MySift mySift[N];
	const int STARTIMAGE = 3;
	for (int i = 0;i < N;i++)
	{
		mySift[i] = MySift(inputAddr[i+ STARTIMAGE], 1);
		mySift[i].SiftMainProcess();
		
		char* SaveUrl = (char*)"Output/1-2/";
		char* SaveName = (char*)"_kp";
		char* SaveType = (char*)".bmp";
		char *buf = new char[strlen(SaveUrl)  + strlen(SaveName) + strlen(SaveType) + sizeof(i) + 1];
		//ƴ�������ַ���

		sprintf(buf, "%s%d%s%s", SaveUrl, i, SaveName,SaveType);
		
		mySift[i].saveImgWithKeypoint(buf);
	}

	//������ƥ������
	MyMatching myMatching[N - 1];

	for (int i = 0;i < N - 1;i++)
	{
		myMatching[i] = MyMatching(mySift[i].getKeyPointsCount(), mySift[i].getFirstKeyDescriptors(),
			mySift[i+1].getKeyPointsCount(), mySift[i+1].getFirstKeyDescriptors());;
		
		//������ƥ��
		myMatching[i].featureMatchMainProcess();

		char* SaveUrl = (char*)"Output/1-2/";
		char* SaveName1 = (char*)"_kp_real";
		char* SaveName2 = (char*)"mixImg";
		char* SaveName3 = (char*)"mixImgWithLine";
		char* SaveName4 = (char*)"mixImgWithLine_fixed";
		
		char* SaveType = (char*)".bmp";
		char* buf = new char[strlen(SaveUrl) + strlen(SaveName1) + strlen(SaveType) + sizeof(i) + 1];
		char* buf1 = new char[strlen(SaveUrl) + strlen(SaveName1) + strlen(SaveType) + sizeof(i) + 1];
		//ƴ�������ַ���
		sprintf(buf, "%s%d%s%s", SaveUrl, i*2, SaveName1, SaveType);
		sprintf(buf1, "%s%d%s%s", SaveUrl, i*2+1, SaveName1, SaveType);
		//ƴ�������ַ���
		//������ɫ��ƥ��㣨���ڿ�ʡ��
		myMatching[i].drawOriKeypointOnImg(inputAddr[i + STARTIMAGE], inputAddr[i+1 + STARTIMAGE], buf, buf1);
		//��������ͼ���ƴ��ͼƬ������ͼƬ�����ڿ���ʡ��

		buf = new char[strlen(SaveUrl) + strlen(SaveName2) + strlen(SaveType) + sizeof(i) + 1];
		buf1 = new char[strlen(SaveUrl) + strlen(SaveName3) + strlen(SaveType) + sizeof(i) + 1];
		//ƴ�������ַ���
		sprintf(buf, "%s%d%s%s", SaveUrl, i, SaveName2, SaveType);
		sprintf(buf1, "%s%d%s%s", SaveUrl, i, SaveName3, SaveType);

		myMatching[i].mixImageAndDrawPairLine(buf, buf1);

		buf = new char[strlen(SaveUrl) + strlen(SaveName4) + strlen(SaveType) + sizeof(i) + 1];
		//ƴ�������ַ���
		sprintf(buf, "%s%d%s%s", SaveUrl, i, SaveName4, SaveType);

		//ͨ��RANSACƥ�������㲢�滭��������
		myMatching[i].myRANSACtoFindKpTransAndDrawOut(buf);

	}

	//ͼ��ƴ������
	MyMorePictureStitch myMorePictureStitch1 = MyMorePictureStitch();

	CImg<int> Imgfirst, lastImage;
	Imgfirst.load(inputAddr[0 + STARTIMAGE]);
	lastImage = Imgfirst;
	vector<char*> Other;
	Other.push_back((char*)"no use LaplacianBlending");
	Other.push_back((char*)"DrawbestSeamLine");
	for (int i = 0;i < N - 1;i++)
	{
		
		lastImage = myMorePictureStitch1.MyStitchImage4(inputAddr[i + STARTIMAGE], inputAddr[i + 1 + STARTIMAGE], lastImage, myMatching[i].getMatchVec().col, myMatching[i].getMatchVec().row,(char*)"OptimalSeamBlend", Other);
	}
	
	


	/*30m (22)*/char* inputAddr1 = (char*)"Input/30m (1).bmp";
	char* inputAddr2 = (char*)"Input/30m (3).bmp";
	char* inputAddr3 = (char*)"Input/30m (22).bmp";
	char* inputAddr4 = (char*)"Input/30m (24).bmp";

	//�˴���ȡͼƬ��������һ��������ϵ��ȡ���������
	//�˴��õ��������㽨�齫����Ϣ���б���
	//����������
	MySift mySift1(inputAddr1, 1);
	mySift1.SiftMainProcess();
	mySift1.saveImgWithKeypoint((char*)"Output/1-2/1_kp.bmp");

	MySift mySift2(inputAddr2, 1);
	mySift2.SiftMainProcess();
	mySift2.saveImgWithKeypoint((char*)"Output/1-2/2_kp.bmp");



	//-------��3��ͼƬ��ƴ��
	MySift mySift3(inputAddr3, 1);
	mySift3.SiftMainProcess();
	mySift3.saveImgWithKeypoint((char*)"Output/1-3/3_kp.bmp");

	//-------��4��ͼƬ��ƴ��
	MySift mySift4(inputAddr3, 1);
	mySift4.SiftMainProcess();
	mySift4.saveImgWithKeypoint((char*)"Output/1-3/4_kp.bmp");

	//�˴�ƥ�������һ������
	MyMatching myMatching1 =  MyMatching(mySift1.getKeyPointsCount(), mySift1.getFirstKeyDescriptors(),
		mySift2.getKeyPointsCount(), mySift2.getFirstKeyDescriptors());
	//������ƥ��
	myMatching1.featureMatchMainProcess();
	//������ɫ��ƥ��㣨���ڿ�ʡ��
	myMatching1.drawOriKeypointOnImg(inputAddr1, inputAddr2, (char*)"Output/1-2/1_kp_real.bmp", (char*)"Output/1-2/2_kp_real.bmp");
	//��������ͼ���ƴ��ͼƬ������ͼƬ�����ڿ���ʡ��
	myMatching1.mixImageAndDrawPairLine((char*)"Output/1-2/mixImg1.bmp", (char*)"Output/1-2/mixImgWithLine1.bmp");
	//ͨ��RANSACƥ�������㲢�滭��������
	myMatching1.myRANSACtoFindKpTransAndDrawOut((char*)"Output/1-2/mixImgWithLine_fixed1.bmp");

	//--------�Ե�����ͼ����д���---------
	//�˴�ƥ�������һ������
	MyMatching myMatching2 = MyMatching(mySift2.getKeyPointsCount(), mySift2.getFirstKeyDescriptors(),
		mySift3.getKeyPointsCount(), mySift3.getFirstKeyDescriptors());
	//������ƥ��
	myMatching2.featureMatchMainProcess();
	//������ɫ��ƥ��㣨���ڿ�ʡ��
	myMatching2.drawOriKeypointOnImg(inputAddr2, inputAddr3, (char*)"Output/1-2/2_kp_real.bmp", (char*)"Output/1-2/3_kp_real.bmp");
	//��������ͼ���ƴ��ͼƬ������ͼƬ�����ڿ���ʡ��
	myMatching2.mixImageAndDrawPairLine((char*)"Output/1-2/mixImg2.bmp", (char*)"Output/1-2/mixImgWithLine2.bmp");
	//ͨ��RANSACƥ�������㲢�滭��������
	myMatching2.myRANSACtoFindKpTransAndDrawOut((char*)"Output/1-2/mixImgWithLine_fixed2.bmp");

	//--------�Ե��ķ�ͼ����д���---------
	//�˴�ƥ�������һ������
	MyMatching myMatching3 = MyMatching(mySift3.getKeyPointsCount(), mySift3.getFirstKeyDescriptors(),
		mySift4.getKeyPointsCount(), mySift4.getFirstKeyDescriptors());
	//������ƥ��
	myMatching3.featureMatchMainProcess();
	//������ɫ��ƥ��㣨���ڿ�ʡ��
	myMatching3.drawOriKeypointOnImg(inputAddr3, inputAddr4, (char*)"Output/1-2/3_kp_real.bmp", (char*)"Output/1-2/4_kp_real.bmp");
	//��������ͼ���ƴ��ͼƬ������ͼƬ�����ڿ���ʡ��
	myMatching3.mixImageAndDrawPairLine((char*)"Output/1-2/mixImg3.bmp", (char*)"Output/1-2/mixImgWithLine3.bmp");
	//ͨ��RANSACƥ�������㲢�滭��������
	myMatching3.myRANSACtoFindKpTransAndDrawOut((char*)"Output/1-2/mixImgWithLine_fixed3.bmp");


	MyMorePictureStitch myMorePictureStitch = MyMorePictureStitch();
	
	CImg<int> Img1;
	Img1.load(inputAddr1);
	CImg<int> lastpicture = myMorePictureStitch.MyStitchImageByGradualFusionBlend(inputAddr1, inputAddr2,Img1, myMatching1.getMatchVec().col, myMatching1.getMatchVec().row);
	lastpicture = myMorePictureStitch.MyStitchImageByGradualFusionBlend(inputAddr2, inputAddr3, lastpicture, myMatching2.getMatchVec().col, myMatching2.getMatchVec().row);

	myMorePictureStitch.MyStitchImageByGradualFusionBlend(inputAddr3, inputAddr4, lastpicture, myMatching3.getMatchVec().col, myMatching3.getMatchVec().row);
	/*
	//��������ͼ��ƴ�ӵĳ�ʼ��ֵ��ͬʱ��������ƴ�ӣ��洢ͼ�񣬽����½������������޸�Դ����
	MyBlending myBlending1(myMatching1.getMatchVec().col, myMatching1.getMatchVec().row);
	myBlending1.blendingMainProcess(inputAddr1, inputAddr2);
	myBlending1.saveBlendedImg((char*)"Output/1-2/blendedImg1.bmp");

	//--------�Ե�����ͼ����д���
	MyBlending myBlending2(myMatching2.getMatchVec().col, myMatching2.getMatchVec().row);
	myBlending2.blendingMainProcess(inputAddr2, inputAddr3);
	myBlending2.saveBlendedImg((char*)"Output/1-2/blendedImg2.bmp");
	*/
	
	/*char* inputAddr1 = (char*)"Input/3.jpg";
	char* inputAddr2 = (char*)"Input/4.jpg";

	MySift mySift1(inputAddr1, 1);
	mySift1.SiftMainProcess();
	mySift1.saveImgWithKeypoint((char*)"Output/1-2/1_kp.jpg");

	MySift mySift2(inputAddr2, 1);
	mySift2.SiftMainProcess();
	mySift2.saveImgWithKeypoint((char*)"Output/1-2/2_kp.jpg");

	MyMatching myMatching1(mySift1.getKeyPointsCount(), mySift1.getFirstKeyDescriptors(),
		mySift2.getKeyPointsCount(), mySift2.getFirstKeyDescriptors());
	myMatching1.featureMatchMainProcess();
	myMatching1.drawOriKeypointOnImg(inputAddr1, inputAddr2, (char*)"Output/1-2/1_kp_real.jpg", (char*)"Output/1-2/2_kp_real.jpg");
	myMatching1.mixImageAndDrawPairLine((char*)"Output/1-2/mixImg.jpg", (char*)"Output/1-2/mixImgWithLine.jpg");
	myMatching1.myRANSACtoFindKpTransAndDrawOut((char*)"Output/1-2/mixImgWithLine_fixed.jpg");

	MyBlending myBlending(myMatching1.getMatchVec().col, myMatching1.getMatchVec().row);
	myBlending.blendingMainProcess(inputAddr1, inputAddr2);
	myBlending.saveBlendedImg((char*)"Output/1-2/blendedImg.jpg");*/


	return 0;
}

