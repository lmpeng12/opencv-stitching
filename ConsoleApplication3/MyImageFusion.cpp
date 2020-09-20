#include "MyImageFusion.h"
#define FILE_BUFFER_LENGTH 30000


MyImageFusion::MyImageFusion()
{
}


MyImageFusion::~MyImageFusion()
{
}

//2019-03-26�汾����
//��ͼ��ƴ�ӵĸĽ��㷨����ѷ�����㷨 ͼ���ں�
//��ѷ���ߴ���
//leftImage �ص�������������ͼ��
//rightImage �ص������ұ������ͼ��
//����汾��Ե������κ���ת�ĺͷ���仯��ͼ��
//3-27ʱ��û����ƫ�ƣ������Ե��Ƿ������źϳ�ͼ�����
//3-28��ɼ��������ң�û�з���仯
CImg<int> MyImageFusion::bestSeamLineGetImage(CImg<int> leftImage, CImg<int> rightImage, int matchVecdx, int matchVecdy)
{
	//ͨ���۲������Կ�����matchVecdx���ص������ˮƽ����ȷ���ĳ��ȣ�
	//�ȸ���֮ǰ�õ���H��������ص�����Rect
	int  Weight;
	//Weight���,Height�߶�
	Weight = leftImage._width;
	int Height = leftImage._height;
	//matchVecdx=-118; ���Ӧ�����ұ���չ�Ŀ��
	//matchVecdx=fix(-matchVecdx);
	matchVecdx = -matchVecdx;

	//�����ͼ������Ͻ�Ϊ����
	//rectLeftBorder�ص��������߽��x����
	//rectRightBorder�ص��������߽��x���꼴���ͼ��ı߽�
	int rectLeftBorder = Weight + matchVecdx;
	//rectRightBorder=Weight;
	int rectRightBorder = Weight;

	int rectHeight = Height - abs(matchVecdy);
	//n=rectRightBorder-rectLeftBorder+1;
	//����ֻ�Ǽ����ص������n���ѣ������������壬����������
	int n = rectRightBorder - rectLeftBorder;


	//�����ں�ͼ�Ĵ�С
	//matchVecdx=-118;
	//rdata2=3;
	//�˴���YӦ�������ºϳ�ͼ��Ŀ��
	int Y = 2 * Weight + matchVecdx;

	CImg<int> D = CImg<int>(Y, Height, 1, 3, 0);

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
		if (j == rectRightBorder - 1)
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
			int x = i - rectLeftBorder;
			int y = j;
			//if( y>Weight || x>Height )continue;//end
			D(i, j, 0, 0) = rightImage(x, y, 0, 0);
			D(i, j, 0, 1) = rightImage(x, y, 0, 1);
			D(i, j, 0, 2) = rightImage(x, y, 0, 2);
		}//end
	}//end
//D=uint8(D);

// ����ѷ����
	rectLeftBorder = Weight + 1 + matchVecdx;
	//figure;imshow(D)
	//hold on;
	for (int j = 0;j < Height;j++)
	{
		D(rectLeftBorder + mypath(j, 0, 0, 0), j, 0, 0) = 1;
		D(rectLeftBorder + mypath(j, 0, 0, 0), j, 0, 1) = 1;
		D(rectLeftBorder + mypath(j, 0, 0, 0), j, 0, 2) = 1;
	}
	//end
	D.display((char*)"fenghe.bmp");

	//���غϳɵ�ͼ��
	return D;


}


//3-29�޶��棬������ƫ��������ƫ�ƣ�ͬʱֻ���ص㼯����ʡ�ռ�
//ͬʱ�Ż�����ṹ
//�����ٴν��в����
CImg<int> MyImageFusion::bestSeamLineGetLinePoint(CImg<int> leftImage1, CImg<int> rightImage2, int matchVecdx, int matchVecdy)
{
	//ͨ���۲������Կ�����matchVecdx���ص������ˮƽ���ȣ���ȷ���ĳ��ȣ�
	//matchVecdyΪ��ֱ�����ϵ�ƫ����
	//�ȸ���֮ǰ�õ���H��������ص�����Rect
	CImg<int> leftImage = CImg<int>(leftImage1._width , leftImage1._height, 1, 3, 0);
	CImg<int> rightImage = CImg<int>(rightImage2._width, rightImage2._height, 1, 3, 0);
	for(int x = 0;x< leftImage._width;x++)
		for (int y = 0;y < leftImage._height;y++) 
			for (int k = 0;k < 3;k++) 
			{
				leftImage(x, y, 0, k) = leftImage1(x, y, 0, k);
				rightImage(x, y, 0, k) = leftImage1(x, y, 0, k);
			}
		

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
	CImg<int> rectLeftImage = CImg<int>(n, rectHeight, 1, 3, 0);
	CImg<int> rectRightImage = CImg<int>(n, rectHeight, 1, 3, 0);

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
		color(y, 0, 0, 0) = leftImage(1, j, 0, 0) - rightImage(1, y, 0, 0);
		if (y == 0)
		{
			Bxdao = rectRightImage(y + 1, 0, 0, 0) + 2 * rectRightImage(y + 1, 1, 0, 0);
			Bydao = rectRightImage(y, 1, 0, 0) + 2 * rectRightImage(y + 1, 1, 0, 0);
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

			Axdao = rectLeftImage(leftX - 1, 0, 0, 0) - 2 * rectLeftImage(leftX - 1, 1, 0, 0);
			Aydao = 2 * rectLeftImage(leftX - 1, 1, 0, 0) + rectLeftImage(leftX, 1, 0, 0);
			Bxdao = rectRightImage(y + 1, 0, 0, 0) + 2 * rectRightImage(y + 1, 1, 0, 0) - rectRightImage(y - 1, 0, 0, 0) - 2 * rectRightImage(y - 1, 1, 0, 0);
			Bydao = 2 * rectRightImage(y - 1, 1, 0, 0) + rectRightImage(y, 1, 0, 0) + 2 * rectRightImage(y + 1, 1, 0, 0);
			geometry(y, 0, 0, 0) = (Axdao - Bxdao)*(Aydao - Bydao);
			leftImageStrength(y, 0, 0, 0) = sqrt(color(y, 0, 0, 0)) + geometry(y, 0, 0, 0);
			path(y, 0, 0, 0) = y;
			continue;
		}
		//end
		Axdao = rectLeftImage(leftX + 1, 0, 0, 0) + 2 * rectLeftImage(leftX + 1, 1, 0, 0) - rectLeftImage(leftX - 1, 0, 0, 0) - 2 * rectLeftImage(leftX - 1, 1, 0, 0);
		Bxdao = rectRightImage(y + 1, 0, 0, 0) + 2 * rectRightImage(y + 1, 1, 0, 0) - rectRightImage(y - 1, 0, 0, 0) - 2 * rectRightImage(y - 1, 1, 0, 0);
		Aydao = 2 * rectLeftImage(leftX - 1, 1, 0, 0) + rectLeftImage(leftX, 1, 0, 0) + 2 * rectLeftImage(leftX + 1, 1, 0, 0);
		Bydao = 2 * rectRightImage(y - 1, 1, 0, 0) + rectRightImage(y, 1, 0, 0) + 2 * rectRightImage(y + 1, 1, 0, 0);
		geometry(y, 0, 0, 0) = (Axdao - Bxdao)*(Aydao - Bydao);
		leftImageStrength(y, 0, 0, 0) = sqrt(color(y, 0, 0, 0)) - geometry(y, 0, 0, 0);
		path(y, 0) = y;
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
			color(y, 0, 0, 0) = rectLeftImage(i, j, 0, 0) - rectRightImage(x, y, 0, 0);
			if (x == 0)
			{
				//��ʱ��Ҫ����ԭͼ����д������������Ŀ�ȵ���Ϣ
				if (matchVecdy > 0)
				{
					Axdao = 2 * leftImage(i + rectLeftBorder + 1, j - 1, 0, 0) + leftImage(i + rectLeftBorder + 1, j, 0, 0) + 2 * leftImage(i + rectLeftBorder + 1, j + 1, 0, 0) - 2 * leftImage(i + rectLeftBorder - 1, j - 1, 0, 0) - leftImage(i + rectLeftBorder - 1, j, 0, 0) - 2 * leftImage(i + rectLeftBorder - 1, j + 1, 0, 0);
					Aydao = -2 * leftImage(i + rectLeftBorder - 1, j - 1, 0, 0) - leftImage(i + rectLeftBorder, j - 1, 0, 0) - 2 * leftImage(i + rectLeftBorder + 1, j - 1, 0, 0) + 2 * leftImage(i + rectLeftBorder - 1, j + 1, 0, 0) + leftImage(i + rectLeftBorder, j + 1, 0, 0) + 2 * leftImage(i + rectLeftBorder + 1, j + 1, 0, 0);
				}
				else
				{//matchVecdy < 0
					//��Ҫ���ϸ߶ȵĲ����ܵ���ȷ����λ��
					Axdao = 2 * leftImage(i + rectLeftBorder + 1, j - 1 + abs(matchVecdy), 0, 0) + leftImage(i + rectLeftBorder + 1, j + abs(matchVecdy), 0, 0) + 2 * leftImage(i + rectLeftBorder + 1, j + 1 + abs(matchVecdy), 0, 0) - 2 * leftImage(i + rectLeftBorder - 1, j - 1 + abs(matchVecdy), 0, 0) - leftImage(i + rectLeftBorder - 1, j + abs(matchVecdy), 0, 0) - 2 * leftImage(i + rectLeftBorder - 1, j + 1 + abs(matchVecdy), 0, 0);
					Aydao = -2 * leftImage(i + rectLeftBorder - 1, j - 1 + abs(matchVecdy), 0, 0) - leftImage(i + rectLeftBorder, j - 1 + abs(matchVecdy), 0, 0) - 2 * leftImage(i + rectLeftBorder + 1, j - 1 + abs(matchVecdy), 0, 0) + 2 * leftImage(i + rectLeftBorder - 1, j + 1 + abs(matchVecdy), 0, 0) + leftImage(i + rectLeftBorder, j + 1 + abs(matchVecdy), 0, 0) + 2 * leftImage(i + rectLeftBorder + 1, j + 1 + abs(matchVecdy), 0, 0);
				}

				Bxdao = 2 * rectRightImage(x + 1, y - 1, 0, 0) + rectRightImage(x + 1, y, 0, 0) + 2 * rectRightImage(x + 1, y + 1, 0, 0);
				Bydao = -rectRightImage(x, y - 1, 0, 0) - 2 * rectRightImage(x + 1, y - 1, 0, 0) + rectRightImage(x, y + 1, 0, 0) + 2 * rectRightImage(x + 1, y + 1, 0, 0);
				geometry(rectStartXPoint, 0, 0, 0) = (Axdao - Bxdao)*(Aydao - Bydao);
				rightImageStrength(rectStartXPoint) = sqrt(color(rectStartXPoint, 0, 0, 0)) + geometry(rectStartXPoint, 0, 0, 0);
				continue;
			}//end
			if (i == rectLeftBorder - 1)
			{
				Axdao = -2 * rectLeftImage(i - 1, j - 1, 0, 0) - rectLeftImage(i - 1, j, 0, 0) - 2 * rectLeftImage(i - 1, j + 1, 0, 0);
				Bxdao = 2 * rectRightImage(x + 1, y - 1, 0, 0) + rectRightImage(x + 1, y, 0, 0) + 2 * rectRightImage(x + 1, y + 1, 0, 0) - 2 * rectRightImage(x - 1, y - 1, 0, 0) - rectRightImage(x - 1, y, 0, 0) - 2 * rectRightImage(x - 1, y + 1, 0, 0);
				Aydao = -2 * rectLeftImage(i - 1, j - 1, 0, 0) - rectLeftImage(i, j - 1, 0, 0) + 2 * rectLeftImage(i - 1, j + 1, 0, 0) + rectLeftImage(i, j + 1, 0, 0);
				Bydao = -2 * rectRightImage(x - 1, y - 1, 0, 0) - rectRightImage(x - 1, y, 0, 0) - 2 * rectRightImage(x + 1, y - 1, 0, 0) + 2 * rectRightImage(x - 1, y + 1, 0, 0) + rectRightImage(x, y + 1, 0, 0) + 2 * rectRightImage(x + 1, y + 1, 0, 0);
				geometry(rectStartXPoint, 0, 0, 0) = (Axdao - Bxdao)*(Aydao - Bydao);
				rightImageStrength(rectStartXPoint, 0, 0, 0) = sqrt(color(rectStartXPoint, 0, 0, 0)) + geometry(rectStartXPoint, 0, 0, 0);
				continue;
			}//end
			Axdao = 2 * rectLeftImage(i + 1, j - 1, 0, 0) + rectLeftImage(i + 1, j, 0, 0) + 2 * rectLeftImage(i + 1, j + 1, 0, 0) - 2 * rectLeftImage(i - 1, j - 1, 0, 0) - rectLeftImage(i - 1, j, 0, 0) - 2 * rectLeftImage(i - 1, j + 1, 0, 0);
			Bxdao = 2 * rectRightImage(x + 1, y - 1, 0, 0) + rectRightImage(x + 1, y, 0, 0) + 2 * rectRightImage(x + 1, y + 1, 0, 0) - 2 * rectRightImage(x - 1, y - 1, 0, 0) - rectRightImage(x - 1, y, 0, 0) - 2 * rectRightImage(x - 1, y + 1, 0, 0);
			Aydao = -2 * rectLeftImage(i - 1, j - 1, 0, 0) - rectLeftImage(i, j - 1, 0, 0) - 2 * rectLeftImage(i + 1, j - 1, 0, 0) + 2 * rectLeftImage(i - 1, j + 1, 0, 0) + rectLeftImage(i, j + 1, 0, 0) + 2 * rectLeftImage(i + 1, j + 1, 0, 0);
			Bydao = -2 * rectRightImage(x - 1, y - 1, 0, 0) - rectRightImage(x, y - 1, 0, 0) - 2 * rectRightImage(x + 1, y - 1, 0, 0) + 2 * rectRightImage(x - 1, y + 1, 0, 0) + rectRightImage(x, y + 1, 0, 0) + 2 * rectRightImage(x + 1, y + 1, 0, 0);
			geometry(rectStartXPoint, 0, 0, 0) = (Axdao - Bxdao)*(Aydao - Bydao);
			rightImageStrength(rectStartXPoint, 0, 0, 0) = sqrt(color(rectStartXPoint, 0, 0, 0)) + geometry(rectStartXPoint, 0, 0, 0);
		}//end
		//for j=1:n
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
						break;
					D(x, y, 0, 0) = leftImage(x, y - abs(matchVecdy), 0, 0);
					D(x, y, 0, 1) = leftImage(x, y - abs(matchVecdy), 0, 1);
					D(x, y, 0, 2) = leftImage(x, y - abs(matchVecdy), 0, 2);
				}
				if (x > rectLeftBorder && y < rightImage._height)
				{
					if (x < rectRightBorder
						&& y > abs(matchVecdy)
						&& y < rightImage._height)
						break;
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
				if (x < leftImage._width&&y < leftImage._height)
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
					&& x<D._width
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

	CImg<int> rectImage = CImg<int>(n, rectHeight, 1, 3, 0);
	for (int y = 0;y < Rectf._height;y++)
	{
		for (int x = 0;x < mypath(y, 0, 0, 0);x++)
		{
			rectImage(x, y, 0, 0) = rectLeftImage(x, y, 0, 0);
			rectImage(x, y, 0, 1) = rectLeftImage(x, y, 0, 1);
			rectImage(x, y, 0, 2) = rectLeftImage(x, y, 0, 2);
		}
		for (int x = mypath(y, 0, 0, 0);x < Rectf._width;x++)
		{
			rectImage(x, y, 0, 0) = rectRightImage(x, y, 0, 0);
			rectImage(x, y, 0, 1) = rectRightImage(x, y, 0, 1);
			rectImage(x, y, 0, 2) = rectRightImage(x, y, 0, 2);
		}
	}
	rectImage = drawbestSeamLine(rectImage, mypath);
	for (int y = 0;y < Rectf._height;y++)
	{
		for (int x = 0;x < Rectf._width;x++)
		{
			D(x + rectLeftBorder, y + abs(matchVecdy), 0, 0) = rectImage(x, y, 0, 0);
			D(x + rectLeftBorder, y + abs(matchVecdy), 0, 1) = rectImage(x, y, 0, 1);
			D(x + rectLeftBorder, y + abs(matchVecdy), 0, 2) = rectImage(x, y, 0, 2);
		}

	}
	//Ч�����±߸ߣ��������Ҫѡ����Ƴ���ѷ���߻�����Ҫ�ϱߵ��ó���
//	for (int y = 0;y < Rectf._height;y++)
//	{
//		for (int x = 0;x < mypath(y, 0, 0, 0);x++)
//		{
//			D(x + rectLeftBorder, y + abs(matchVecdy), 0, 0) = rectLeftImage(x, y, 0, 0);
//			D(x + rectLeftBorder, y + abs(matchVecdy), 0, 1) = rectLeftImage(x, y, 0, 1);
//			D(x + rectLeftBorder, y + abs(matchVecdy), 0, 2) = rectLeftImage(x, y, 0, 2);
//		}
//		for (int x = mypath(y, 0, 0, 0);x < Rectf._width;x++)
//		{
//			D(x + rectLeftBorder, y + abs(matchVecdy), 0, 0) = rectRightImage(x, y, 0, 0);
//			D(x + rectLeftBorder, y + abs(matchVecdy), 0, 1) = rectRightImage(x, y, 0, 1);
//			D(x + rectLeftBorder, y + abs(matchVecdy), 0, 2) = rectRightImage(x, y, 0, 2);
//		}
//	}

	//end
	//D.display((char*)"fenghe.bmp");
	//rectImage.display((char*)"rectImage.bmp");
	//���غϳɺ��ص������ͼ��
	return  rectImage;

}



//----2019-4-17----�޶�����
//�ع�
CImg<int> MyImageFusion::bestSeamLineGetLinePoint1(char* inputTestAddr1, char* inputTestAddr2, int matchVecdx, int matchVecdy)
{
	return bestSeamLineGetLinePoint1(inputTestAddr1, inputTestAddr2, matchVecdx, matchVecdy, (char*)"use LaplacianBlending", (char*)"not DrawbestSeamLine");
}


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
//2.��ʱ���޸Ķ�ȡ������ͼ�������������Ҫ��ʱ����Զ�ͼ�������չ����Ҫ���󵼵�ʱ���е�����

CImg<int> MyImageFusion::bestSeamLineGetLinePoint1(char* inputTestAddr1, char* inputTestAddr2, int matchVecdx, int matchVecdy, char* isLaplacianBlending,char* isDrawbestSeamLine)
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
			rectLeftImage(i - rectLeftBorder, j, 0, 3) = 0.3*leftImage(i, leftY, 0, 0) + 0.59*leftImage(i, leftY, 0, 1) + 0.11*leftImage(i, leftY, 0, 2);
			rectRightImage(i - rectLeftBorder, j, 0, 3) = 0.3*rightImage(i - rectLeftBorder, rightY, 0, 0) + 0.59*rightImage(i - rectLeftBorder, rightY, 0, 1) + 0.11*rightImage(i - rectLeftBorder, rightY, 0, 2);
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
				Bxdao = rightImage(y + 1, 0 + abs(matchVecdy), 0, 0) + 2 * rightImage(y + 1, 1 + abs(matchVecdy), 0, 0) - rightImage(y - 1, 0 + abs(matchVecdy), 0, 0) - 2 * rightImage(y - 1, 1 + abs(matchVecdy), 0, 0);
				Bydao = 2 * rightImage(y - 1, 1 + abs(matchVecdy), 0, 0) + rightImage(y, 1 + abs(matchVecdy), 0, 0) + 2 * rightImage(y + 1, 1 + abs(matchVecdy), 0, 0);
			}
			else
			{//matchVecdy < 0
				Bxdao = rightImage(y + 1, 0, 0, 0) + 2 * rightImage(y + 1, 1, 0, 0) - rightImage(y - 1, 0, 0, 0) - 2 * rightImage(y - 1, 1, 0, 0);
				Bydao = 2 * rightImage(y - 1, 1, 0, 0) + rightImage(y, 1, 0, 0) + 2 * rightImage(y + 1, 1, 0, 0);

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
		path(y, 0, 0, 0) = y;
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
						+ 2 * (0.3*leftImage(i + rectLeftBorder + 1, j + 1, 0, 0) + 0.59*leftImage(i + rectLeftBorder + 1, j + 1, 0, 1) + 0.11* leftImage(i + rectLeftBorder + 1, j + 1, 0, 2))
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
						- 2 * (0.3*leftImage(i + rectLeftBorder - 1, j + 1 + abs(matchVecdy), 0, 0) + 0.59 * leftImage(i + rectLeftBorder - 1, j + 1 + abs(matchVecdy), 0, 0) + 0.11 * leftImage(i + rectLeftBorder - 1, j + 1 + abs(matchVecdy), 0, 0));

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
					Bxdao = 2 * (0.3*rightImage(x + 1, y - 1, 0, 0) + 0.59 *rightImage(x + 1, y - 1, 0, 1) + 0.11 *rightImage(x + 1, y - 1, 0, 2))
						+ (0.3*rightImage(x + 1, y, 0, 0) + 0.59*rightImage(x + 1, y, 0, 1) + 0.11*rightImage(x + 1, y, 0, 2))
						+ 2 * (0.3*rightImage(x + 1, y + 1, 0, 0) + 0.59 *rightImage(x + 1, y + 1, 0, 1) + 0.11*rightImage(x + 1, y + 1, 0, 2))
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


	//����������Ĳ��������жϣ�
	//isLaplacianBlending == "use LaplacianBlending"  ������˹�ں�
	//isLaplacianBlending == "no use LaplacianBlending"  ������������˹�ں�
	//isDrawbestSeamLine == "DrawbestSeamLine" ������ѷ����
	//isDrawbestSeamLine == "not DrawbestSeamLine" ��������ѷ����

	CImg<int> BesbestSeamLineBlendingImage;
	if (strcmp(isLaplacianBlending, (char*)"no use LaplacianBlending") == 0) 
	{
		BesbestSeamLineBlendingImage = CImg<int>(leftImage._width, leftImage._height, 1, leftImage._spectrum, 0);
		//�����ж�ֱ����ں�
		
	for (int y = 0;y < Rectf._height;y++)
	{
		for (int x = 0;x < mypath(y, 0, 0, 0);x++)
		{
			//ͨ����
			for (int k = 0;k < leftImage._spectrum;k++)
				BesbestSeamLineBlendingImage(x, y, 0, k) = rectLeftImage(x, y, 0, k);
			
		}
		for (int x = mypath(y, 0, 0, 0);x < Rectf._width;x++)
		{
			//ͨ����
			for (int k = 0;k < leftImage._spectrum;k++)
				BesbestSeamLineBlendingImage(x, y, 0, k) = rectRightImage(x, y, 0, k);
		}
	}
	}
	else //if (strcmp(isLaplacianBlending, (char*)"use LaplacianBlending")) ��ʡ�ԣ����Բ�ֺ������룬Ϊ�Ժ��ֱ����ںϿ���
	{
		//���ж�ֱ����ں�
		//��������������Ĳ�����Ĭ���ȷŽ�ȥ
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
	char* rectLeftImageName = (char*)"Output/temp/SaverectLeftImage.bmp";
	char* rectRightImageName = (char*)"Output/temp/SaverectRightImage.bmp";
	char* blendWhiteBlackImageName = (char*)"Output/temp/blendWhiteBlackImage.bmp";

	//��Ҫ��������Ϊ��ͨ��
	CImg<int> SaverectLeftImage = CImg<int>(rectLeftImage._width, rectLeftImage._height, 1, 3, 0);
	CImg<int> SaverectRightImage = CImg<int>(rectRightImage._width, rectRightImage._height, 1, 3, 0);

	cimg_forXY(SaverectLeftImage, x, y) {
	for (int k = 0;k < SaverectLeftImage._spectrum;k++) 
	{
		SaverectLeftImage(x, y, 0, k) = rectLeftImage(x, y, 0, k);
		SaverectRightImage(x, y, 0, k) = rectRightImage(x, y, 0, k);
	}}
	SaverectLeftImage.save((char*)"Output/temp/SaverectLeftImage.bmp");
	SaverectRightImage.save((char*)"Output/temp/SaverectRightImage.bmp");
	blendWhiteBlackImage.save((char*)"Output/temp/blendWhiteBlackImage.bmp");

	cout << "�����ںϲ���"<<endl;
	MyLaplacianBlending myreplacianoperatorObj = MyLaplacianBlending();
	BesbestSeamLineBlendingImage = myreplacianoperatorObj.MyLaplacianBlend(rectLeftImageName, rectRightImageName, blendWhiteBlackImageName);
	
	}
	
	if (strcmp(isDrawbestSeamLine, (char*)"DrawbestSeamLine")==0) 
	{
		//����ѷ����
		BesbestSeamLineBlendingImage = drawbestSeamLine(BesbestSeamLineBlendingImage, mypath);
	}
	else 
	{
		//����
		//������
	}
	
	return BesbestSeamLineBlendingImage;
}

//�滭��ѷ���ߣ���Ҫ�����ص������ͼ���Լ���ѷ���ߵĵ㼯��
//rectImage�ص������ͼ��bestSeamLinePathһά����ѷ���ߵ㼯
CImg<int> MyImageFusion::drawbestSeamLine(CImg<int> rectImage, CImg<int> bestSeamLinePath)
{
	for (int j = 0;j < bestSeamLinePath._width;j++)
	{
		rectImage(bestSeamLinePath(j, 0, 0, 0), j, 0, 0) = 1;
		rectImage(bestSeamLinePath(j, 0, 0, 0), j, 0, 1) = 1;
		rectImage(bestSeamLinePath(j, 0, 0, 0), j, 0, 2) = 1;
	}
	return rectImage;
}

void bestSeamLineGetLinePoint2(char* inputTestAddr1, char* inputTestAddr2, int matchVecdx, int matchVecdy) 
{
	//��ȡ�������ߵ�ͼ��
	CImg<int> leftImage, rightImage;
	leftImage.load(inputTestAddr1);
	rightImage.load(inputTestAddr2);

	//rectLeftBorder�ص��������߽��x����
	//rectRightBorder�ص��������߽��x���꼴���ͼ��ı߽�
	//rectRightBorder=Weight;
	//�ص�����ĸ߶�
	int rectLeftBorder = leftImage._width - abs(matchVecdx);
	int rectRightBorder = leftImage._width;
	int rectHeight = leftImage._height - abs(matchVecdy);


}