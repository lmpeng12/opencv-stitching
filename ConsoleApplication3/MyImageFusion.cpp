#include "MyImageFusion.h"
#define FILE_BUFFER_LENGTH 30000


MyImageFusion::MyImageFusion()
{
}


MyImageFusion::~MyImageFusion()
{
}

//2019-03-26版本更新
//《图像拼接的改进算法》最佳缝合线算法 图像融合
//最佳缝合线代码
//leftImage 重叠区域左边区域的图像
//rightImage 重叠区域右边区域的图像
//这个版本针对的是无任何旋转的和仿射变化的图形
//3-27时候没有下偏移，这个针对的是返回两张合成图的情况
//3-28完成简单上下左右，没有仿射变化
CImg<int> MyImageFusion::bestSeamLineGetImage(CImg<int> leftImage, CImg<int> rightImage, int matchVecdx, int matchVecdy)
{
	//通过观察代码可以看到，matchVecdx是重叠区域的水平（宽度方向的长度）
	//先根据之前得到的H矩阵计算重叠区域Rect
	int  Weight;
	//Weight宽度,Height高度
	Weight = leftImage._width;
	int Height = leftImage._height;
	//matchVecdx=-118; 这个应该是右边扩展的宽度
	//matchVecdx=fix(-matchVecdx);
	matchVecdx = -matchVecdx;

	//以左边图像的左上角为顶点
	//rectLeftBorder重叠区域的左边界的x坐标
	//rectRightBorder重叠区域的左边界的x坐标即左边图像的边界
	int rectLeftBorder = Weight + matchVecdx;
	//rectRightBorder=Weight;
	int rectRightBorder = Weight;

	int rectHeight = Height - abs(matchVecdy);
	//n=rectRightBorder-rectLeftBorder+1;
	//这里只是计算重叠区域的n而已，并无其他意义，看不懂意义
	int n = rectRightBorder - rectLeftBorder;


	//最终融合图的大小
	//matchVecdx=-118;
	//rdata2=3;
	//此处的Y应该是重新合成图像的宽度
	int Y = 2 * Weight + matchVecdx;

	CImg<int> D = CImg<int>(Y, Height, 1, 3, 0);

	//计算得到重叠区域的差值图像  其实我不懂计算差值图像干嘛  只要计算重叠区域的大小就好了 为什么还要计算差值图 后面又没用到
	//Rect=zeros(Height,n);
	//Rectf 为重叠区域的差值图像
	CImg<int> Rectf = CImg<int>(leftImage._width, leftImage._height, 1, 3, 0);
	//#define leftImage(x,y) leftImage(x,y)[0] 其实这边只是相减操作等而已，用于matlab
	//#define rightImage(x,y) rightImage(x,y)[0]

		//imageChannelNum 图像的通道数
	int imageChannelNum = 3;
	//这里应该是重叠区域像素值相减即可，需要修改下
	//for i=1:Height
	//假设没有高度偏移，有的话后期需要修改

	for (int i = 0;i < Height;i++) {
		//	for j=rectLeftBorder:rectRightBorder
		//rectLeftBorder重叠区域的起始位置
		for (int j = rectLeftBorder;j < rectRightBorder;j++) {
			for (int k = 0;k < imageChannelNum;k++)
			{
				Rectf(i, j - rectLeftBorder + 1, 0, k) = leftImage(i, j, 0, k) - rightImage(i, j - rectLeftBorder + 1, 0, k);
			}
			//Rectf(i,j-rectLeftBorder)=A(i,j)[0]-B(i,j-rectLeftBorder+1)[0];
		}//end
	}//end
	//Rectf=uint8(Rectf);//这句要不要呢？




	//path放路径的矩阵
	//这里为定义二维数组,将path设置为2维数组，且只存放一维数值
	CImg<int> path = CImg<int>(n, Height, 1, 1, 0);
	//放强度值 每条路径的强度值strength=color^2+geometry
	CImg<int> color = CImg<int>(n, 1, 1, 1, 0);
	CImg<int> geometry = CImg<int>(n, 1, 1, 1, 0);

	//leftImageStrength左边图像的强度
	//rightImageStrength右边图像的强度
	CImg<int> leftImageStrength = CImg<int>(n, 1, 1, 3, 0);
	CImg<int> rightImageStrength = CImg<int>(n, 1, 1, 3, 0);

	float Bxdao;
	float Bydao;
	float Aydao;
	float Axdao;

	//计算第一行即初始化的强度值
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

	//开始扩展 向下一行 从第二行到倒数第二行 最后一行单独拿出来 像第一行一样 因为它的结构差值geometry不好算
	//for i=2:Height-1
	for (int j = 1;j < Height - 1;j++) {
		//先把下一行的强度值全部计算出来 到时候需要比较哪三个就拿出哪三个
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
	//比较leftImageStrength里放的每条路径的强度值的总和 谁最小 就选path中对应的那一列的路径
//[minNumber,minIndex]=min(leftImageStrength)
	//minNumber 最小值
	//minIndex 最小值所在的下标参数
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

	//朋友MR_Radish666 说上句应该是：mypath=path.colRange(minIndex-1,minIndex)

	//cout<<"一条最小路径："<<endl<<mypath<<endl;



//mypath放的就是最佳缝合线选出的路径 这条路径坐标是参考图A 右边是目标图B
//for i=1:Height
//这里应该是赋值最佳缝合线的左右两端的数值
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

// 画最佳缝合线
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

	//返回合成的图像
	return D;


}


//3-29修订版，加上下偏移与左右偏移，同时只返回点集，节省空间
//同时优化代码结构
//后期再次进行拆分类
CImg<int> MyImageFusion::bestSeamLineGetLinePoint(CImg<int> leftImage1, CImg<int> rightImage2, int matchVecdx, int matchVecdy)
{
	//通过观察代码可以看到，matchVecdx是重叠区域的水平长度（宽度方向的长度）
	//matchVecdy为竖直方向上的偏移量
	//先根据之前得到的H矩阵计算重叠区域Rect
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
	//Weight宽度,Height高度
	Weight = leftImage._width;
	int Height = leftImage._height;

	//以左边图像的左上角为顶点
	//rectLeftBorder重叠区域的左边界的x坐标
	//rectRightBorder重叠区域的左边界的x坐标即左边图像的边界
	//rectRightBorder=Weight;
	//重叠区域的高度
	int rectLeftBorder = Weight - matchVecdx;
	int rectRightBorder = Weight;
	int rectHeight = Height - abs(matchVecdy);

	//判断matchVecdy>0 则右图在上
	//matchVecdy < 0,则右图在下
	//int rectTopBorder;

	//这里只是计算重叠区域的n而已，并无其他意义，看不懂意义
	int n = rectRightBorder - rectLeftBorder;

	//最终融合图的大小2 * Weight - matchVecdx, leftImage._height + abs(matchVecdy);两幅图像之时
	CImg<int> D = CImg<int>(2 * leftImage._width - abs(matchVecdx), leftImage._height + abs(matchVecdy), 1, 3, 0);

	//计算得到重叠区域的差值图像  其实我不懂计算差值图像干嘛  只要计算重叠区域的大小就好了 为什么还要计算差值图 后面又没用到
	//Rectf 为重叠区域的差值图像
	CImg<int> Rectf = CImg<int>(n, rectHeight, 1, 3, 0);

	//imageChannelNum 图像的通道数
	int imageChannelNum = 3;
	//这里应该是重叠区域像素值相减即可，需要修改下

	//发觉到算法其实只需要重叠区域，无论左右图
	CImg<int> rectLeftImage = CImg<int>(n, rectHeight, 1, 3, 0);
	CImg<int> rectRightImage = CImg<int>(n, rectHeight, 1, 3, 0);

	for (int j = 0;j < Rectf._height;j++)
	{
		//	for j=0:Rectf._height
		//rectLeftBorder重叠区域的起始位置
		int leftY, rightY;
		if (matchVecdy > 0)
		{
			leftY = j; 
			rightY = j + matchVecdy;
		}
		else
		{
			//因为此时的matchVecdy为负值，相当于直接相减,需要取绝对值
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

	//path放路径的矩阵
	//这里为定义二维数组,将path设置为2维数组，且只存放一维数值
	CImg<int> path = CImg<int>(n, rectHeight, 1, 1, 0);
	//放强度值 每条路径的强度值strength=color^2+geometry
	CImg<int> color = CImg<int>(n, 1, 1, 1, 0);
	CImg<int> geometry = CImg<int>(n, 1, 1, 1, 0);

	//leftImageStrength左边图像的强度
	//rightImageStrength右边图像的强度
	CImg<int> leftImageStrength = CImg<int>(n, 1, 1, 3, 0);
	CImg<int> rightImageStrength = CImg<int>(n, 1, 1, 3, 0);

	float Bxdao;
	float Bydao;
	float Aydao;
	float Axdao;

	//计算第一行即初始化的强度值
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
			//这里需要借助原图像来进行处理
			//但是高度这一块需要进行判断是上还是下偏移修改
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

	//开始扩展 向下一行 从第二行到倒数第二行 最后一行单独拿出来 像第一行一样 因为它的结构差值geometry不好算

	for (int j = 1;j < rectHeight - 1;j++) {
		//先把下一行的强度值全部计算出来 到时候需要比较哪三个就拿出哪三个
		//for j=rectLeftBorder:rectRightBorder
		for (int i = 0;i < rectRightBorder - rectLeftBorder;i++) {
			int x = i;
			int y = j;
			int rectStartXPoint = i;
			color(y, 0, 0, 0) = rectLeftImage(i, j, 0, 0) - rectRightImage(x, y, 0, 0);
			if (x == 0)
			{
				//此时需要借助原图像进行处理，补会上述的宽度等信息
				if (matchVecdy > 0)
				{
					Axdao = 2 * leftImage(i + rectLeftBorder + 1, j - 1, 0, 0) + leftImage(i + rectLeftBorder + 1, j, 0, 0) + 2 * leftImage(i + rectLeftBorder + 1, j + 1, 0, 0) - 2 * leftImage(i + rectLeftBorder - 1, j - 1, 0, 0) - leftImage(i + rectLeftBorder - 1, j, 0, 0) - 2 * leftImage(i + rectLeftBorder - 1, j + 1, 0, 0);
					Aydao = -2 * leftImage(i + rectLeftBorder - 1, j - 1, 0, 0) - leftImage(i + rectLeftBorder, j - 1, 0, 0) - 2 * leftImage(i + rectLeftBorder + 1, j - 1, 0, 0) + 2 * leftImage(i + rectLeftBorder - 1, j + 1, 0, 0) + leftImage(i + rectLeftBorder, j + 1, 0, 0) + 2 * leftImage(i + rectLeftBorder + 1, j + 1, 0, 0);
				}
				else
				{//matchVecdy < 0
					//需要加上高度的差距才能到达确定的位置
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
	//比较leftImageStrength里放的每条路径的强度值的总和 谁最小 就选path中对应的那一列的路径
//[minNumber,minIndex]=min(leftImageStrength)
	//minNumber 最小值
	//minIndex 最小值所在的下标参数
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

	//朋友MR_Radish666 说上句应该是：mypath=path.colRange(minIndex-1,minIndex)

	//cout<<"一条最小路径："<<endl<<mypath<<endl;

//mypath放的就是最佳缝合线选出的路径 这条路径坐标是参考图A 右边是目标图B
//这里应该是赋值最佳缝合线的左右两端的数值
	//这里边的赋值就借助了之前的赋值操作
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
	//效率是下边高，但是如果要选择绘制出最佳缝合线还是需要上边调用程序
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
	//返回合成后重叠区域的图像
	return  rectImage;

}



//----2019-4-17----修订增加
//重构
CImg<int> MyImageFusion::bestSeamLineGetLinePoint1(char* inputTestAddr1, char* inputTestAddr2, int matchVecdx, int matchVecdy)
{
	return bestSeamLineGetLinePoint1(inputTestAddr1, inputTestAddr2, matchVecdx, matchVecdy, (char*)"use LaplacianBlending", (char*)"not DrawbestSeamLine");
}


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
//2.暂时不修改读取过来的图像参数，后期需要的时候可以对图像进行扩展，主要是求导的时候有点问题

CImg<int> MyImageFusion::bestSeamLineGetLinePoint1(char* inputTestAddr1, char* inputTestAddr2, int matchVecdx, int matchVecdy, char* isLaplacianBlending,char* isDrawbestSeamLine)
{
	//通过观察代码可以看到，matchVecdx是重叠区域的水平长度（宽度方向的长度）
//matchVecdy为竖直方向上的偏移量
//先根据之前得到的H矩阵计算重叠区域Rect
	CImg<int> leftImage, rightImage;
	leftImage.load(inputTestAddr1);
	rightImage.load(inputTestAddr2);
	int  Weight;
	//Weight宽度,Height高度
	Weight = leftImage._width;
	int Height = leftImage._height;

	//以左边图像的左上角为顶点
	//rectLeftBorder重叠区域的左边界的x坐标
	//rectRightBorder重叠区域的左边界的x坐标即左边图像的边界
	//rectRightBorder=Weight;
	//重叠区域的高度
	int rectLeftBorder = Weight - matchVecdx;
	int rectRightBorder = Weight;
	int rectHeight = Height - abs(matchVecdy);

	//判断matchVecdy>0 则右图在上
	//matchVecdy < 0,则右图在下
	//int rectTopBorder;

	//这里只是计算重叠区域的n而已，并无其他意义，看不懂意义
	int n = rectRightBorder - rectLeftBorder;

	//最终融合图的大小2 * Weight - matchVecdx, leftImage._height + abs(matchVecdy);两幅图像之时
	CImg<int> D = CImg<int>(2 * leftImage._width - abs(matchVecdx), leftImage._height + abs(matchVecdy), 1, 3, 0);

	//计算得到重叠区域的差值图像  其实我不懂计算差值图像干嘛  只要计算重叠区域的大小就好了 为什么还要计算差值图 后面又没用到
	//Rectf 为重叠区域的差值图像
	CImg<int> Rectf = CImg<int>(n, rectHeight, 1, 3, 0);

	//imageChannelNum 图像的通道数
	int imageChannelNum = 3;
	//这里应该是重叠区域像素值相减即可，需要修改下

	//发觉到算法其实只需要重叠区域，无论左右图
	CImg<int> rectLeftImage = CImg<int>(n, rectHeight, 1, 4, 0);
	CImg<int> rectRightImage = CImg<int>(n, rectHeight, 1, 4, 0);

	for (int j = 0;j < Rectf._height;j++)
	{
		//	for j=0:Rectf._height
		//rectLeftBorder重叠区域的起始位置
		int leftY, rightY;
		if (matchVecdy > 0)
		{
			leftY = j;
			rightY = j + matchVecdy;
		}
		else
		{
			//因为此时的matchVecdy为负值，相当于直接相减,需要取绝对值
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

	//path放路径的矩阵
	//这里为定义二维数组,将path设置为2维数组，且只存放一维数值
	CImg<int> path = CImg<int>(n, rectHeight, 1, 1, 0);
	//放强度值 每条路径的强度值strength=color^2+geometry
	CImg<int> color = CImg<int>(n, 1, 1, 1, 0);
	CImg<int> geometry = CImg<int>(n, 1, 1, 1, 0);

	//leftImageStrength左边图像的强度
	//rightImageStrength右边图像的强度
	CImg<int> leftImageStrength = CImg<int>(n, 1, 1, 3, 0);
	CImg<int> rightImageStrength = CImg<int>(n, 1, 1, 3, 0);

	float Bxdao;
	float Bydao;
	float Aydao;
	float Axdao;

	//计算第一行即初始化的强度值
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
			//这里需要借助原图像来进行处理
			//但是高度这一块需要进行判断是上还是下偏移修改
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

			//出现越界
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

	//开始扩展 向下一行 从第二行到倒数第二行 最后一行单独拿出来 像第一行一样 因为它的结构差值geometry不好算




	for (int j = 1;j < rectHeight - 1;j++) {
		//先把下一行的强度值全部计算出来 到时候需要比较哪三个就拿出哪三个
		//for j=rectLeftBorder:rectRightBorder

		for (int i = 0;i < rectRightBorder - rectLeftBorder;i++) {
			int x = i;
			int y = j;
			int rectStartXPoint = i;
#ifndef DEBUG	
			color(i, 0, 0, 0) = rectLeftImage(i, j, 0, 3) - rectRightImage(x, y, 0, 3);

			if (x == 0)
			{
				//此时需要借助原图像进行处理，补会上述的宽度等信息
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
					//需要加上高度的差距才能到达确定的位置
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

	//比较leftImageStrength里放的每条路径的强度值的总和 谁最小 就选path中对应的那一列的路径
//[minNumber,minIndex]=min(leftImageStrength)
	//minNumber 最小值
	//minIndex 最小值所在的下标参数
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

	//朋友MR_Radish666 说上句应该是：mypath=path.colRange(minIndex-1,minIndex)

	//cout<<"一条最小路径："<<endl<<mypath<<endl;

//mypath放的就是最佳缝合线选出的路径 这条路径坐标是参考图A 右边是目标图B
//这里应该是赋值最佳缝合线的左右两端的数值
	//这里边的赋值就借助了之前的赋值操作


	//对请求过来的参数进行判断，
	//isLaplacianBlending == "use LaplacianBlending"  拉普拉斯融合
	//isLaplacianBlending == "no use LaplacianBlending"  不进行拉普拉斯融合
	//isDrawbestSeamLine == "DrawbestSeamLine" 画出最佳缝合线
	//isDrawbestSeamLine == "not DrawbestSeamLine" 不画出最佳缝合线

	CImg<int> BesbestSeamLineBlendingImage;
	if (strcmp(isLaplacianBlending, (char*)"no use LaplacianBlending") == 0) 
	{
		BesbestSeamLineBlendingImage = CImg<int>(leftImage._width, leftImage._height, 1, leftImage._spectrum, 0);
		//不进行多分辨率融合
		
	for (int y = 0;y < Rectf._height;y++)
	{
		for (int x = 0;x < mypath(y, 0, 0, 0);x++)
		{
			//通道数
			for (int k = 0;k < leftImage._spectrum;k++)
				BesbestSeamLineBlendingImage(x, y, 0, k) = rectLeftImage(x, y, 0, k);
			
		}
		for (int x = mypath(y, 0, 0, 0);x < Rectf._width;x++)
		{
			//通道数
			for (int k = 0;k < leftImage._spectrum;k++)
				BesbestSeamLineBlendingImage(x, y, 0, k) = rectRightImage(x, y, 0, k);
		}
	}
	}
	else //if (strcmp(isLaplacianBlending, (char*)"use LaplacianBlending")) 先省略，可以拆分函数代码，为以后多分辨率融合考虑
	{
		//进行多分辨率融合
		//当传入其它错误的参数，默认先放进去
	//将重叠区域以1|0进行生成图像
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

	//需要重新设置为三通道
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

	cout << "进行融合操作"<<endl;
	MyLaplacianBlending myreplacianoperatorObj = MyLaplacianBlending();
	BesbestSeamLineBlendingImage = myreplacianoperatorObj.MyLaplacianBlend(rectLeftImageName, rectRightImageName, blendWhiteBlackImageName);
	
	}
	
	if (strcmp(isDrawbestSeamLine, (char*)"DrawbestSeamLine")==0) 
	{
		//画最佳缝合线
		BesbestSeamLineBlendingImage = drawbestSeamLine(BesbestSeamLineBlendingImage, mypath);
	}
	else 
	{
		//不画
		//不操作
	}
	
	return BesbestSeamLineBlendingImage;
}

//绘画最佳缝合线，需要传入重叠区域的图像以及最佳缝合线的点集，
//rectImage重叠区域的图像，bestSeamLinePath一维的最佳缝合线点集
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
	//读取左右两边的图像
	CImg<int> leftImage, rightImage;
	leftImage.load(inputTestAddr1);
	rightImage.load(inputTestAddr2);

	//rectLeftBorder重叠区域的左边界的x坐标
	//rectRightBorder重叠区域的左边界的x坐标即左边图像的边界
	//rectRightBorder=Weight;
	//重叠区域的高度
	int rectLeftBorder = leftImage._width - abs(matchVecdx);
	int rectRightBorder = leftImage._width;
	int rectHeight = leftImage._height - abs(matchVecdy);


}