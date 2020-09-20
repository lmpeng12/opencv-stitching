#include "MyMorePictureStitch.h"


MyMorePictureStitch::MyMorePictureStitch(){
}


MyMorePictureStitch::~MyMorePictureStitch(){
}

MyMorePictureStitch::MyMorePictureStitch(int sx, int sy) {
//	matchVec.dx = sx;
//	matchVec.dy = sy;
	
}

void MyMorePictureStitch::SaveSequentialImagesDifference(char* _filenameA, char* _filenameB, int sx, int sy)
{
	//将两幅图像相差的dy存储起来
	struct HeightRecord *newRecord = (struct HeightRecord*)malloc(sizeof(struct HeightRecord));
	newRecord->img1_name = _filenameA;
	newRecord->img2_name = _filenameB;
	newRecord->dx = sx;
	newRecord->dy = sy;
	if (head == NULL)
	{
		head = newRecord;
		nowRecord = newRecord;
	}
	else
		nowRecord->rightRecord = newRecord;
}
//初期版本
CImg<int> MyMorePictureStitch::MyStitchImage(char* _filenameA, char* _filenameB, CImg<int> lastblendedImg,int matchVecdx,int matchVecdy) {
	CImg<int> srcImgA, srcImgB;
	//将两幅图像之间的x和y的偏移量进行存储
	SaveSequentialImagesDifference(_filenameA, _filenameB, matchVecdx, matchVecdy);

	//读取图像
	srcImgA.load(_filenameA);
	srcImgB.load(_filenameB);

	//判断传入的上一个图像的是否为空
	//若为空则给其赋予图像a的数值
	if(lastblendedImg==NULL)
	{
		lastblendedImg = srcImgA;
	}

	//判断是最初开始的图片
	if (StitchPictureNum == 0) 
	{
		//若是的话讲初始图片的宽度和高度都保存下来
		oriImageWidth = lastblendedImg._width;
		oriImageHeight = lastblendedImg._height;
	}

	//判断该图像待拼接图像是否在右边，是的话坐上记录，并且进行图像扩展的调整
	if (matchVecdx > srcImgB._width)
	{
		//2019-03-09修改内容
		//需要拼接的图片在左边
		//新的记录值进行登记替换
		//待真实的 = （拼接图像 - dx/2）*2；
		//可以分析纸2先拼接全右图片
		//leftmatchVec真正的重合区域
		//matchVecx为左边剩余的待拼接面积
		int leftmatchVec = (srcImgB._width* 2 - matchVecdx);

		//与拼接右边图像不同的是，需要将dy先取负值
		//判断当前图像相对于上一个图像是向上移动还是向下移动
		//小于0，向下移动
		//大于0，向上移动
		//公式：dy1 = img2.y - img1.y
		//dy2 = img3.y - img2.y;
		//img3相对于img1:
		//dy = dy1+dy2 = img3.y - img1.y;
		//首先先计算偏移量，看是相对与第一个图像是向上还是向下
		//取负数
		lastcalureHeight = lastcalureHeight + (0- matchVecdy);

		if (lastcalureHeight <= 0)
		{
			//负数比较谁最小
			//判断相对于最下的偏移量，如果小于扩增后的最大向下行，就无需要增大下行数
			//如果大于的话，就更新下的行数，同时更新最下的值
			MaxDown = MaxDown < lastcalureHeight ? MaxDown : lastcalureHeight;

		}
		else//向上移动 
		{
			//正数比较谁最大
			//判断相对于最上的偏移量，如果小于扩增后的最大向上行，就无需要增大上行数
				//如果大于的话，就更新上的行数，同时更新最上的值
			MaxUp = MaxUp > matchVecdy ? MaxUp : matchVecdy;
		}

		//计算出当前的图像相对于第一个图像横向偏移了多少
		lastcalureWeight = lastcalureWeight -(srcImgB._width - leftmatchVec) ;
		if (lastcalureWeight > 0)
			MaxRight = MaxRight > lastcalureWeight ? MaxRight : lastcalureWeight;
		else
			MaxLeft = MaxLeft < lastcalureWeight ? MaxLeft : lastcalureWeight;

		//待拼接的图片在图像的左边
		//左边的图像的相对纵向的方向的偏移值是相反数
		//dx得用新的表示方法
		//-----------------------合并的窗口大小位置-----------------------------------------
		//dx为两幅图像的特征点之间的横的差值
		//dy为两幅图像的特征点之间的竖的差值
		//判断是否需要增加画布的长度
		//宽度上是原图像，加上上增加值同时加上下增加值
		//若用上一个图像判断是否扩增行的高度
		//增加初始化图像的大小
		//宽度上是原图像，加上上增加值同时加上下增加值
		//下面的原理为所有宽度叠加取代原来的传入图像的宽度，
		//减少在拼接左边的图像时候出现需要库充左边图像的情况
		//blendedImg = CImg<int>(lastblendedImg._width + srcImgB._width - matchVecdx,
		//	srcImgB._height + abs(MaxDown) + abs(MaxUp), 1, 3, 0);
		blendedImg = CImg<int>(oriImageWidth + abs(MaxLeft) + abs(MaxRight),
			srcImgB._height + abs(MaxDown) + abs(MaxUp), 1, 3, 0);

		//判断新加入的图像是相对于上一个图像上移动还是下移动
	//判断x属于左边拼接好或者是第一个图像的区域
	//如果这个图像的点恰好在两个图像的重叠之处，需要做插值处理
	//计算重叠区域的插值的时候，将重合区域的变量同时处于重叠区域的宽度
	//此处需要重新讨论
		//此处的拼接是针对于待拼接图像在匹配的图像的右边
		cimg_forXY(blendedImg, x, y) {
			if (matchVecdy <= 0) {    //右侧图片需要往下左移动
				if (x < lastblendedImg._width && y < lastblendedImg._height) {
					//此处的公式为累计偏移量，加上图像未赋值的部分面积宽度 = 实际重叠部分
					if (x >= (lastcalureWeight + (srcImgB._width- leftmatchVec)) && y >= (0 - lastcalureHeight)) {    //混合
						blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
							* (float)(lastblendedImg._width - x) / (float)abs(matchVecdx)
							+ (float)srcImgB(x - (lastblendedImg._width - matchVecdx), y - (0 - lastcalureHeight), 0, 0)
							* (float)(x - (lastblendedImg._width - matchVecdx)) / (float)abs(matchVecdx);
						blendedImg(x, y, 0, 1) = (float)lastblendedImg(x, y, 0, 1)
							* (float)(lastblendedImg._width - x) / (float)abs(matchVecdx)
							+ (float)srcImgB(x - (lastblendedImg._width - matchVecdx), y - (0 - lastcalureHeight), 0, 1)
							* (float)(x - (lastblendedImg._width - matchVecdx)) / (float)abs(matchVecdx);
						blendedImg(x, y, 0, 2) = (float)lastblendedImg(x, y, 0, 2)
							* (float)(lastblendedImg._width - x) / (float)abs(matchVecdx)
							+ (float)srcImgB(x - (lastblendedImg._width - matchVecdx), y - (0 - lastcalureHeight), 0, 2)
							* (float)(x - (lastblendedImg._width - matchVecdx)) / (float)abs(matchVecdx);

					}
					else {    //A或者左边拼接好的独在部分
						blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
						blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
						blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
					}

				}
				//y部分应该是在左边图像的下边，右边
				else if (x >= (lastblendedImg._width - matchVecdx)
					&& y >= (0 - lastcalureHeight) && y < (0 - lastcalureHeight) + srcImgB._height) {    //B独在部分
					blendedImg(x, y, 0, 0) = srcImgB(x - (lastblendedImg._width - matchVecdx), y - (0 - lastcalureHeight), 0, 0);
					blendedImg(x, y, 0, 1) = srcImgB(x - (lastblendedImg._width - matchVecdx), y - (0 - lastcalureHeight), 0, 1);
					blendedImg(x, y, 0, 2) = srcImgB(x - (lastblendedImg._width - matchVecdx), y - (0 - lastcalureHeight), 0, 2);
				}
				else {    //黑色部分
					blendedImg(x, y, 0, 0) = 0;
					blendedImg(x, y, 0, 1) = 0;
					blendedImg(x, y, 0, 2) = 0;
				}
			}
			//3-9号修改
			/**/else {    //matchVec.dy > 0; 右侧图片需要往上左移动
				if (x < lastblendedImg._width && y >= lastcalureHeight) {
					if (x >= (lastblendedImg._width - matchVecdx) && y < srcImgB._height) {    //混合
						blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y - lastcalureHeight, 0, 0)
							* (float)(lastblendedImg._width - x) / (float)abs(matchVecdx)
							+ (float)srcImgB(x - (lastblendedImg._width - matchVecdx), y, 0, 0)
							* (float)(x - (lastblendedImg._width - matchVecdx)) / (float)abs(matchVecdx);
						blendedImg(x, y, 0, 1) = (float)srcImgA(x, y - lastcalureHeight, 0, 1)
							* (float)(lastblendedImg._width - x) / (float)abs(matchVecdx)
							+ (float)srcImgB(x - (lastblendedImg._width - matchVecdx), y, 0, 1)
							* (float)(x - (lastblendedImg._width - matchVecdx)) / (float)abs(matchVecdx);
						blendedImg(x, y, 0, 2) = (float)srcImgA(x, y - lastcalureHeight, 0, 2)
							* (float)(lastblendedImg._width - x) / (float)abs(matchVecdx)
							+ (float)srcImgB(x - (lastblendedImg._width - matchVecdx), y, 0, 2)
							* (float)(x - (lastblendedImg._width - matchVecdx)) / (float)abs(matchVecdx);
					}
					else {    //A独在部分
						blendedImg(x, y, 0, 0) = srcImgA(x, y - lastcalureHeight, 0, 0);
						blendedImg(x, y, 0, 1) = srcImgA(x, y - lastcalureHeight, 0, 1);
						blendedImg(x, y, 0, 2) = srcImgA(x, y - lastcalureHeight, 0, 2);
					}
				}
				else if (x >= (lastblendedImg._width - matchVecdx) && y < srcImgB._height) {    //B独在部分
					blendedImg(x, y, 0, 0) = srcImgB(x - (lastblendedImg._width - matchVecdx), y, 0, 0);
					blendedImg(x, y, 0, 1) = srcImgB(x - (lastblendedImg._width - matchVecdx), y, 0, 1);
					blendedImg(x, y, 0, 2) = srcImgB(x - (lastblendedImg._width - matchVecdx), y, 0, 2);
				}
				else {    //黑色部分
					blendedImg(x, y, 0, 0) = 0;
					blendedImg(x, y, 0, 1) = 0;
					blendedImg(x, y, 0, 2) = 0;
				}
			}

		}



	}
	else
	{
		//需要拼接的图像在右边
		//判断当前图像相对于上一个图像是向上移动还是向下移动
		//小于0，向下移动
		//大于0，向上移动
		//公式：dy1 = img2.y - img1.y
		//dy2 = img3.y - img2.y;
		//img3相对于img1:
		//dy = dy1+dy2 = img3.y - img1.y;
		//首先先计算偏移量，看是相对与第一个图像是向上还是向下
		lastcalureHeight += matchVecdy;
		if (lastcalureHeight <= 0)
		{
			//负数比较谁最小
			//判断相对于最下的偏移量，如果小于扩增后的最大向下行，就无需要增大下行数
			//如果大于的话，就更新下的行数，同时更新最下的值
			MaxDown = MaxDown < lastcalureHeight ? MaxDown : lastcalureHeight;

		}
		else//向上移动 
		{
			//正数比较谁最大
			//判断相对于最上的偏移量，如果小于扩增后的最大向上行，就无需要增大上行数
				//如果大于的话，就更新上的行数，同时更新最上的值
			MaxUp = MaxUp > lastcalureHeight ? MaxUp : lastcalureHeight;
		}

		//计算出当前的图像相对于第一个图像横向偏移了多少
		lastcalureWeight = lastcalureWeight + srcImgB._width - matchVecdx;
		if (lastcalureWeight > 0)
			MaxRight = MaxRight > lastcalureWeight ? MaxRight : lastcalureWeight;
		else
			MaxLeft = MaxLeft < lastcalureWeight ? MaxLeft : lastcalureWeight;

		//待拼接的图片在图像的右边
		//-----------------------合并的窗口大小位置-----------------------------------------
		//dx为两幅图像的特征点之间的横的差值
		//dy为两幅图像的特征点之间的竖的差值
		//判断是否需要增加画布的长度
		//宽度上是原图像，加上上增加值同时加上下增加值
		//若用上一个图像判断是否扩增行的高度
		//增加初始化图像的大小
		//下面的原理为所有宽度叠加取代原来的传入图像的宽度，
		//减少在拼接左边的图像时候出现需要库充左边图像的情况
		//blendedImg = CImg<int>(lastblendedImg._width + srcImgB._width - matchVecdx,
		//	srcImgB._height + abs(MaxDown) + abs(MaxUp), 1, 3, 0);

		//
		blendedImg = CImg<int>(oriImageWidth + abs(MaxLeft) + abs(MaxRight),
			srcImgB._height + abs(MaxDown) + abs(MaxUp), 1, 3, 0);
	

	//判断新加入的图像是相对于上一个图像上移动还是下移动
	//判断x属于左边拼接好或者是第一个图像的区域
	//如果这个图像的点恰好在两个图像的重叠之处，需要做插值处理
	//计算重叠区域的插值的时候，将重合区域的变量同时处于重叠区域的宽度
	//此处需要重新讨论
	cimg_forXY(blendedImg, x, y) {
		if (matchVecdy <= 0) {    //右侧图片需要往下左移动
			//判断该图像是相对于第一幅图像位置是向下还是向上
			

				if (lastcalureHeight == MaxDown|| lastcalureHeight==MaxUp) 
				{
				//属于增加高度的部分情况，按照3-9号代码

				}
				else
				{
					//属于不需要增加高度的部分，按照3-10号代码另行处理
					if (x < lastblendedImg._width && y < lastblendedImg._height) {
						//判断待拼接的图像与第一个图像相比较是在还是在下边
						if (lastcalureHeight > 0) 
						{
						//该图片相对第一幅图像在上边，重叠区域为上顶面减去上偏移开始到待拼接这幅图像结束
						}
						else 
						{
						//该图片相对第一幅图像在下边，重叠区域为下顶边减去下偏移，得出第二幅图像的结尾像素距离整幅图像的距离
							//lastblendedImg._height - abs(MaxDown - lastcalureHeight)为第二幅图像结束拼接时候的高度
							if (x >= (lastblendedImg._width - matchVecdx) && y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight)
								-srcImgB._height && y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight)- matchVecdy) {    //混合
								blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
									* (float)(lastblendedImg._width - x) / (float)abs(matchVecdx)
									+ (float)srcImgB(x - (lastblendedImg._width - matchVecdx), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight)- srcImgB._height), 0, 0)
									* (float)(x - (lastblendedImg._width - matchVecdx)) / (float)abs(matchVecdx);
								blendedImg(x, y, 0, 1) = (float)lastblendedImg(x, y, 0, 1)
									* (float)(lastblendedImg._width - x) / (float)abs(matchVecdx)
									+ (float)srcImgB(x - (lastblendedImg._width - matchVecdx), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1)
									* (float)(x - (lastblendedImg._width - matchVecdx)) / (float)abs(matchVecdx);
								blendedImg(x, y, 0, 2) = (float)lastblendedImg(x, y, 0, 2)
									* (float)(lastblendedImg._width - x) / (float)abs(matchVecdx)
									+ (float)srcImgB(x - (lastblendedImg._width - matchVecdx), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2)
									* (float)(x - (lastblendedImg._width - matchVecdx)) / (float)abs(matchVecdx);

							}
							else {    //A或者左边拼接好的独在部分
								blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
								blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
								blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
							}
						
						}
						if (x >= (lastblendedImg._width - matchVecdx) && y >= (0 - lastcalureHeight)&&y <= abs(MaxDown- lastcalureHeight)) {    //混合
							blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
								* (float)(lastblendedImg._width - x) / (float)abs(matchVecdx)
								+ (float)srcImgB(x - (lastblendedImg._width - matchVecdx), y - (0 - lastcalureHeight), 0, 0)
								* (float)(x - (lastblendedImg._width - matchVecdx)) / (float)abs(matchVecdx);
							blendedImg(x, y, 0, 1) = (float)lastblendedImg(x, y, 0, 1)
								* (float)(lastblendedImg._width - x) / (float)abs(matchVecdx)
								+ (float)srcImgB(x - (lastblendedImg._width - matchVecdx), y - (0 - lastcalureHeight), 0, 1)
								* (float)(x - (lastblendedImg._width - matchVecdx)) / (float)abs(matchVecdx);
							blendedImg(x, y, 0, 2) = (float)lastblendedImg(x, y, 0, 2)
								* (float)(lastblendedImg._width - x) / (float)abs(matchVecdx)
								+ (float)srcImgB(x - (lastblendedImg._width - matchVecdx), y - (0 - lastcalureHeight), 0, 2)
								* (float)(x - (lastblendedImg._width - matchVecdx)) / (float)abs(matchVecdx);

						}
						else {    //A或者左边拼接好的独在部分
							blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
							blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
							blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
						}

					}
					//y部分应该是在左边图像的下边，右边
					else if (x >= (lastblendedImg._width - matchVecdx)
						&& y >= (0 - lastcalureHeight) && y < (0 - lastcalureHeight) + srcImgB._height) {    //B独在部分
						blendedImg(x, y, 0, 0) = srcImgB(x - (lastblendedImg._width - matchVecdx), y - (0 - lastcalureHeight), 0, 0);
						blendedImg(x, y, 0, 1) = srcImgB(x - (lastblendedImg._width - matchVecdx), y - (0 - lastcalureHeight), 0, 1);
						blendedImg(x, y, 0, 2) = srcImgB(x - (lastblendedImg._width - matchVecdx), y - (0 - lastcalureHeight), 0, 2);
					}
					else {    //黑色部分
						blendedImg(x, y, 0, 0) = 0;
						blendedImg(x, y, 0, 1) = 0;
						blendedImg(x, y, 0, 2) = 0;
					}

				}
			

			if (x < lastblendedImg._width && y < lastblendedImg._height) {
				if (x >= (lastblendedImg._width - matchVecdx) && y >= (0 - lastcalureHeight)) {    //混合
					blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
						* (float)(lastblendedImg._width - x) / (float)abs(matchVecdx)
						+ (float)srcImgB(x - (lastblendedImg._width - matchVecdx), y - (0 - lastcalureHeight), 0, 0)
						* (float)(x - (lastblendedImg._width - matchVecdx)) / (float)abs(matchVecdx);
					blendedImg(x, y, 0, 1) = (float)lastblendedImg(x, y, 0, 1)
						* (float)(lastblendedImg._width - x) / (float)abs(matchVecdx)
						+ (float)srcImgB(x - (lastblendedImg._width - matchVecdx), y - (0 - lastcalureHeight), 0, 1)
						* (float)(x - (lastblendedImg._width - matchVecdx)) / (float)abs(matchVecdx);
					blendedImg(x, y, 0, 2) = (float)lastblendedImg(x, y, 0, 2)
						* (float)(lastblendedImg._width - x) / (float)abs(matchVecdx)
						+ (float)srcImgB(x - (lastblendedImg._width - matchVecdx), y - (0 - lastcalureHeight), 0, 2)
						* (float)(x - (lastblendedImg._width - matchVecdx)) / (float)abs(matchVecdx);

				}
				else {    //A或者左边拼接好的独在部分
					blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
					blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
					blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
				}
				
			}
			//y部分应该是在左边图像的下边，右边
			else if (x >= (lastblendedImg._width - matchVecdx)
				&& y >= (0 - lastcalureHeight) && y < (0 - lastcalureHeight) + srcImgB._height) {    //B独在部分
				blendedImg(x, y, 0, 0) = srcImgB(x - (lastblendedImg._width - matchVecdx), y - (0 - lastcalureHeight), 0, 0);
				blendedImg(x, y, 0, 1) = srcImgB(x - (lastblendedImg._width - matchVecdx), y - (0 - lastcalureHeight), 0, 1);
				blendedImg(x, y, 0, 2) = srcImgB(x - (lastblendedImg._width - matchVecdx), y - (0 - lastcalureHeight), 0, 2);
			}
			else {    //黑色部分
				blendedImg(x, y, 0, 0) = 0;
				blendedImg(x, y, 0, 1) = 0;
				blendedImg(x, y, 0, 2) = 0;
			}
		}
		//3-9号修改
		/**/else {    //matchVec.dy > 0; 右侧图片需要往上左移动
			if (x < lastblendedImg._width && y >= lastcalureHeight) {
				if (x >= (lastblendedImg._width - matchVecdx) && y < srcImgB._height) {    //混合
					blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y - lastcalureHeight, 0, 0)
						* (float)(lastblendedImg._width - x) / (float)abs(matchVecdx)
						+ (float)srcImgB(x - (lastblendedImg._width - matchVecdx), y, 0, 0)
						* (float)(x - (lastblendedImg._width - matchVecdx)) / (float)abs(matchVecdx);
					blendedImg(x, y, 0, 1) = (float)lastblendedImg(x, y - lastcalureHeight, 0, 1)
						* (float)(lastblendedImg._width - x) / (float)abs(matchVecdx)
						+ (float)srcImgB(x - (lastblendedImg._width - matchVecdx), y, 0, 1)
						* (float)(x - (lastblendedImg._width - matchVecdx)) / (float)abs(matchVecdx);
					blendedImg(x, y, 0, 2) = (float)lastblendedImg(x, y - lastcalureHeight, 0, 2)
						* (float)(lastblendedImg._width - x) / (float)abs(matchVecdx)
						+ (float)srcImgB(x - (lastblendedImg._width - matchVecdx), y, 0, 2)
						* (float)(x - (lastblendedImg._width - matchVecdx)) / (float)abs(matchVecdx);
				}
				else {    //A独在部分
					blendedImg(x, y, 0, 0) = lastblendedImg(x, y - lastcalureHeight, 0, 0);
					blendedImg(x, y, 0, 1) = lastblendedImg(x, y - lastcalureHeight, 0, 1);
					blendedImg(x, y, 0, 2) = lastblendedImg(x, y - lastcalureHeight, 0, 2);
				}
			}
			else if (x >= (lastblendedImg._width - matchVecdx) && y < srcImgB._height) {    //B独在部分
				blendedImg(x, y, 0, 0) = srcImgB(x - (lastblendedImg._width - matchVecdx), y, 0, 0);
				blendedImg(x, y, 0, 1) = srcImgB(x - (lastblendedImg._width - matchVecdx), y, 0, 1);
				blendedImg(x, y, 0, 2) = srcImgB(x - (lastblendedImg._width - matchVecdx), y, 0, 2);
			}
			else {    //黑色部分
				blendedImg(x, y, 0, 0) = 0;
				blendedImg(x, y, 0, 1) = 0;
				blendedImg(x, y, 0, 2) = 0;
			}
		}

	}

	}

	//这段注释应该是没有用处的
/*cimg_forXY(blendedImg, x, y) {
	if (matchVecdy <= 0) {    //右侧图片需要往下左移动
		if (x < lastblendedImg._width && y < lastblendedImg._height) {
			//此处的公式为累计偏移量，加上图像未赋值的部分面积宽度 = 实际重叠部分
			if (x >= (lastcalureWeight + (srcImgB._width - leftmatchVec)) && y >= (0 - lastcalureHeight)) {    //混合
				blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
					* (float)(lastblendedImg._width - x) / (float)abs(matchVecdx)
					+ (float)srcImgB(x - (lastblendedImg._width - matchVecdx), y - (0 - lastcalureHeight), 0, 0)
					* (float)(x - (lastblendedImg._width - matchVecdx)) / (float)abs(matchVecdx);
				blendedImg(x, y, 0, 1) = (float)lastblendedImg(x, y, 0, 1)
					* (float)(lastblendedImg._width - x) / (float)abs(matchVecdx)
					+ (float)srcImgB(x - (lastblendedImg._width - matchVecdx), y - (0 - lastcalureHeight), 0, 1)
					* (float)(x - (lastblendedImg._width - matchVecdx)) / (float)abs(matchVecdx);
				blendedImg(x, y, 0, 2) = (float)lastblendedImg(x, y, 0, 2)
					* (float)(lastblendedImg._width - x) / (float)abs(matchVecdx)
					+ (float)srcImgB(x - (lastblendedImg._width - matchVecdx), y - (0 - lastcalureHeight), 0, 2)
					* (float)(x - (lastblendedImg._width - matchVecdx)) / (float)abs(matchVecdx);

			}
			else {    //A或者左边拼接好的独在部分
				blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
				blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
				blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
			}

		}
		//y部分应该是在左边图像的下边，右边
		else if (x >= (lastblendedImg._width - matchVecdx)
			&& y >= (0 - lastcalureHeight) && y < (0 - lastcalureHeight) + srcImgB._height) {    //B独在部分
			blendedImg(x, y, 0, 0) = srcImgB(x - (lastblendedImg._width - matchVecdx), y - (0 - lastcalureHeight), 0, 0);
			blendedImg(x, y, 0, 1) = srcImgB(x - (lastblendedImg._width - matchVecdx), y - (0 - lastcalureHeight), 0, 1);
			blendedImg(x, y, 0, 2) = srcImgB(x - (lastblendedImg._width - matchVecdx), y - (0 - lastcalureHeight), 0, 2);
		}
		else {    //黑色部分
			blendedImg(x, y, 0, 0) = 0;
			blendedImg(x, y, 0, 1) = 0;
			blendedImg(x, y, 0, 2) = 0;
		}
	}
	//3-9号修改
	else {    //matchVec.dy > 0; 右侧图片需要往上左移动
		if (x < lastblendedImg._width && y >= lastcalureHeight) {
			if (x >= (lastblendedImg._width - matchVecdx) && y < srcImgB._height) {    //混合
				blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y - lastcalureHeight, 0, 0)
					* (float)(lastblendedImg._width - x) / (float)abs(matchVecdx)
					+ (float)srcImgB(x - (lastblendedImg._width - matchVecdx), y, 0, 0)
					* (float)(x - (lastblendedImg._width - matchVecdx)) / (float)abs(matchVecdx);
				blendedImg(x, y, 0, 1) = (float)srcImgA(x, y - lastcalureHeight, 0, 1)
					* (float)(lastblendedImg._width - x) / (float)abs(matchVecdx)
					+ (float)srcImgB(x - (lastblendedImg._width - matchVecdx), y, 0, 1)
					* (float)(x - (lastblendedImg._width - matchVecdx)) / (float)abs(matchVecdx);
				blendedImg(x, y, 0, 2) = (float)srcImgA(x, y - lastcalureHeight, 0, 2)
					* (float)(lastblendedImg._width - x) / (float)abs(matchVecdx)
					+ (float)srcImgB(x - (lastblendedImg._width - matchVecdx), y, 0, 2)
					* (float)(x - (lastblendedImg._width - matchVecdx)) / (float)abs(matchVecdx);
			}
			else {    //A独在部分
				blendedImg(x, y, 0, 0) = srcImgA(x, y - lastcalureHeight, 0, 0);
				blendedImg(x, y, 0, 1) = srcImgA(x, y - lastcalureHeight, 0, 1);
				blendedImg(x, y, 0, 2) = srcImgA(x, y - lastcalureHeight, 0, 2);
			}
		}
		else if (x >= (lastblendedImg._width - matchVecdx) && y < srcImgB._height) {    //B独在部分
			blendedImg(x, y, 0, 0) = srcImgB(x - (lastblendedImg._width - matchVecdx), y, 0, 0);
			blendedImg(x, y, 0, 1) = srcImgB(x - (lastblendedImg._width - matchVecdx), y, 0, 1);
			blendedImg(x, y, 0, 2) = srcImgB(x - (lastblendedImg._width - matchVecdx), y, 0, 2);
		}
		else {    //黑色部分
			blendedImg(x, y, 0, 0) = 0;
			blendedImg(x, y, 0, 1) = 0;
			blendedImg(x, y, 0, 2) = 0;
		}
	}

}
*/
	blendedImg.display("blendedImg");
	
	if (StitchPictureNum == 0)
		StitchPictureNum = 2;
	else
		StitchPictureNum++;
	return blendedImg;
}

//3-11版本，大版本更新
//1.模块化分析所以情况，并将所有情况分离
//将所有的变量所有点的区域移动进入不同的情况分支，方便后期管理当个，不会影响使用

//2.补充各种情况的发生修改

//3-13 修改左边图像的思想，左边图像相对右边的情况，大部分都是右边相对左边的坐标系
//需要修改思路右边相对于左边图像为

//3-14增加右边图像并未增加宽度的情况
//同时对左边拼接的一些图像像素处理进行重新的分析和整理，修改bug

//3-15对所有拼接的地方进行了修改
//3-31将融合部分的代码抽象出来新的类进行选择哪种融合方式，主要是对融合区域的图像进行抽象，提前备份一份放入百度云
//2019-4-17重新修改名字
CImg<int> MyMorePictureStitch::MyStitchImageByGradualFusionBlend(char* _filenameA, char* _filenameB, CImg<int> lastblendedImg, int matchVecdx, int matchVecdy)
{
	CImg<int> srcImgA, srcImgB;
	//将两幅图像之间的x和y的偏移量进行存储
	SaveSequentialImagesDifference(_filenameA, _filenameB, matchVecdx, matchVecdy);

	//读取图像
	srcImgA.load(_filenameA);
	srcImgB.load(_filenameB);

	//判断传入的上一个图像的是否为空
	//若为空则给其赋予图像a的数值
	if (lastblendedImg == NULL)
	{
		lastblendedImg = srcImgA;
	}

	//判断是最初开始的图片
	if (StitchPictureNum == 0)
	{
		//若是的话讲初始图片的宽度和高度都保存下来
		oriImageWidth = lastblendedImg._width;
		oriImageHeight = lastblendedImg._height;
	}

	//判断该图像待拼接图像是否在左边
	//是的话做上记录，并且进行图像扩展的调整
	if (matchVecdx > srcImgB._width)
	{
		//2019-03-09修改内容
		//需要拼接的图片在左边
		//新的记录值进行登记替换
		//待真实的 = （拼接图像 - dx/2）*2；
		//可以分析纸2先拼接全右图片
		//leftmatchVec真正的重合区域
		//matchVecx为左边剩余的待拼接面积
		int leftmatchVec = (srcImgB._width * 2 - matchVecdx );

		//与拼接右边图像不同的是，需要将dy先取负值
		//判断当前图像相对于上一个图像是向上移动还是向下移动
		//小于0，向下移动
		//大于0，向上移动
		//公式：dy1 = img2.y - img1.y
		//dy2 = img3.y - img2.y;
		//img3相对于img1:
		//dy = dy1+dy2 = img3.y - img1.y;
		//首先先计算偏移量，看是相对与第一个图像是向上还是向下
		//取负数
		//同时将偏移量取负数，转换为正确的图片放置位置相对偏移量
		//matchVecdy = 0 - matchVecdy;
		lastcalureHeight = lastcalureHeight + (matchVecdy);

		if (lastcalureHeight <= 0)
		{
			//负数比较谁最小
			//判断相对于最下的偏移量，如果小于扩增后的最大向下行，就无需要增大下行数
			//如果大于的话，就更新下的行数，同时更新最下的值
			MaxDown = MaxDown < lastcalureHeight ? MaxDown : lastcalureHeight;

		}
		else//向上移动 
		{
			//正数比较谁最大
			//判断相对于最上的偏移量，如果小于扩增后的最大向上行，就无需要增大上行数
				//如果大于的话，就更新上的行数，同时更新最上的值
			MaxUp = MaxUp > lastcalureHeight ? MaxUp : lastcalureHeight;
		}

		//计算出当前的图像相对于第一个图像横向偏移了多少
		lastcalureWeight = lastcalureWeight - (srcImgB._width - leftmatchVec);
		if (lastcalureWeight > 0)
			MaxRight = MaxRight > lastcalureWeight ? MaxRight : lastcalureWeight;
		else
			MaxLeft = MaxLeft < lastcalureWeight ? MaxLeft : lastcalureWeight;

		//待拼接的图片在图像的左边
		//左边的图像的相对纵向的方向的偏移值是相反数
		//dx得用新的表示方法
		//-----------------------合并的窗口大小位置-----------------------------------------
		//dx为两幅图像的特征点之间的横的差值
		//dy为两幅图像的特征点之间的竖的差值
		//判断是否需要增加画布的长度
		//宽度上是原图像，加上上增加值同时加上下增加值
		//若用上一个图像判断是否扩增行的高度
		//增加初始化图像的大小
		//宽度上是原图像，加上上增加值同时加上下增加值
		//下面的原理为所有宽度叠加取代原来的传入图像的宽度，
		//减少在拼接左边的图像时候出现需要库充左边图像的情况
		//blendedImg = CImg<int>(lastblendedImg._width + srcImgB._width - matchVecdx,
		//	srcImgB._height + abs(MaxDown) + abs(MaxUp), 1, 3, 0);
		blendedImg = CImg<int>(oriImageWidth + abs(MaxLeft) + abs(MaxRight),
			srcImgB._height + abs(MaxDown) + abs(MaxUp), 1, 3, 0);

		//判断新加入的图像是相对于上一个图像上移动还是下移动
		//判断x属于左边拼接好或者是第一个图像的区域
		//如果这个图像的点恰好在两个图像的重叠之处，需要做插值处理
		//计算重叠区域的插值的时候，将重合区域的变量同时处于重叠区域的宽度
		//此处需要重新讨论
		//判断是否从左边开始扩展图像
		if (lastcalureWeight == MaxLeft) 
		{
			//该图像拼接的时候需要从左边增加空间
			//并且待拼接图像在左边
			//判断该图像是否引起高度的变化
			if (lastcalureHeight == MaxDown || lastcalureHeight == MaxUp)
			{
				//属于整幅图像需要增加高度的部分情况，
				
				if (matchVecdy <= 0) {
					//该图像相对第一幅图像在下边,则lastcalureHeight一定是在下且为负数
					//并且b图像（待拼接图像）的重叠区域为总的图像高度减去b图像的高度，得到起始高度
					//这里要注意的是，图像的起始位置和待拼接位置起始不一定都在同一横坐标上，可以使A待拼接图片
					//在整幅图像的内部靠近边界地方，见图
					cimg_forXY(blendedImg, x, y) {
						if (x>= blendedImg._width - lastblendedImg._width
							&& x < blendedImg._width
							&& y < lastblendedImg._height) {
							//这里可以选择使用插值或者不插值，插值有渐变，但是可能会和黑色区域重叠，
							//插值的话整个区域的话面积改变
							//但是不插值会有点突兀，可以复制下面的x大于起始语句
							//本例子为重叠区域插值,暂时方便调试，后期将其功能分析时候可以对整个区域插值
							//为图像b还未重合的区域
							//abs(MaxLeft) + (lastcalureWeight)== 0相互抵消
							if (x >= abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec)
								&& x < abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y >= blendedImg._height - srcImgB._height
								&& y < blendedImg._height - srcImgB._height + (srcImgB._height - abs(matchVecdy))
								) 
							{    //混合
								//3-12最新发现，后期全部按照这个思路修改
								blendedImg(x, y, 0, 0) = (float)lastblendedImg((x - (blendedImg._width - lastblendedImg._width)), y, 0, 0)
									* (float)(x - (abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
									+ (float)srcImgB(x, y - (blendedImg._height - srcImgB._height), 0, 0)
									* (float)((abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width))- x) / (float)abs(leftmatchVec);
								blendedImg(x, y, 0, 1) = (float)lastblendedImg((x - (blendedImg._width - lastblendedImg._width)), y, 0, 1)
									* (float)(x - (abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
									+ (float)srcImgB(x, y - (blendedImg._height - srcImgB._height), 0, 1)
									* (float)((abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width)) - x) / (float)abs(leftmatchVec);
								blendedImg(x, y, 0, 2) = (float)lastblendedImg((x - (blendedImg._width - lastblendedImg._width)), y, 0, 2)
									* (float)(x - (abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
									+ (float)srcImgB(x, y - (blendedImg._height - srcImgB._height), 0, 2)
									* (float)((abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width)) - x) / (float)abs(leftmatchVec);
								
							}
							else if (x >= blendedImg._width - lastblendedImg._width
								&& x < (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width - leftmatchVec)
								&& y >= blendedImg._height - srcImgB._height
								&& y < blendedImg._height) {    //B独在部分
								blendedImg(x, y, 0, 0) = srcImgB(x, y - (blendedImg._height - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x, y - (blendedImg._height - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x, y - (blendedImg._height - srcImgB._height), 0, 2);
							}
							else {    //A或者左边拼接好的独在部分
								blendedImg(x, y, 0, 0) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y , 0, 0);
								blendedImg(x, y, 0, 1) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 1);
								blendedImg(x, y, 0, 2) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 2);
							}

						}
						//y部分应该是在左边图像的下边，右边
						else if (x >= (abs(MaxLeft) + (lastcalureWeight)) 
							&& x < blendedImg._width - lastblendedImg._width
							&& y >= blendedImg._height - srcImgB._height 
							&& y < blendedImg._height) {    //B独在部分
							blendedImg(x, y, 0, 0) = srcImgB(x, y - (blendedImg._height - srcImgB._height), 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x, y - (blendedImg._height - srcImgB._height), 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x, y - (blendedImg._height - srcImgB._height), 0, 2);
						}
						else {    //黑色部分
							blendedImg(x, y, 0, 0) = 0;
							blendedImg(x, y, 0, 1) = 0;
							blendedImg(x, y, 0, 2) = 0;
						}

						
					}
				}
				//3-11号修改
				else {
					//matchVec.dy > 0;左侧图片需要往上左移动
					//该图像相对第一幅图像在上边,则lastcalureHeight一定是在上且为正数
					cimg_forXY(blendedImg, x, y) {
						if (x >= blendedImg._width - lastblendedImg._width
							&& x < blendedImg._width 
							&& y >= blendedImg._height - lastblendedImg._height
							&& y < blendedImg._height
							) {
							//这里可以选择使用插值或者不插值，插值有渐变，但是可能会和黑色区域重叠，
							//插值的话整个区域的话面积改变
							//但是不插值会有点突兀，可以复制下面的x大于起始语句
							//不插值区间为
							if (x >= abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y>= abs(matchVecdy)
								&& y < srcImgB._height) {    //混合
								blendedImg(x, y, 0, 0) = (float)lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y - (blendedImg._height - lastblendedImg._height), 0, 0)
									* (float)( x - (abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
									+ (float)srcImgB(x, y, 0, 0)
									* (float)((abs(MaxLeft) + lastcalureWeight + srcImgB._width)- x) / (float)abs(leftmatchVec);
								
								blendedImg(x, y, 0, 1) = (float)lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y - (blendedImg._height - lastblendedImg._height), 0, 1)
									* (float)(x - (abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
									+ (float)srcImgB(x, y, 0, 1)
									* (float)((abs(MaxLeft) + lastcalureWeight + srcImgB._width) - x) / (float)abs(leftmatchVec);
								
								blendedImg(x, y, 0, 2) = (float)lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y - (blendedImg._height - lastblendedImg._height), 0, 2)
									* (float)(x - (abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
									+ (float)srcImgB(x, y, 0, 2)
									* (float)((abs(MaxLeft) + lastcalureWeight + srcImgB._width) - x) / (float)abs(leftmatchVec);
								
							}
							else if (x >= blendedImg._width - lastblendedImg._width
								&& x <= abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width)
								&& y < srcImgB._height)
							{	//B部分
								blendedImg(x, y, 0, 0) = srcImgB(x, y, 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x, y, 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x, y, 0, 2);
							}
							else {    //A独在部分
								blendedImg(x, y, 0, 0) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y - (blendedImg._height - lastblendedImg._height), 0, 0);
								blendedImg(x, y, 0, 1) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y - (blendedImg._height - lastblendedImg._height), 0, 1);
								blendedImg(x, y, 0, 2) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y - (blendedImg._height - lastblendedImg._height), 0, 2);
							}
						}
						else if (x >= (abs(MaxLeft) + (lastcalureWeight))
							&& x <= blendedImg._width - lastblendedImg._width
							&& y < srcImgB._height) {    //B独在部分
							blendedImg(x, y, 0, 0) = srcImgB(x, y, 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x, y, 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x, y, 0, 2);
						}
						else {    //黑色部分
							blendedImg(x, y, 0, 0) = 0;
							blendedImg(x, y, 0, 1) = 0;
							blendedImg(x, y, 0, 2) = 0;
						}
					}
				}
			}
			else
			{
				//属于整幅图像不需要增加高度的部分情况，按照3-11号重构代码
				//但是该图像左边又增加空间变化
				if (matchVecdy <= 0)
				{

					//待拼接的图片相对于匹配的图像是往下左移动
					//但整幅图像不需要扩展高度,不需要扩展宽度
					if (lastcalureHeight > 0)
					{
						//该图片相对第一幅图像在上边，重叠区域为上顶面减去上偏移开始到待拼接这幅图像结束
						cimg_forXY(blendedImg, x, y)
						{
							if (x >= blendedImg._width - lastblendedImg._width
								&& x < blendedImg._width
								&& y < lastblendedImg._height)
							{
								//这里可以选择使用插值或者不插值，插值有渐变，但是可能会和黑色区域重叠，
								//插值的话整个区域的话面积改变
								//但是不插值会有点突兀，可以复制下面的x大于起始语句

								//本例子为重叠区域插值,暂时方便调试，后期将其功能分析时候可以对整个区域插值
								//(srcImgB._width - leftmatchVec) 为图像b还未重合的区域
								if (x >= abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec)
									&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
									&& y >= abs(MaxUp - lastcalureHeight)
									&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height - abs(matchVecdy)
									)
								{
									blendedImg(x, y, 0, 0) = (float)lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 0)
										* (float)(x - (abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
										+ (float)srcImgB(x , y - abs(MaxUp - lastcalureHeight), 0, 0)
										* (float)((abs(MaxLeft) + lastcalureWeight + (srcImgB._width )) - x) / (float)abs(leftmatchVec);
									blendedImg(x, y, 0, 1) = (float)lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 1)
										* (float)(x - (abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
										+ (float)srcImgB(x, y - abs(MaxUp - lastcalureHeight), 0, 1)
										* (float)((abs(MaxLeft) + lastcalureWeight + (srcImgB._width)) - x) / (float)abs(leftmatchVec);
									blendedImg(x, y, 0, 2) = (float)lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 2)
										* (float)(x - (abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
										+ (float)srcImgB(x, y - abs(MaxUp - lastcalureHeight), 0, 2)
										* (float)((abs(MaxLeft) + lastcalureWeight + (srcImgB._width)) - x) / (float)abs(leftmatchVec);
								}
								//将b区域的值也附加进去，这部分的图像尚未进行插值操作
								else if (x >= blendedImg._width - lastblendedImg._width
									&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
									&& y >= abs(MaxUp - lastcalureHeight)
									&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B独在部分
									blendedImg(x, y, 0, 0) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 2);
								}
								else {    //A或者左边拼接好的独在部分
									blendedImg(x, y, 0, 0) = lastblendedImg(x - blendedImg._width - lastblendedImg._width, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x - blendedImg._width - lastblendedImg._width, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x - blendedImg._width - lastblendedImg._width, y, 0, 2);
								}
							}
							//y部分应该是在左边图像的下边，右边,基本这里不会运行，情况都包含在上边循环
							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&& x <= (abs(MaxLeft) + (lastcalureWeight)) + blendedImg._width - lastblendedImg._width
								&& y >= abs(MaxUp - lastcalureHeight)
								&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B独在部分
								blendedImg(x, y, 0, 0) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 2);
							}
							else {    //黑色部分
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}
					else
					{
						//该图片相对第一幅图像在下边，重叠区域为下顶面减去下偏移开始到两张图片重叠区域结束
						cimg_forXY(blendedImg, x, y)
						{
							if (x >= blendedImg._width - lastblendedImg._width
								&& x < blendedImg._width
								&& y < lastblendedImg._height)
							{
								//这里可以选择使用插值或者不插值，插值有渐变，但是可能会和黑色区域重叠，
								//插值的话整个区域的话面积改变
								//但是不插值会有点突兀，可以复制下面的x大于起始语句

								//本例子为重叠区域插值,暂时方便调试，后期将其功能分析时候可以对整个区域插值
								//(srcImgB._width - leftmatchVec) 为图像b还未重合的区域
								if (x >= abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec)
									&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
									&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
									&& y <= blendedImg._height - abs(MaxDown - lastcalureHeight) + (srcImgB._height - abs(matchVecdy))
									)
								{
									blendedImg(x, y, 0, 0) = (float)lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 0)
										* (float)(x - (abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
										+ (float)srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0)
										* (float)((abs(MaxLeft) + lastcalureWeight + (srcImgB._width)) - x) / (float)abs(leftmatchVec);
									blendedImg(x, y, 0, 1) = (float)lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 1)
										* (float)(x - (abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
										+ (float)srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1)
										* (float)((abs(MaxLeft) + lastcalureWeight + (srcImgB._width)) - x) / (float)abs(leftmatchVec);
									blendedImg(x, y, 0, 2) = (float)lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 2)
										* (float)(x - (abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
										+ (float)srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2)
										* (float)((abs(MaxLeft) + lastcalureWeight + (srcImgB._width)) - x) / (float)abs(leftmatchVec);
								}
								//将b区域的值也附加进去，这部分的图像尚未进行插值操作
								else if (x >= blendedImg._width - lastblendedImg._width
									&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
									&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
									&& y < blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B独在部分
									blendedImg(x, y, 0, 0) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x , y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x , y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
								}
								else {    //A或者左边拼接好的独在部分
									blendedImg(x, y, 0, 0) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 2);
								}


							}
							//y部分应该是在左边图像的下边，右边
							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&& x <= blendedImg._width - lastblendedImg._width
								&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
								&& y < blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B独在部分
								blendedImg(x, y, 0, 0) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
							}
							else {    //黑色部分
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}

				}
				else
				{
					//待拼接的图片相对于匹配的到图像是往上左移动
					//但整幅图像不需要扩展高度,不需要扩展宽度
					if (lastcalureHeight > 0)
					{
						//该图片相对第一幅图像在上边，重叠区域为上顶面减去上偏移开始到待拼接这幅图像结束
						cimg_forXY(blendedImg, x, y)
						{
							if (x > blendedImg._width - lastblendedImg._width
								&& x < blendedImg._width
								&& y < lastblendedImg._height)
							{
								//这里可以选择使用插值或者不插值，插值有渐变，但是可能会和黑色区域重叠，
								//插值的话整个区域的话面积改变
								//但是不插值会有点突兀，可以复制下面的x大于起始语句

								//本例子为重叠区域插值,暂时方便调试，后期将其功能分析时候可以对整个区域插值
								//(srcImgB._width - leftmatchVec) 为图像b还未重合的区域
								if (x >= abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec)
									&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
									&& y >= abs(MaxUp - lastcalureHeight) + abs(matchVecdy)
									&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height
									)
								{
									blendedImg(x, y, 0, 0) = (float)lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 0)
										* (float)(x - (abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
										+ (float)srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 0)
										* (float)((abs(MaxLeft) + lastcalureWeight + (srcImgB._width)) - x) / (float)abs(leftmatchVec);
									blendedImg(x, y, 0, 1) = (float)lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 1)
										* (float)(x - (abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
										+ (float)srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 1)
										* (float)((abs(MaxLeft) + lastcalureWeight + (srcImgB._width)) - x) / (float)abs(leftmatchVec);
									blendedImg(x, y, 0, 2) = (float)lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 2)
										* (float)(x - (abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
										+ (float)srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 2)
										* (float)((abs(MaxLeft) + lastcalureWeight + (srcImgB._width)) - x) / (float)abs(leftmatchVec);
								}
								//将b区域的值也附加进去，这部分的图像尚未进行插值操作
								else if (x >= blendedImg._width - lastblendedImg._width
									&& x <= (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width)
									&& y >= abs(MaxUp - lastcalureHeight) 
									&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B独在部分
									blendedImg(x, y, 0, 0) = srcImgB(x , y - (abs(MaxUp - lastcalureHeight)), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x , y - (abs(MaxUp - lastcalureHeight)), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x , y - (abs(MaxUp - lastcalureHeight)), 0, 2);
								}
								else {    //A或者左边拼接好的独在部分
									blendedImg(x, y, 0, 0) = lastblendedImg(x-(blendedImg._width - lastblendedImg._width), y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 2);
								}
							}
							//y部分应该是在左边图像的下边，右边,基本这里不会运行，情况都包含在上边循环
							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&& x <= blendedImg._width - lastblendedImg._width
								&& y >= abs(MaxUp - lastcalureHeight) 
								&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B独在部分
								blendedImg(x, y, 0, 0) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 2);
							}
							else {    //黑色部分
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}
					else
					{
						//该图片相对第一幅图像在下边，重叠区域为下顶面减去下偏移开始到两张图片重叠区域结束
						cimg_forXY(blendedImg, x, y)
						{
							if (x > blendedImg._width - lastblendedImg._width
								&& x < blendedImg._width
								&& y < lastblendedImg._height)
							{
								//这里可以选择使用插值或者不插值，插值有渐变，但是可能会和黑色区域重叠，
								//插值的话整个区域的话面积改变
								//但是不插值会有点突兀，可以复制下面的x大于起始语句

								//本例子为重叠区域插值,暂时方便调试，后期将其功能分析时候可以对整个区域插值
								//(srcImgB._width - leftmatchVec) 为图像b还未重合的区域
								if (x >= abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec)
									&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
									&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - (srcImgB._height-abs(matchVecdy))
									&& y <= blendedImg._height - abs(MaxDown - lastcalureHeight)
									)
								{
									blendedImg(x, y, 0, 0) = (float)lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 0)
										* (float)(x - (abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
										+ (float)srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - (srcImgB._height)), 0, 0)
										* (float)((abs(MaxLeft) + lastcalureWeight + (srcImgB._width)) - x) / (float)abs(leftmatchVec);
									blendedImg(x, y, 0, 1) = (float)lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 1)
										* (float)(x - (abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
										+ (float)srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - (srcImgB._height )), 0, 1)
										* (float)((abs(MaxLeft) + lastcalureWeight + (srcImgB._width)) - x) / (float)abs(leftmatchVec);
									blendedImg(x, y, 0, 2) = (float)lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 2)
										* (float)(x - (abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
										+ (float)srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - (srcImgB._height )), 0, 2)
										* (float)((abs(MaxLeft) + lastcalureWeight + (srcImgB._width)) - x) / (float)abs(leftmatchVec);
								}
								//将b区域的值也附加进去，这部分的图像尚未进行插值操作
								else if (x >= blendedImg._width - lastblendedImg._width
									&& x <= (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width)
									&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
									&& y < blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B独在部分
									blendedImg(x, y, 0, 0) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
								}
								else {    //A或者左边拼接好的独在部分
									blendedImg(x, y, 0, 0) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 2);
								}
							}
							
							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&& x <= blendedImg._width - lastblendedImg._width
								&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
								&& y < blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B独在部分
								blendedImg(x, y, 0, 0) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
							}
							else {    //黑色部分
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}

				}
			}
		}
		else
		{
			//该图像拼接的时候不需要从左边增加空间
			//待拼接图像在原来的宽度中
			//判断该图像是否引起高度的变化
			if (lastcalureHeight == MaxDown || lastcalureHeight == MaxUp) 
			{
				//属于整幅图像需要增加高度的部分情况，按照3-9号代码
				if (matchVecdy <= 0) {
					//该图像相对第一幅图像在下边,则lastcalureHeight一定是在下且为负数
					//并且b图像（待拼接图像）的重叠区域为总的图像高度减去b图像的高度，得到起始高度
					cimg_forXY(blendedImg, x, y) {
						if (x < lastblendedImg._width && y < lastblendedImg._height) {
							//这里可以选择使用插值或者不插值，插值有渐变，但是可能会和黑色区域重叠，
							//插值的话整个区域的话面积改变
							//但是不插值会有点突兀，可以复制下面的x大于起始语句
							//不插值区间为if (x >= abs(MaxLeft) + lastcalureWeight + srcImgB - leftmatchVec） && y >= blendedImg._height - srcImgB._height)
							//blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
							// * (float)(x - (abs(MaxLeft) + lastcalureWeight)) / (float)abs(matchVecdx)
							//	+ (float)srcImgB(x - (abs(MaxLeft) + lastcalureWeight), y - (blendedImg._height - srcImgB._height), 0, 0)
							//	* (float)(x - (abs(MaxLeft) + lastcalureWeight)) / (float)abs(matchVecdx);
							//本例子为重叠区域插值,暂时方便调试，后期将其功能分析时候可以对整个区域插值
							//(srcImgB._width - leftmatchVec) 为图像b还未重合的区域
							if (x >= abs(MaxLeft) + (lastcalureWeight) + (srcImgB._width - leftmatchVec) 
								&&x <= abs(MaxLeft) + (lastcalureWeight)+ srcImgB._width 
								&& y >= blendedImg._height - srcImgB._height) {    //混合
								blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
									* (float)((abs(MaxLeft) + lastcalureWeight + srcImgB._width) - x) / (float)abs(leftmatchVec)
									+ (float)srcImgB(x - (abs(MaxLeft) + lastcalureWeight), y - (blendedImg._height - srcImgB._height), 0, 0)
									* (float)(x - (abs(MaxLeft) + lastcalureWeight + (srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec);
								blendedImg(x, y, 0, 1) = (float)lastblendedImg(x, y, 0, 1)
									* (float)((abs(MaxLeft) + lastcalureWeight + srcImgB._width) - x) / (float)abs(leftmatchVec)
									+ (float)srcImgB(x - (abs(MaxLeft) + lastcalureWeight), y - (blendedImg._height - srcImgB._height), 0, 1)
									* (float)(x - (abs(MaxLeft) + lastcalureWeight + (srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec);
								blendedImg(x, y, 0, 2) = (float)lastblendedImg(x, y, 0, 2)
									* (float)((abs(MaxLeft) + lastcalureWeight + srcImgB._width) - x) / (float)abs(leftmatchVec)
									+ (float)srcImgB(x - (abs(MaxLeft) + lastcalureWeight), y - (blendedImg._height - srcImgB._height), 0, 2)
									* (float)(x - (abs(MaxLeft) + lastcalureWeight + (srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec);

							}
							//将b区域的值也附加进去，这部分的图像尚未进行插值操作
							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&&x<= (abs(MaxLeft) + (lastcalureWeight) + srcImgB._width)
								&& y >= blendedImg._height - srcImgB._height 
								&& y < blendedImg._height) {    //B独在部分
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 2);
							}
							else {    //A或者左边拼接好的独在部分
								blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
								blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
								blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
							}

						}
						//y部分应该是在左边图像的下边，右边
						else if (x >= (abs(MaxLeft) + (lastcalureWeight))
							&& x <= (abs(MaxLeft) + (lastcalureWeight)) + srcImgB._width
							&& y >= blendedImg._height - srcImgB._height && y < blendedImg._height) {    //B独在部分
							blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 2);
						}
						else {    //黑色部分
							blendedImg(x, y, 0, 0) = 0;
							blendedImg(x, y, 0, 1) = 0;
							blendedImg(x, y, 0, 2) = 0;
						}
					}
				}
				//3-11号修改
				else {
					//matchVec.dy > 0; 右侧图片需要往上左移动
					//该图像相对第一幅图像在上边,则lastcalureHeight一定是在上且为正数
					cimg_forXY(blendedImg, x, y) {
						if (x < lastblendedImg._width && y >= blendedImg._height - lastblendedImg._height) {
							//这里可以选择使用插值或者不插值，插值有渐变，但是可能会和黑色区域重叠，
							//插值的话整个区域的话面积改变
							//但是不插值会有点突兀，可以复制下面的x大于起始语句
							//不插值区间为
							if (x >= abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec) 
								&&x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y < srcImgB._height) {    //混合
								blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 0)
									* (float)(x - (abs(MaxLeft) + lastcalureWeight + (srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
									+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 0)
									* (float)((abs(MaxLeft) + lastcalureWeight + srcImgB._width) - x) / (float)abs(leftmatchVec);
								blendedImg(x, y, 0, 1) = (float)lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 1)
									* (float)(x - (abs(MaxLeft) + lastcalureWeight + (srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
									+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 1)
									* (float)((abs(MaxLeft) + lastcalureWeight + srcImgB._width) - x) / (float)abs(leftmatchVec);
								blendedImg(x, y, 0, 2) = (float)lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 2)
									* (float)(x - (abs(MaxLeft) + lastcalureWeight + (srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
									+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 2)
									* (float)((abs(MaxLeft) + lastcalureWeight + srcImgB._width) - x) / (float)abs(leftmatchVec);
							}
							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&& x <= abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width )
								&& y < srcImgB._height) 
							{
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 2);
							}
							else {    //A独在部分
								blendedImg(x, y, 0, 0) = lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 0);
								blendedImg(x, y, 0, 1) = lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 1);
								blendedImg(x, y, 0, 2) = lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 2);
							}
						}
						else if (x >= (abs(MaxLeft) + (lastcalureWeight)) 
							&& x <= (abs(MaxLeft) + (lastcalureWeight)) + srcImgB._width
							&& y < srcImgB._height) {    //B独在部分
							blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 2);
						}
						else {    //黑色部分
							blendedImg(x, y, 0, 0) = 0;
							blendedImg(x, y, 0, 1) = 0;
							blendedImg(x, y, 0, 2) = 0;
						}
					}
				}
			}
			else 
			{
				//属于整幅图像不需要增加高度的部分情况，按照3-11号重构代码

				if (matchVecdy <= 0) 
				{
				
					//待拼接的图片相对于匹配的图像是往下左移动
					//但整幅图像不需要扩展高度,不需要扩展宽度
					if (lastcalureHeight > 0)
					{
						//该图片相对第一幅图像在上边，重叠区域为上顶面减去上偏移开始到待拼接这幅图像结束
						cimg_forXY(blendedImg, x, y)
						{
							if (x < lastblendedImg._width && y < lastblendedImg._height) 
							{
								//这里可以选择使用插值或者不插值，插值有渐变，但是可能会和黑色区域重叠，
								//插值的话整个区域的话面积改变
								//但是不插值会有点突兀，可以复制下面的x大于起始语句

								//本例子为重叠区域插值,暂时方便调试，后期将其功能分析时候可以对整个区域插值
								//(srcImgB._width - leftmatchVec) 为图像b还未重合的区域
								if (x >= abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec)
									&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
									&& y >= abs(MaxUp - lastcalureHeight)
									&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height - abs(matchVecdy)
									) 
								{
									blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
										* (float)(x - (abs(MaxLeft) + lastcalureWeight + (srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
										+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 0)
										* (float)(abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width) - x) / (float)abs(leftmatchVec);
									blendedImg(x, y, 0, 1) = (float)lastblendedImg(x, y, 0, 1)
										* (float)(x - (abs(MaxLeft) + lastcalureWeight + (srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
										+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 1)
										* (float)(abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width) - x) / (float)abs(leftmatchVec);
									blendedImg(x, y, 0, 2) = (float)lastblendedImg(x, y, 0, 2)
										* (float)(x - (abs(MaxLeft) + lastcalureWeight + (srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
										+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 2)
										* (float)(abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width) - x) / (float)abs(leftmatchVec);
								}
								//将b区域的值也附加进去，这部分的图像尚未进行插值操作
								else if (x >= (abs(MaxLeft) + (lastcalureWeight)) 
									&& x <= (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width )
									&& y >= abs(MaxUp - lastcalureHeight) 
									&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B独在部分
									blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight) ), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight) ), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight) ), 0, 2);
								}
								else {    //A或者左边拼接好的独在部分
									blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
								}
							}
							//y部分应该是在左边图像的下边，右边,基本这里不会运行，情况都包含在上边循环
							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&& x <= (abs(MaxLeft) + (lastcalureWeight)) + srcImgB._width
								&& y >= abs(MaxUp - lastcalureHeight) 
								&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B独在部分
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 2);
							}
							else {    //黑色部分
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}
					else 
					{
						//该图片相对第一幅图像在下边，重叠区域为下顶面减去下偏移开始到两张图片重叠区域结束
						cimg_forXY(blendedImg, x, y)
						{
							if (x < lastblendedImg._width && y < lastblendedImg._height)
							{
								//这里可以选择使用插值或者不插值，插值有渐变，但是可能会和黑色区域重叠，
								//插值的话整个区域的话面积改变
								//但是不插值会有点突兀，可以复制下面的x大于起始语句

								//本例子为重叠区域插值,暂时方便调试，后期将其功能分析时候可以对整个区域插值
								//(srcImgB._width - leftmatchVec) 为图像b还未重合的区域
								if (x >= abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec)
									&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
									&& y >= blendedImg._height - abs(MaxDown -lastcalureHeight) - srcImgB._height
									&& y <= blendedImg._height - abs(MaxDown - lastcalureHeight) + (srcImgB._height - abs(matchVecdy))
									) 
									{
									blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
										* (float)(x - (abs(MaxLeft) + lastcalureWeight + (srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
										+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight)-srcImgB._height), 0, 0)
										* (float)(abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width) - x) / (float)abs(leftmatchVec);
									blendedImg(x, y, 0, 1) = (float)lastblendedImg(x, y, 0, 1)
										* (float)(x - (abs(MaxLeft) + lastcalureWeight + (srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
										+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0,1)
										* (float)(abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width) - x) / (float)abs(leftmatchVec);
									blendedImg(x, y, 0, 2) = (float)lastblendedImg(x, y, 0, 2)
										* (float)(x - (abs(MaxLeft) + lastcalureWeight + (srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
										+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2)
										* (float)(abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width) - x) / (float)abs(leftmatchVec);
									}
								//将b区域的值也附加进去，这部分的图像尚未进行插值操作
								else if (x >= (abs(MaxLeft) + (lastcalureWeight)) 
									&& x <= (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width )
									&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
									&& y < blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B独在部分
									blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
								}
								else {    //A或者左边拼接好的独在部分
									blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
								}


							}
							//y部分应该是在左边图像的下边，右边
							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&& x <= (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width)
								&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
								&& y < blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B独在部分
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
							}
							else {    //黑色部分
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}

					
				}
				else 
				{
					//待拼接的图片相对于匹配的到图像是往上左移动
					//但整幅图像不需要扩展高度,不需要扩展宽度
					if (lastcalureHeight > 0)
					{
						//该图片相对第一幅图像在上边，重叠区域为上顶面减去上偏移开始到待拼接这幅图像结束
						cimg_forXY(blendedImg, x, y)
						{
							if (x < lastblendedImg._width && y < lastblendedImg._height)
							{
								//这里可以选择使用插值或者不插值，插值有渐变，但是可能会和黑色区域重叠，
								//插值的话整个区域的话面积改变
								//但是不插值会有点突兀，可以复制下面的x大于起始语句

								//本例子为重叠区域插值,暂时方便调试，后期将其功能分析时候可以对整个区域插值
								//(srcImgB._width - leftmatchVec) 为图像b还未重合的区域
								if (x >= abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec)
									&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
									&& y >= abs(MaxUp - lastcalureHeight) + abs(matchVecdy)
									&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height
									)
								{
									blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
										* (float)(x - (abs(MaxLeft) + lastcalureWeight + (srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
										+ (float)srcImgB(x - (abs(MaxLeft) + lastcalureWeight), y - abs(MaxUp - lastcalureHeight), 0, 0)
										* (float)(abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width) - x) / (float)abs(leftmatchVec);
									blendedImg(x, y, 0, 1) = (float)lastblendedImg(x, y, 0,1)
										* (float)(x - (abs(MaxLeft) + lastcalureWeight + (srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
										+ (float)srcImgB(x - (abs(MaxLeft) + lastcalureWeight), y - abs(MaxUp - lastcalureHeight), 0, 1)
										* (float)(abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width) - x) / (float)abs(leftmatchVec);
									blendedImg(x, y, 0, 2) = (float)lastblendedImg(x, y, 0, 2)
										* (float)(x - (abs(MaxLeft) + lastcalureWeight + (srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
										+ (float)srcImgB(x - (abs(MaxLeft) + lastcalureWeight), y - abs(MaxUp - lastcalureHeight), 0,2)
										* (float)(abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width) - x) / (float)abs(leftmatchVec);
								}
								//将b区域的值也附加进去，这部分的图像尚未进行插值操作
								else if (x >= (abs(MaxLeft) + (lastcalureWeight)) 
									&& x <= (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width )
									&& y >= abs(MaxUp - lastcalureHeight) 
									&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B独在部分
									blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 2);
								}
								else {    //A或者左边拼接好的独在部分
									blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
								}
							}
							//y部分应该是在左边图像的下边，右边,基本这里不会运行，情况都包含在上边循环
							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&& x <= (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width)
								&& y >= abs(MaxUp - lastcalureHeight)
								&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B独在部分
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 2);
							}
							else {    //黑色部分
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}
					else
					{
						//该图片相对第一幅图像在下边，重叠区域为下顶面减去下偏移开始到两张图片重叠区域结束
						cimg_forXY(blendedImg, x, y)
						{
							if (x < lastblendedImg._width && y < lastblendedImg._height)
							{
								//这里可以选择使用插值或者不插值，插值有渐变，但是可能会和黑色区域重叠，
								//插值的话整个区域的话面积改变
								//但是不插值会有点突兀，可以复制下面的x大于起始语句

								//本例子为重叠区域插值,暂时方便调试，后期将其功能分析时候可以对整个区域插值
								//(srcImgB._width - leftmatchVec) 为图像b还未重合的区域
								if (x >= abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec)
									&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
									&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - abs(matchVecdy)
									&& y <= blendedImg._height - abs(MaxDown - lastcalureHeight) 
									)
								{
									blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
										* (float)(abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width ) - x) / (float)abs(leftmatchVec)
										+ (float)srcImgB(x - (abs(MaxLeft) + lastcalureWeight ), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0)
										* (float)(x - (abs(MaxLeft) + lastcalureWeight + (srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec);
									blendedImg(x, y, 0, 1) = (float)lastblendedImg(x, y, 0, 1)
										* (float)(abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width) - x) / (float)abs(leftmatchVec)
										+ (float)srcImgB(x - (abs(MaxLeft) + lastcalureWeight), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0,1)
										* (float)(x - (abs(MaxLeft) + lastcalureWeight + (srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec);
									blendedImg(x, y, 0, 2) = (float)lastblendedImg(x, y, 0, 2)
										* (float)(abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width) - x) / (float)abs(leftmatchVec)
										+ (float)srcImgB(x - (abs(MaxLeft) + lastcalureWeight), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2)
										* (float)(x - (abs(MaxLeft) + lastcalureWeight + (srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec);
								}
								//将b区域的值也附加进去，这部分的图像尚未进行插值操作
								else if (x >= (abs(MaxLeft) + (lastcalureWeight)) 
									&& x <= (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width )
									&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) -  srcImgB._height
									&& y < blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B独在部分
									blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
								}
								else {    //A或者左边拼接好的独在部分
									blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
								}


							}
							//y部分应该是在左边图像的下边，右边
							//一般不会再使用
							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&& x <= (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width)
								&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
								&& y < blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B独在部分
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
							}
							else {    //黑色部分
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}
					
				}
			}
		}
		
	}
	else
	{
		//需要拼接的图像在右边
		//判断当前图像相对于上一个图像是向上移动还是向下移动
		//小于0，向下移动
		//大于0，向上移动
		//公式：dy1 = img2.y - img1.y
		//dy2 = img3.y - img2.y;
		//img3相对于img1:
		//dy = dy1+dy2 = img3.y - img1.y;
		//首先先计算偏移量，看是相对与第一个图像是向上还是向下

		lastcalureHeight += matchVecdy;
		if (lastcalureHeight <= 0)
		{
			//负数比较谁最小
			//判断相对于最下的偏移量，如果小于扩增后的最大向下行，就无需要增大下行数
			//如果大于的话，就更新下的行数，同时更新最下的值
			MaxDown = MaxDown < lastcalureHeight ? MaxDown : lastcalureHeight;

		}
		else//向上移动 
		{
			//正数比较谁最大
			//判断相对于最上的偏移量，如果小于扩增后的最大向上行，就无需要增大上行数
				//如果大于的话，就更新上的行数，同时更新最上的值
			MaxUp = MaxUp > lastcalureHeight ? MaxUp : lastcalureHeight;
		}

		//计算出当前的图像相对于第一个图像横向偏移了多少
		lastcalureWeight = lastcalureWeight + srcImgB._width - matchVecdx;
		if (lastcalureWeight > 0)
			MaxRight = MaxRight > lastcalureWeight ? MaxRight : lastcalureWeight;
		else
			MaxLeft = MaxLeft < lastcalureWeight ? MaxLeft : lastcalureWeight;

		//待拼接的图片在图像的右边
		//-----------------------合并的窗口大小位置-----------------------------------------
		//dx为两幅图像的特征点之间的横的差值
		//dy为两幅图像的特征点之间的竖的差值
		//判断是否需要增加画布的长度
		//宽度上是原图像，加上上增加值同时加上下增加值
		//若用上一个图像判断是否扩增行的高度
		//增加初始化图像的大小
		//下面的原理为所有宽度叠加取代原来的传入图像的宽度，
		//减少在拼接左边的图像时候出现需要库充左边图像的情况
		//blendedImg = CImg<int>(lastblendedImg._width + srcImgB._width - matchVecdx,
		//	srcImgB._height + abs(MaxDown) + abs(MaxUp), 1, 3, 0);

		//
		blendedImg = CImg<int>(oriImageWidth + abs(MaxLeft) + abs(MaxRight),
			srcImgB._height + abs(MaxDown) + abs(MaxUp), 1, 3, 0);


		//判断新加入的图像是相对于上一个图像上移动还是下移动
		//判断x属于左边拼接好或者是第一个图像的区域
		//如果这个图像的点恰好在两个图像的重叠之处，需要做插值处理
		//计算重叠区域的插值的时候，将重合区域的变量同时处于重叠区域的宽度
		//此处需要重新讨论
		
		if (lastcalureWeight == MaxRight)
		{
			//该图像拼接的时候需要从右边增加空间
			//并且待拼接图像在右边
			//判断该图像是否引起高度的变化
			if (lastcalureHeight == MaxDown || lastcalureHeight == MaxUp)
			{
				//属于整幅图像需要增加高度的部分情况，按照3-9号代码
				if (matchVecdy <= 0) {
					//该图像相对第一幅图像在下边,则lastcalureHeight一定是在下且为负数
					//并且b图像（待拼接图像）的重叠区域为总的图像高度减去b图像的高度，得到起始高度
					cimg_forXY(blendedImg, x, y) {
					if (x < lastblendedImg._width && y < lastblendedImg._height) {
						if (x >= abs(MaxLeft) + (lastcalureWeight)
							&& x <= abs(MaxLeft) + (lastcalureWeight)+matchVecdx
							&& y >= blendedImg._height-srcImgB._height
							&& y <= blendedImg._height - srcImgB._height + (srcImgB._height -abs(matchVecdy))
							) {    //混合
							blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
								* (float)(abs(MaxLeft) + (lastcalureWeight)+matchVecdx - x) / (float)abs(matchVecdx)
								+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 0)
								* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
							blendedImg(x, y, 0, 1) = (float)lastblendedImg(x, y, 0, 1)
								* (float)(abs(MaxLeft) + (lastcalureWeight)+matchVecdx - x) / (float)abs(matchVecdx)
								+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 1)
								* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
							blendedImg(x, y, 0, 2) = (float)lastblendedImg(x, y, 0, 2)
								* (float)(abs(MaxLeft) + (lastcalureWeight)+matchVecdx - x) / (float)abs(matchVecdx)
								+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 2)
								* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
						}
						//y部分应该是在左边图像的下边，右边
						else if (x >= abs(MaxLeft) + (lastcalureWeight)
							&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
							&& y >= blendedImg._height - srcImgB._height && y < blendedImg._height) {    //B独在部分
							blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 2);
						}
						else {    //A或者左边拼接好的独在部分
							blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
							blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
							blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
						}

					}
					//y部分应该是在左边图像的下边，右边
					else if (x >= abs(MaxLeft) + (lastcalureWeight)
						&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
						&& y >= blendedImg._height - srcImgB._height && y < blendedImg._height) {    //B独在部分
						blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 0);
						blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 1);
						blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 2);
					}
					else {    //黑色部分
						blendedImg(x, y, 0, 0) = 0;
						blendedImg(x, y, 0, 1) = 0;
						blendedImg(x, y, 0, 2) = 0;
					}
					}
				}
				//3-11号修改
				else {    
					//matchVec.dy > 0; 右侧图片需要往上左移动
					//该图像相对第一幅图像在上边,则lastcalureHeight一定是在上且为正数
					cimg_forXY(blendedImg, x, y) {
						if (x < lastblendedImg._width 
							&& y >= blendedImg._height - lastblendedImg._height) {
							if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+matchVecdx
								&& y > srcImgB._height - abs(matchVecdy)
								&& y < srcImgB._height) {    //混合
								blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 0)
									* (float)(abs(MaxLeft) + (lastcalureWeight)+matchVecdx - x) / (float)abs(matchVecdx)
									+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 0)
									* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
								blendedImg(x, y, 0, 1) = (float)lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 1)
									* (float)(abs(MaxLeft) + (lastcalureWeight)+matchVecdx - x) / (float)abs(matchVecdx)
									+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 1)
									* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
								blendedImg(x, y, 0, 2) = (float)lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 2)
									* (float)(abs(MaxLeft) + (lastcalureWeight)+matchVecdx - x) / (float)abs(matchVecdx)
									+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 2)
									* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
							}
							else if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y < srcImgB._height) {    //B独在部分
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 2);
							}
							else {    //A独在部分
								blendedImg(x, y, 0, 0) = lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 0);
								blendedImg(x, y, 0, 1) = lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 1);
								blendedImg(x, y, 0, 2) = lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 2);
							}
						}
						else if (x >= abs(MaxLeft) + (lastcalureWeight)
							&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
							&& y < srcImgB._height) {    //B独在部分
							blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 2);
						}
						else {    //黑色部分
							blendedImg(x, y, 0, 0) = 0;
							blendedImg(x, y, 0, 1) = 0;
							blendedImg(x, y, 0, 2) = 0;
						}
					}
				}
			}
			else
			{
				//属于不需要增加高度的部分，按照3-11号代码另行处理
				if (matchVecdy <= 0) 
				{
					//待拼接的图片相对于匹配的图像是往下右移动
					//但整幅图像不需要扩展高度
					if (lastcalureHeight > 0)
					{
						//该图片相对第一幅图像在上边，重叠区域为上顶面减去上偏移开始到待拼接这幅图像结束
						cimg_forXY(blendedImg, x, y) {
							if (x < lastblendedImg._width && y < lastblendedImg._height)
							{
								//也可更改为这样子，不将剩余的下边界做插值，但是可能会消失，效果不好
								//if (x >= (lastblendedImg._width - matchVecdx) && y >= abs(MaxUp - lastcalureHeight)
								//	&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height - abs(matchVecdy))
								if (x >= abs(MaxLeft) + (lastcalureWeight)
									&& x <= abs(MaxLeft) + (lastcalureWeight)+matchVecdx
									&& y >= abs(MaxUp - lastcalureHeight)
									 && y <= abs(MaxUp - lastcalureHeight) + srcImgB._height - abs(matchVecdy)) {    //混合
									blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
										* (float)(abs(MaxLeft) + (lastcalureWeight)+matchVecdx - x) / (float)abs(matchVecdx)
										+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 0)
										* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
									blendedImg(x, y, 0, 1) = (float)lastblendedImg(x, y, 0, 1)
										* (float)(abs(MaxLeft) + (lastcalureWeight)+matchVecdx - x) / (float)abs(matchVecdx)
										+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 1)
										* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
									blendedImg(x, y, 0, 2) = (float)lastblendedImg(x, y, 0, 2)
										* (float)(abs(MaxLeft) + (lastcalureWeight)+matchVecdx - x) / (float)abs(matchVecdx)
										+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 2)
										* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);

								}
								else if (x >= abs(MaxLeft) + (lastcalureWeight)
									&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
									&& y >= abs(MaxUp - lastcalureHeight)
									&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B独在部分
									blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 2);
								}
								else {    //A或者左边拼接好的独在部分
									blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
								}
							}
							else if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y >= abs(MaxUp - lastcalureHeight)
								&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B独在部分
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 2);
							}
							else {    //黑色部分
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}

					else
					{
						//该图片相对第一幅图像在下边，重叠区域为下顶边减去下偏移，得出第二幅图像的结尾像素距离整幅图像的距离
						//lastblendedImg._height - abs(MaxDown - lastcalureHeight)为第二幅图像结束拼接时候的高度
						cimg_forXY(blendedImg, x, y) {
						if (x < lastblendedImg._width && y < lastblendedImg._height) 
						{
							//也可更改为这样子，不将剩余的下边界做插值，但是可能会消失，效果不好
							//if (x >= (lastblendedImg._width - matchVecdx) && y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight)
							//-srcImgB._height && y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - matchVecdy)
							if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+matchVecdx
								&& y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight)- srcImgB._height 
								&& y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - abs(matchVecdy))
							{    //混合
							blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
								* (float)(abs(MaxLeft) + (lastcalureWeight)+matchVecdx - x) / (float)abs(matchVecdx)
								+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0)
								* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
							blendedImg(x, y, 0, 1) = (float)lastblendedImg(x, y, 0, 1)
								* (float)(abs(MaxLeft) + (lastcalureWeight)+matchVecdx - x) / (float)abs(matchVecdx)
								+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1)
								* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
							blendedImg(x, y, 0, 2) = (float)lastblendedImg(x, y, 0, 2)
								* (float)(abs(MaxLeft) + (lastcalureWeight)+matchVecdx - x) / (float)abs(matchVecdx)
								+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2)
								* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
						}
						else if (x >= abs(MaxLeft) + (lastcalureWeight)
							&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
							&& y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight)- srcImgB._height 
							&& y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight))
						{    //B独在部分
							blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
						}
						else {    //A或者左边拼接好的独在部分
							blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
							blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
							blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
						}
						}
						else if (x >= abs(MaxLeft) + (lastcalureWeight)
							&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
							&& y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
							&& y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B独在部分
							blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
						}
						else {    //黑色部分
							blendedImg(x, y, 0, 0) = 0;
							blendedImg(x, y, 0, 1) = 0;
							blendedImg(x, y, 0, 2) = 0;
						}
						}
					}
					
				}
				else 
				{
					//待拼接的图片相对于匹配的图像是往上右移动
					//但整幅图像不需要扩展高度

					if (lastcalureHeight > 0)
					{
						//该图片相对第一幅图像在上边，重叠区域为上顶面减去上偏移开始到待拼接这幅图像结束
						cimg_forXY(blendedImg, x, y) {
							if (x < lastblendedImg._width && y < lastblendedImg._height)
							{
								//也可更改为这样子，不将剩余的下边界做插值，但是可能会消失，效果不好
								//if (x >= (lastblendedImg._width - matchVecdx) && y >= abs(MaxUp - lastcalureHeight)
								//	&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height - abs(matchVecdy))
								if (x >= abs(MaxLeft) + (lastcalureWeight)
									&& x <= abs(MaxLeft) + (lastcalureWeight)+matchVecdx
									&& y >= abs(MaxUp - lastcalureHeight) + abs(matchVecdy)
									&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //混合
									blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
										* (float)((abs(MaxLeft) + (lastcalureWeight)+matchVecdx) - x) / (float)abs(matchVecdx)
										+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 0)
										* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
									blendedImg(x, y, 0, 1) = (float)lastblendedImg(x, y, 0, 1)
										* (float)((abs(MaxLeft) + (lastcalureWeight)+matchVecdx) - x) / (float)abs(matchVecdx)
										+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 1)
										* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
									blendedImg(x, y, 0, 2) = (float)lastblendedImg(x, y, 0, 2)
										* (float)((abs(MaxLeft) + (lastcalureWeight)+matchVecdx) - x) / (float)abs(matchVecdx)
										+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 2)
										* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
								}
								else if (x >= abs(MaxLeft) + (lastcalureWeight)
									&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
									&& y >= abs(MaxUp - lastcalureHeight)
									&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B独在部分
									blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 2);
								}
								else {    //A或者左边拼接好的独在部分
									blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
								}
							}
							else if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y >= abs(MaxUp - lastcalureHeight)
								&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B独在部分
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 2);
							}
							else {    //黑色部分
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}

					else
					{
						//该图片相对第一幅图像在下边，重叠区域为下顶边减去下偏移，得出第二幅图像的结尾像素距离整幅图像的距离
						//lastblendedImg._height - abs(MaxDown - lastcalureHeight)为第二幅图像结束拼接时候的高度
						cimg_forXY(blendedImg, x, y) {
							if (x < lastblendedImg._width && y < lastblendedImg._height)
							{
								//也可更改为这样子，不将剩余的下边界做插值，但是可能会消失，效果不好
								//if (x >= (lastblendedImg._width - matchVecdx) && y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight)
								//-srcImgB._height && y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - matchVecdy)
								if (x >= abs(MaxLeft) + (lastcalureWeight)
									&& x <= abs(MaxLeft) + (lastcalureWeight)+matchVecdx 
									&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight)- (srcImgB._height - matchVecdy)
									&& y <= blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //混合
									blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
										* (float)(abs(MaxLeft) + (lastcalureWeight)+matchVecdx - x) / (float)abs(matchVecdx)
										+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0)
										* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
									blendedImg(x, y, 0, 1) = (float)lastblendedImg(x, y, 0, 1)
										* (float)(abs(MaxLeft) + (lastcalureWeight)+matchVecdx - x) / (float)abs(matchVecdx)
										+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1)
										* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
									blendedImg(x, y, 0, 2) = (float)lastblendedImg(x, y, 0, 2)
										* (float)(abs(MaxLeft) + (lastcalureWeight)+matchVecdx - x) / (float)abs(matchVecdx)
										+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2)
										* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
								}
								else if (x >= abs(MaxLeft) + (lastcalureWeight)
									&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
									&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
									&& y <= blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B独在部分
									blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
								}
								else {    //A或者左边拼接好的独在部分
									blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
								}
							}
							else if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
								&& y <= blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B独在部分
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
							}
							else {    //黑色部分
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}

				}

			}
		}
		else 
		{
			//该图像拼接的时候不需要从右边新增加空间
			//待拼接图像在原来的宽度中
			//判断该图像是否引起高度的变化
			if (lastcalureHeight == MaxDown || lastcalureHeight == MaxUp)
		{
			//属于整幅图像需要增加高度的部分情况，按照3-14号代码
			if (matchVecdy <= 0) {
				//该图像相对第一幅图像在下边,则lastcalureHeight一定是在下且为负数
				//并且b图像（待拼接图像）的重叠区域为总的图像高度减去b图像的高度，得到起始高度
				cimg_forXY(blendedImg, x, y) {
					if (x < lastblendedImg._width && y < lastblendedImg._height) {
						if (x >= abs(MaxLeft) + (lastcalureWeight)
							&& x <= abs(MaxLeft) + (lastcalureWeight) + matchVecdx
							&& y >= blendedImg._height - srcImgB._height
							&& y <= blendedImg._height - srcImgB._height + abs(matchVecdy)
							) {    //混合
							blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
								* (float)(abs(MaxLeft) + (lastcalureWeight)+matchVecdx - x) / (float)abs(matchVecdx)
								+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 0)
								* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
							blendedImg(x, y, 0, 1) = (float)lastblendedImg(x, y, 0, 1)
								* (float)(abs(MaxLeft) + (lastcalureWeight)+matchVecdx - x) / (float)abs(matchVecdx)
								+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 1)
								* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
							blendedImg(x, y, 0, 2) = (float)lastblendedImg(x, y, 0, 2)
								* (float)(abs(MaxLeft) + (lastcalureWeight)+matchVecdx - x) / (float)abs(matchVecdx)
								+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 2)
								* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);

						}
						else if (x >= abs(MaxLeft) + (lastcalureWeight)
							&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
							&& y >= blendedImg._height - srcImgB._height
							&& y <= blendedImg._height 
							) 
						{
							//B图像在A图像中的部分不需要计算插值的部分
							blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 2);
						}
						else {    //A或者左边拼接好的独在部分
							blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
							blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
							blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
						}

					}
					//y部分应该是在左边图像的下边，右边
					else if (x >= abs(MaxLeft) + (lastcalureWeight)
						&& x<= abs(MaxLeft) + (lastcalureWeight) + srcImgB._width
						&& y >= blendedImg._height - srcImgB._height
						&& y < blendedImg._height) {    //B独在部分
						blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 0);
						blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 1);
						blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 2);
					}
					else {    //黑色部分
						blendedImg(x, y, 0, 0) = 0;
						blendedImg(x, y, 0, 1) = 0;
						blendedImg(x, y, 0, 2) = 0;
					}
				}
			}
			//3-11号修改
			else {
				//matchVec.dy > 0; 右侧图片需要往上右移动
				//该图像相对第一幅图像在上边,则lastcalureHeight一定是在下且为正数
				cimg_forXY(blendedImg, x, y) {
					if (x < lastblendedImg._width 
						&& y >= blendedImg._height - lastblendedImg._height
						&& y < blendedImg._height) 
					{
						if (x >= abs(MaxLeft) + (lastcalureWeight)
							&& x <= abs(MaxLeft) + (lastcalureWeight)+matchVecdx
							&& y > matchVecdy
							&& y < srcImgB._height
							) 
						{    //混合
							blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 0)
								* (float)(abs(MaxLeft) + (lastcalureWeight) + matchVecdx - x) / (float)abs(matchVecdx)
								+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 0)
								* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
							blendedImg(x, y, 0, 1) = (float)lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 1)
								* (float)(abs(MaxLeft) + (lastcalureWeight)+matchVecdx - x) / (float)abs(matchVecdx)
								+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 1)
								* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
							blendedImg(x, y, 0, 2) = (float)lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 2)
								* (float)(abs(MaxLeft) + (lastcalureWeight)+matchVecdx - x) / (float)abs(matchVecdx)
								+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 2)
								* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
						}
						else if (x >= abs(MaxLeft) + (lastcalureWeight)
							&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
							&& y >= blendedImg._height - lastblendedImg._height
							&& y <=srcImgB._height
							)
						{
							//B图像在A图像中的部分不需要计算插值的部分
							blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y , 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y , 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y , 0, 2);
						}

						else {    //A独在部分
							blendedImg(x, y, 0, 0) = lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 0);
							blendedImg(x, y, 0, 1) = lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 1);
							blendedImg(x, y, 0, 2) = lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 2);
						}
					}
					else if (
						x >= abs(MaxLeft) + (lastcalureWeight)
						&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
						&& y >= blendedImg._height - lastblendedImg._height
						&& y <= srcImgB._height
						) {    //B独在部分
						blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 0);
						blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 1);
						blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 2);
					}
					else {    //黑色部分
						blendedImg(x, y, 0, 0) = 0;
						blendedImg(x, y, 0, 1) = 0;
						blendedImg(x, y, 0, 2) = 0;
					}
				}
			}
		}
		else
		{
			//属于不需要增加高度的部分，按照3-14号代码另行处理
			if (matchVecdy <= 0)
			{
				//待拼接的图片相对于匹配的图像是往下右移动
				//但整幅图像不需要扩展高度
				if (lastcalureHeight > 0)
				{
					//该图片相对第一幅图像在上边，重叠区域为上顶面减去上偏移开始到待拼接这幅图像结束
					cimg_forXY(blendedImg, x, y) {
						if (x < lastblendedImg._width && y < lastblendedImg._height)
						{
							//也可更改为这样子，不将剩余的下边界做插值，但是可能会消失，效果不好
							//if (x >= (lastblendedImg._width - matchVecdx) && y >= abs(MaxUp - lastcalureHeight)
							//	&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height - abs(matchVecdy))
							if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+matchVecdx
								&& y >= abs(MaxUp - lastcalureHeight)
								&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height - abs(matchVecdy)) {    //混合
								blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
									* (float)(abs(MaxLeft) + (lastcalureWeight)+matchVecdx - x) / (float)abs(matchVecdx)
									+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 0)
									* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
								blendedImg(x, y, 0, 1) = (float)lastblendedImg(x, y, 0, 1)
									* (float)(abs(MaxLeft) + (lastcalureWeight)+matchVecdx - x) / (float)abs(matchVecdx)
									+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 1)
									* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
								blendedImg(x, y, 0, 2) = (float)lastblendedImg(x, y, 0, 2)
									* (float)(abs(MaxLeft) + (lastcalureWeight)+matchVecdx - x) / (float)abs(matchVecdx)
									+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 2)
									* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);

							}
							else if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y >= abs(MaxUp - lastcalureHeight)
								&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height
								) 
							{
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 2);
							}
							else {    //A或者左边拼接好的独在部分
								blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
								blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
								blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
							}
						}
						else if ((x >= abs(MaxLeft) + (lastcalureWeight)
							&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
							&& y >= abs(MaxUp - lastcalureHeight)
							&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height)) {    //B独在部分
							blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 2);
						}
						else {    //黑色部分
							blendedImg(x, y, 0, 0) = 0;
							blendedImg(x, y, 0, 1) = 0;
							blendedImg(x, y, 0, 2) = 0;
						}
					}
				}

				else
				{
					//该图片相对第一幅图像在下边，重叠区域为下顶边减去下偏移，得出第二幅图像的结尾像素距离整幅图像的距离
					//lastblendedImg._height - abs(MaxDown - lastcalureHeight)为第二幅图像结束拼接时候的高度
					cimg_forXY(blendedImg, x, y) {
						if (x < lastblendedImg._width && y < lastblendedImg._height)
						{
							//也可更改为这样子，不将剩余的下边界做插值，但是可能会消失，效果不好
							//if (x >= (lastblendedImg._width - matchVecdx) && y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight)
							//-srcImgB._height && y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - matchVecdy)
							if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+matchVecdx
								&& y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight)- srcImgB._height 
								&& y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - abs(matchVecdy))
							{    //混合
								blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
									* (float)((abs(MaxLeft) + (lastcalureWeight)+matchVecdx) - x) / (float)abs(matchVecdx)
									+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0)
									* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
								blendedImg(x, y, 0, 1) = (float)lastblendedImg(x, y, 0, 1)
									* (float)((abs(MaxLeft) + (lastcalureWeight)+matchVecdx) - x) / (float)abs(matchVecdx)
									+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1)
									* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
								blendedImg(x, y, 0, 2) = (float)lastblendedImg(x, y, 0, 2)
									* (float)((abs(MaxLeft) + (lastcalureWeight)+matchVecdx) - x) / (float)abs(matchVecdx)
									+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2)
									* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);

							}
							else if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
								&& y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight)
								)
							{
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
							}
							else {    //A或者左边拼接好的独在部分
								blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
								blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
								blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
							}
						}
						else if (x >= abs(MaxLeft) + (lastcalureWeight)
							&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
							&& y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
							&& y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight)
							)
						{	//b区域的值
							blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
						}
						else {    //黑色部分
							blendedImg(x, y, 0, 0) = 0;
							blendedImg(x, y, 0, 1) = 0;
							blendedImg(x, y, 0, 2) = 0;
						}
					}
				}

			}
			else
			{
				//待拼接的图片相对于匹配的图像是往上右移动
				//但整幅图像不需要扩展高度

				if (lastcalureHeight > 0)
				{
					//该图片相对第一幅图像在上边，重叠区域为上顶面减去上偏移开始到待拼接这幅图像结束
					cimg_forXY(blendedImg, x, y) {
						if (x < lastblendedImg._width && y < lastblendedImg._height)
						{
							
							if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+matchVecdx
								&& y >= abs(MaxUp - lastcalureHeight) + abs(matchVecdy)
								&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height
								) {    //混合
								blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
									* (float)((abs(MaxLeft) + (lastcalureWeight)+matchVecdx) - x) / (float)abs(matchVecdx)
									+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 0)
									* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
								blendedImg(x, y, 0, 1) = (float)lastblendedImg(x, y, 0, 1)
									* (float)((abs(MaxLeft) + (lastcalureWeight)+matchVecdx) - x) / (float)abs(matchVecdx)
									+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 1)
									* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
								blendedImg(x, y, 0, 2) = (float)lastblendedImg(x, y, 0, 2)
									* (float)((abs(MaxLeft) + (lastcalureWeight)+matchVecdx) - x) / (float)abs(matchVecdx)
									+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 2)
									* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);

							}
							else if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y >= abs(MaxUp - lastcalureHeight)
								&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height
								)
							{
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 2);
							}
							else {    //A或者左边拼接好的独在部分
								blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
								blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
								blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
							}
						}
						else if (x >= abs(MaxLeft) + (lastcalureWeight)
							&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
							&& y >= abs(MaxUp - lastcalureHeight)
							&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height
							)
						{
							//B独在的部分
							blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 2);
						}
						else {    //黑色部分
							blendedImg(x, y, 0, 0) = 0;
							blendedImg(x, y, 0, 1) = 0;
							blendedImg(x, y, 0, 2) = 0;
						}
					}
				}

				else
				{
					//该图片相对第一幅图像在下边，重叠区域为下顶边减去下偏移，得出第二幅图像的结尾像素距离整幅图像的距离
					//lastblendedImg._height - abs(MaxDown - lastcalureHeight)为第二幅图像结束拼接时候的高度
					cimg_forXY(blendedImg, x, y) {
						if (x < lastblendedImg._width && y < lastblendedImg._height)
						{
							//也可更改为这样子，不将剩余的下边界做插值，但是可能会消失，效果不好
							//if (x >= (lastblendedImg._width - matchVecdx) && y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight)
							//-srcImgB._height && y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - matchVecdy)
							if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+matchVecdx
								&& y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - abs(matchVecdy)
								&& y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight)) 
							{    //混合
								blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
									* (float)((abs(MaxLeft) + (lastcalureWeight)+matchVecdx) - x) / (float)abs(matchVecdx)
									+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0)
									* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
								blendedImg(x, y, 0, 1) = (float)lastblendedImg(x, y, 0, 1)
									* (float)((abs(MaxLeft) + (lastcalureWeight)+matchVecdx) - x) / (float)abs(matchVecdx)
									+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1)
									* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
								blendedImg(x, y, 0, 2) = (float)lastblendedImg(x, y, 0, 2)
									* (float)((abs(MaxLeft) + (lastcalureWeight)+matchVecdx) - x) / (float)abs(matchVecdx)
									+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2)
									* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);

							}
							else if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
								&& y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight)
								)
							{
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
							}
							else {    //A或者左边拼接好的独在部分
								blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
								blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
								blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
							}
						}
						else if (x >= abs(MaxLeft) + (lastcalureWeight)
							&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
							&& y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
							&& y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight)
							)
						{	//B部分数值附加进入
							blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
						}
						else {    //黑色部分
							blendedImg(x, y, 0, 0) = 0;
							blendedImg(x, y, 0, 1) = 0;
							blendedImg(x, y, 0, 2) = 0;
						}
					}
				}

			}

		}

		}
		

	}
	
	blendedImg.display("blendedImg");

	if (StitchPictureNum == 0)
		StitchPictureNum = 2;
	else
		StitchPictureNum++;
	blendedImg.save((char*)"test1.bmp");
	return blendedImg;

}

//最佳缝合线多分辨率融合部分
//2019-4-17需要将多分辨率代码提取出来，主要是因为不能再每次赋值的时候进行重复的运算操作
//进入不必要的循环
CImg<int> MyMorePictureStitch::MyStitchImageByOptimalSeamBlend(char* _filenameA, char* _filenameB, CImg<int> lastblendedImg, int matchVecdx, int matchVecdy,vector<char*> Other)
{
	char* isLaplacianBlending = Other[0];
	char* isDrawbestSeamLine;
	if (strcmp(isLaplacianBlending, (char*)"no thing exist") == 0)
	{
		isLaplacianBlending = (char*)"use LaplacianBlending";
		isDrawbestSeamLine = (char*)"not DrawbestSeamLine";
	}
	else 
	{
		isDrawbestSeamLine =  Other[1];
	}
	CImg<int> srcImgA, srcImgB;
	//将两幅图像之间的x和y的偏移量进行存储
	SaveSequentialImagesDifference(_filenameA, _filenameB, matchVecdx, matchVecdy);

	//读取图像
	srcImgA.load(_filenameA);
	srcImgB.load(_filenameB);

	//判断传入的上一个图像的是否为空
	//若为空则给其赋予图像a的数值
	if (lastblendedImg == NULL)
	{
		lastblendedImg = srcImgA;
	}

	//判断是最初开始的图片
	if (StitchPictureNum == 0)
	{
		//若是的话讲初始图片的宽度和高度都保存下来
		oriImageWidth = lastblendedImg._width;
		oriImageHeight = lastblendedImg._height;
	}

	//判断该图像待拼接图像是否在左边
	//是的话做上记录，并且进行图像扩展的调整
	if (matchVecdx > srcImgB._width)
	{
		//2019-03-09修改内容
		//需要拼接的图片在左边
		//新的记录值进行登记替换
		//待真实的 = （拼接图像 - dx/2）*2；
		//可以分析纸2先拼接全右图片
		//leftmatchVec真正的重合区域
		//matchVecx为左边剩余的待拼接面积
		int leftmatchVec = (srcImgB._width * 2 - matchVecdx);

		//与拼接右边图像不同的是，需要将dy先取负值
		//判断当前图像相对于上一个图像是向上移动还是向下移动
		//小于0，向下移动
		//大于0，向上移动
		//公式：dy1 = img2.y - img1.y
		//dy2 = img3.y - img2.y;
		//img3相对于img1:
		//dy = dy1+dy2 = img3.y - img1.y;
		//首先先计算偏移量，看是相对与第一个图像是向上还是向下
		//取负数
		//同时将偏移量取负数，转换为正确的图片放置位置相对偏移量
		//matchVecdy = 0 - matchVecdy;
		lastcalureHeight = lastcalureHeight + (matchVecdy);

		if (lastcalureHeight <= 0)
		{
			//负数比较谁最小
			//判断相对于最下的偏移量，如果小于扩增后的最大向下行，就无需要增大下行数
			//如果大于的话，就更新下的行数，同时更新最下的值
			MaxDown = MaxDown < lastcalureHeight ? MaxDown : lastcalureHeight;

		}
		else//向上移动 
		{
			//正数比较谁最大
			//判断相对于最上的偏移量，如果小于扩增后的最大向上行，就无需要增大上行数
				//如果大于的话，就更新上的行数，同时更新最上的值
			MaxUp = MaxUp > lastcalureHeight ? MaxUp : lastcalureHeight;
		}

		//计算出当前的图像相对于第一个图像横向偏移了多少
		lastcalureWeight = lastcalureWeight - (srcImgB._width - leftmatchVec);
		if (lastcalureWeight > 0)
			MaxRight = MaxRight > lastcalureWeight ? MaxRight : lastcalureWeight;
		else
			MaxLeft = MaxLeft < lastcalureWeight ? MaxLeft : lastcalureWeight;

		//待拼接的图片在图像的左边
		//左边的图像的相对纵向的方向的偏移值是相反数
		//dx得用新的表示方法
		//-----------------------合并的窗口大小位置-----------------------------------------
		//dx为两幅图像的特征点之间的横的差值
		//dy为两幅图像的特征点之间的竖的差值
		//判断是否需要增加画布的长度
		//宽度上是原图像，加上上增加值同时加上下增加值
		//若用上一个图像判断是否扩增行的高度
		//增加初始化图像的大小
		//宽度上是原图像，加上上增加值同时加上下增加值
		//下面的原理为所有宽度叠加取代原来的传入图像的宽度，
		//减少在拼接左边的图像时候出现需要库充左边图像的情况
		//blendedImg = CImg<int>(lastblendedImg._width + srcImgB._width - matchVecdx,
		//	srcImgB._height + abs(MaxDown) + abs(MaxUp), 1, 3, 0);
		blendedImg = CImg<int>(oriImageWidth + abs(MaxLeft) + abs(MaxRight),
			srcImgB._height + abs(MaxDown) + abs(MaxUp), 1, 3, 0);

		//判断新加入的图像是相对于上一个图像上移动还是下移动
		//判断x属于左边拼接好或者是第一个图像的区域
		//如果这个图像的点恰好在两个图像的重叠之处，需要做插值处理
		//计算重叠区域的插值的时候，将重合区域的变量同时处于重叠区域的宽度
		//此处需要重新讨论
		//判断是否从左边开始扩展图像
		if (lastcalureWeight == MaxLeft)
		{
			//该图像拼接的时候需要从左边增加空间
			//并且待拼接图像在左边
			//判断该图像是否引起高度的变化
			if (lastcalureHeight == MaxDown || lastcalureHeight == MaxUp)
			{
				//属于整幅图像需要增加高度的部分情况，

				if (matchVecdy <= 0) {
					//该图像相对第一幅图像在下边,则lastcalureHeight一定是在下且为负数
					//并且b图像（待拼接图像）的重叠区域为总的图像高度减去b图像的高度，得到起始高度
					//这里要注意的是，图像的起始位置和待拼接位置起始不一定都在同一横坐标上，可以使A待拼接图片
					//在整幅图像的内部靠近边界地方，见图
					CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameB, _filenameA, leftmatchVec,-matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
					cimg_forXY(blendedImg, x, y) {
						if (x >= blendedImg._width - lastblendedImg._width
							&& x < blendedImg._width
							&& y < lastblendedImg._height) {
							//这里可以选择使用插值或者不插值，插值有渐变，但是可能会和黑色区域重叠，
							//插值的话整个区域的话面积改变
							//但是不插值会有点突兀，可以复制下面的x大于起始语句
							//本例子为重叠区域插值,暂时方便调试，后期将其功能分析时候可以对整个区域插值
							//为图像b还未重合的区域
							//abs(MaxLeft) + (lastcalureWeight)== 0相互抵消
							if (x >= abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec)
								&& x < abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y >= blendedImg._height - srcImgB._height
								&& y < blendedImg._height - srcImgB._height + (srcImgB._height - abs(matchVecdy))
								)
							{    //混合
								//3-12最新发现，后期全部按照这个思路修改

								
								for (int k = 0;k < srcImgB._spectrum;k++)
								{
								blendedImg(x, y, 0, k) = rectImage(x - (abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec))
									, y - blendedImg._height - srcImgB._height, 0, k);
								}

							}
							else if (x >= blendedImg._width - lastblendedImg._width
								&& x < (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width - leftmatchVec)
								&& y >= blendedImg._height - srcImgB._height
								&& y < blendedImg._height) {    //B独在部分
								blendedImg(x, y, 0, 0) = srcImgB(x, y - (blendedImg._height - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x, y - (blendedImg._height - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x, y - (blendedImg._height - srcImgB._height), 0, 2);
							}
							else {    //A或者左边拼接好的独在部分
								blendedImg(x, y, 0, 0) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 0);
								blendedImg(x, y, 0, 1) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 1);
								blendedImg(x, y, 0, 2) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 2);
							}

						}
						//y部分应该是在左边图像的下边，右边
						else if (x >= (abs(MaxLeft) + (lastcalureWeight))
							&& x < blendedImg._width - lastblendedImg._width
							&& y >= blendedImg._height - srcImgB._height
							&& y < blendedImg._height) {    //B独在部分
							blendedImg(x, y, 0, 0) = srcImgB(x, y - (blendedImg._height - srcImgB._height), 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x, y - (blendedImg._height - srcImgB._height), 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x, y - (blendedImg._height - srcImgB._height), 0, 2);
						}
						else {    //黑色部分
							blendedImg(x, y, 0, 0) = 0;
							blendedImg(x, y, 0, 1) = 0;
							blendedImg(x, y, 0, 2) = 0;
						}


					}
				}
				//3-11号修改
				else {
					//matchVec.dy > 0;左侧图片需要往上左移动
					//该图像相对第一幅图像在上边,则lastcalureHeight一定是在上且为正数
					CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameB, _filenameA, leftmatchVec, -matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
					cimg_forXY(blendedImg, x, y) {
						if (x >= blendedImg._width - lastblendedImg._width
							&& x < blendedImg._width
							&& y >= blendedImg._height - lastblendedImg._height
							&& y < blendedImg._height
							) {
							//这里可以选择使用插值或者不插值，插值有渐变，但是可能会和黑色区域重叠，
							//插值的话整个区域的话面积改变
							//但是不插值会有点突兀，可以复制下面的x大于起始语句
							//不插值区间为
							if (x >= abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec)
								&& x < abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y >= abs(matchVecdy)
								&& y < srcImgB._height) 
							{    //混合

								for (int k = 0;k < srcImgB._spectrum;k++)
								{
									blendedImg(x, y, 0, k) = rectImage(x - (abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec))
										, y - abs(matchVecdy), 0, k);
								}
								/*blendedImg(x, y, 0, 0) = (float)lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y - (blendedImg._height - lastblendedImg._height), 0, 0)
									* (float)(x - (abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
									+ (float)srcImgB(x, y, 0, 0)
									* (float)((abs(MaxLeft) + lastcalureWeight + srcImgB._width) - x) / (float)abs(leftmatchVec);

								*/

							}
							else if (x >= blendedImg._width - lastblendedImg._width
								&& x < abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width)
								&& y < srcImgB._height)
							{	//B部分
								blendedImg(x, y, 0, 0) = srcImgB(x, y, 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x, y, 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x, y, 0, 2);
							}
							else {    //A独在部分
								blendedImg(x, y, 0, 0) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y - (blendedImg._height - lastblendedImg._height), 0, 0);
								blendedImg(x, y, 0, 1) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y - (blendedImg._height - lastblendedImg._height), 0, 1);
								blendedImg(x, y, 0, 2) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y - (blendedImg._height - lastblendedImg._height), 0, 2);
							}
						}
						else if (x >= (abs(MaxLeft) + (lastcalureWeight))
							&& x < blendedImg._width - lastblendedImg._width
							&& y < srcImgB._height) {    //B独在部分
							blendedImg(x, y, 0, 0) = srcImgB(x, y, 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x, y, 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x, y, 0, 2);
						}
						else {    //黑色部分
							blendedImg(x, y, 0, 0) = 0;
							blendedImg(x, y, 0, 1) = 0;
							blendedImg(x, y, 0, 2) = 0;
						}
					}
				}
			}
			else
			{
				//属于整幅图像不需要增加高度的部分情况，按照3-11号重构代码
				//但是该图像左边又增加空间变化
				if (matchVecdy <= 0)
				{

					//待拼接的图片相对于匹配的图像是往下左移动
					//但整幅图像不需要扩展高度,不需要扩展宽度
					CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameB, _filenameA, leftmatchVec, -matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
					if (lastcalureHeight > 0)
					{
						//该图片相对第一幅图像在上边，重叠区域为上顶面减去上偏移开始到待拼接这幅图像结束
						
						cimg_forXY(blendedImg, x, y)
						{
							if (x >= blendedImg._width - lastblendedImg._width
								&& x < blendedImg._width
								&& y < lastblendedImg._height)
							{
								//这里可以选择使用插值或者不插值，插值有渐变，但是可能会和黑色区域重叠，
								//插值的话整个区域的话面积改变
								//但是不插值会有点突兀，可以复制下面的x大于起始语句

								//本例子为重叠区域插值,暂时方便调试，后期将其功能分析时候可以对整个区域插值
								//(srcImgB._width - leftmatchVec) 为图像b还未重合的区域
								if (x >= abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec)
									&& x < abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
									&& y >= abs(MaxUp - lastcalureHeight)
									&& y < abs(MaxUp - lastcalureHeight) + srcImgB._height - abs(matchVecdy)
									)
								{

									
									for (int k = 0;k < srcImgB._spectrum;k++)
									{
										blendedImg(x, y, 0, k) = rectImage(x - (abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec))
											, y - abs(MaxUp - lastcalureHeight), 0, k);
									}

									/*
									blendedImg(x, y, 0, 0) = (float)lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 0)
										* (float)(x - (abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
										+ (float)srcImgB(x, y - abs(MaxUp - lastcalureHeight), 0, 0)
										* (float)((abs(MaxLeft) + lastcalureWeight + (srcImgB._width)) - x) / (float)abs(leftmatchVec);
									*/
								}
								//将b区域的值也附加进去，这部分的图像尚未进行插值操作
								else if (x >= blendedImg._width - lastblendedImg._width
									&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
									&& y >= abs(MaxUp - lastcalureHeight)
									&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B独在部分
									blendedImg(x, y, 0, 0) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 2);
								}
								else {    //A或者左边拼接好的独在部分
									blendedImg(x, y, 0, 0) = lastblendedImg(x - blendedImg._width - lastblendedImg._width, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x - blendedImg._width - lastblendedImg._width, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x - blendedImg._width - lastblendedImg._width, y, 0, 2);
								}
							}
							//y部分应该是在左边图像的下边，右边,基本这里不会运行，情况都包含在上边循环
							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&& x <= (abs(MaxLeft) + (lastcalureWeight)) + blendedImg._width - lastblendedImg._width
								&& y >= abs(MaxUp - lastcalureHeight)
								&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B独在部分
								blendedImg(x, y, 0, 0) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 2);
							}
							else {    //黑色部分
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}
					else
					{
						//该图片相对第一幅图像在下边，重叠区域为下顶面减去下偏移开始到两张图片重叠区域结束
						CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameB, _filenameA, leftmatchVec, -matchVecdy, isLaplacianBlending, isDrawbestSeamLine);

						cimg_forXY(blendedImg, x, y)
						{
							if (x >= blendedImg._width - lastblendedImg._width
								&& x < blendedImg._width
								&& y < lastblendedImg._height)
							{
								//这里可以选择使用插值或者不插值，插值有渐变，但是可能会和黑色区域重叠，
								//插值的话整个区域的话面积改变
								//但是不插值会有点突兀，可以复制下面的x大于起始语句

								//本例子为重叠区域插值,暂时方便调试，后期将其功能分析时候可以对整个区域插值
								//(srcImgB._width - leftmatchVec) 为图像b还未重合的区域
								if (x >= abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec)
									&& x < abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
									&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
									&& y < blendedImg._height - abs(MaxDown - lastcalureHeight) + (srcImgB._height - abs(matchVecdy))
									)
								{

									
									for (int k = 0;k < srcImgB._spectrum;k++)
									{
										blendedImg(x, y, 0, k) = rectImage(x - (abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec))
											, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, k);
									}

									/*
									blendedImg(x, y, 0, 0) = (float)lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 0)
										* (float)(x - (abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
										+ (float)srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0)
										* (float)((abs(MaxLeft) + lastcalureWeight + (srcImgB._width)) - x) / (float)abs(leftmatchVec);
									*/
								}
								//将b区域的值也附加进去，这部分的图像尚未进行插值操作
								else if (x >= blendedImg._width - lastblendedImg._width
									&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
									&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
									&& y < blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B独在部分
									blendedImg(x, y, 0, 0) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
								}
								else {    //A或者左边拼接好的独在部分
									blendedImg(x, y, 0, 0) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 2);
								}


							}
							//y部分应该是在左边图像的下边，右边
							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&& x <= blendedImg._width - lastblendedImg._width
								&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
								&& y < blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B独在部分
								blendedImg(x, y, 0, 0) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
							}
							else {    //黑色部分
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}

				}
				else
				{
					//待拼接的图片相对于匹配的到图像是往上左移动
					//但整幅图像不需要扩展高度,不需要扩展宽度
					if (lastcalureHeight > 0)
					{
						CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameB, _filenameA, leftmatchVec, -matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
						//该图片相对第一幅图像在上边，重叠区域为上顶面减去上偏移开始到待拼接这幅图像结束
						cimg_forXY(blendedImg, x, y)
						{
							if (x > blendedImg._width - lastblendedImg._width
								&& x < blendedImg._width
								&& y < lastblendedImg._height)
							{
								//这里可以选择使用插值或者不插值，插值有渐变，但是可能会和黑色区域重叠，
								//插值的话整个区域的话面积改变
								//但是不插值会有点突兀，可以复制下面的x大于起始语句

								//本例子为重叠区域插值,暂时方便调试，后期将其功能分析时候可以对整个区域插值
								//(srcImgB._width - leftmatchVec) 为图像b还未重合的区域
								if (x >= abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec)
									&& x < abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
									&& y >= abs(MaxUp - lastcalureHeight) + abs(matchVecdy)
									&& y < abs(MaxUp - lastcalureHeight) + srcImgB._height
									)
								{
									//这里给我感觉有点问题，貌似没有进行修改
									//CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(srcImgA, srcImgB, leftmatchVec, matchVecdy);
									
									for (int k = 0;k < srcImgB._spectrum;k++)
									{
										blendedImg(x, y, 0, k) = rectImage(x - (abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec))
											, y - (abs(MaxUp - lastcalureHeight) + abs(matchVecdy)), 0, k);
									}
 
									/*
									blendedImg(x, y, 0, 0) = (float)lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 0)
										* (float)(x - (abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
										+ (float)srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 0)
										* (float)((abs(MaxLeft) + lastcalureWeight + (srcImgB._width)) - x) / (float)abs(leftmatchVec);

									*/
								}
								//将b区域的值也附加进去，这部分的图像尚未进行插值操作
								else if (x >= blendedImg._width - lastblendedImg._width
									&& x <= (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width)
									&& y >= abs(MaxUp - lastcalureHeight)
									&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B独在部分
									blendedImg(x, y, 0, 0) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 2);
								}
								else {    //A或者左边拼接好的独在部分
									blendedImg(x, y, 0, 0) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 2);
								}
							}
							//y部分应该是在左边图像的下边，右边,基本这里不会运行，情况都包含在上边循环
							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&& x <= blendedImg._width - lastblendedImg._width
								&& y >= abs(MaxUp - lastcalureHeight)
								&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B独在部分
								blendedImg(x, y, 0, 0) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 2);
							}
							else {    //黑色部分
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}
					else
					{
						CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameB, _filenameA, leftmatchVec, -matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
						//该图片相对第一幅图像在下边，重叠区域为下顶面减去下偏移开始到两张图片重叠区域结束
						cimg_forXY(blendedImg, x, y)
						{
							if (x >= blendedImg._width - lastblendedImg._width
								&& x < blendedImg._width
								&& y < lastblendedImg._height)
							{
								//这里可以选择使用插值或者不插值，插值有渐变，但是可能会和黑色区域重叠，
								//插值的话整个区域的话面积改变
								//但是不插值会有点突兀，可以复制下面的x大于起始语句

								//本例子为重叠区域插值,暂时方便调试，后期将其功能分析时候可以对整个区域插值
								//(srcImgB._width - leftmatchVec) 为图像b还未重合的区域
								if (x >= abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec)
									&& x < abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
									&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - (srcImgB._height - abs(matchVecdy))
									&& y < blendedImg._height - abs(MaxDown - lastcalureHeight)
									)
								{
//CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(srcImgA, srcImgB, leftmatchVec, matchVecdy);
									
									for (int k = 0;k < srcImgB._spectrum;k++)
									{
										blendedImg(x, y, 0, k) = rectImage(x - (abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec))
											, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - (srcImgB._height - abs(matchVecdy))), 0, k);
									}

									/*blendedImg(x, y, 0, 0) = (float)lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 0)
										* (float)(x - (abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
										+ (float)srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - (srcImgB._height)), 0, 0)
										* (float)((abs(MaxLeft) + lastcalureWeight + (srcImgB._width)) - x) / (float)abs(leftmatchVec);
									
									*/
								}
								//将b区域的值也附加进去，这部分的图像尚未进行插值操作
								else if (x >= blendedImg._width - lastblendedImg._width
									&& x <= (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width)
									&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
									&& y < blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B独在部分
									blendedImg(x, y, 0, 0) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
								}
								else {    //A或者左边拼接好的独在部分
									blendedImg(x, y, 0, 0) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 2);
								}
							}

							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&& x <= blendedImg._width - lastblendedImg._width
								&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
								&& y < blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B独在部分
								blendedImg(x, y, 0, 0) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
							}
							else {    //黑色部分
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}

				}
			}
		}
		else
		{
			//该图像拼接的时候不需要从左边增加空间
			//待拼接图像在原来的宽度中
			//判断该图像是否引起高度的变化
			if (lastcalureHeight == MaxDown || lastcalureHeight == MaxUp)
			{
				//属于整幅图像需要增加高度的部分情况，按照3-9号代码
				if (matchVecdy <= 0) {
					//该图像相对第一幅图像在下边,则lastcalureHeight一定是在下且为负数
					//并且b图像（待拼接图像）的重叠区域为总的图像高度减去b图像的高度，得到起始高度
					CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameB, _filenameA, leftmatchVec, -matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
					cimg_forXY(blendedImg, x, y) {
						if (x < lastblendedImg._width && y < lastblendedImg._height) {
							//这里可以选择使用插值或者不插值，插值有渐变，但是可能会和黑色区域重叠，
							//插值的话整个区域的话面积改变
							//但是不插值会有点突兀，可以复制下面的x大于起始语句
							//不插值区间为if (x >= abs(MaxLeft) + lastcalureWeight + srcImgB - leftmatchVec） && y >= blendedImg._height - srcImgB._height)
							//blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
							// * (float)(x - (abs(MaxLeft) + lastcalureWeight)) / (float)abs(matchVecdx)
							//	+ (float)srcImgB(x - (abs(MaxLeft) + lastcalureWeight), y - (blendedImg._height - srcImgB._height), 0, 0)
							//	* (float)(x - (abs(MaxLeft) + lastcalureWeight)) / (float)abs(matchVecdx);
							//本例子为重叠区域插值,暂时方便调试，后期将其功能分析时候可以对整个区域插值
							//(srcImgB._width - leftmatchVec) 为图像b还未重合的区域
							if (x >= abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec)
								&& x < abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y >= blendedImg._height - srcImgB._height) {    //混合

//CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(srcImgA, srcImgB, leftmatchVec, matchVecdy);
								
								for (int k = 0;k < srcImgB._spectrum;k++)
								{
									blendedImg(x, y, 0, k) = rectImage(x - (abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec))
										, y - (blendedImg._height - srcImgB._height), 0, k);
								}
								
								
								/*blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
									* (float)((abs(MaxLeft) + lastcalureWeight + srcImgB._width) - x) / (float)abs(leftmatchVec)
									+ (float)srcImgB(x - (abs(MaxLeft) + lastcalureWeight), y - (blendedImg._height - srcImgB._height), 0, 0)
									* (float)(x - (abs(MaxLeft) + lastcalureWeight + (srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec);
								*/

							}
							//将b区域的值也附加进去，这部分的图像尚未进行插值操作
							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&& x < (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width)
								&& y >= blendedImg._height - srcImgB._height
								&& y < blendedImg._height) {    //B独在部分
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 2);
							}
							else {    //A或者左边拼接好的独在部分
								blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
								blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
								blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
							}

						}
						//y部分应该是在左边图像的下边，右边
						else if (x >= (abs(MaxLeft) + (lastcalureWeight))
							&& x < (abs(MaxLeft) + (lastcalureWeight)) + srcImgB._width
							&& y >= blendedImg._height - srcImgB._height && y < blendedImg._height) {    //B独在部分
							blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 2);
						}
						else {    //黑色部分
							blendedImg(x, y, 0, 0) = 0;
							blendedImg(x, y, 0, 1) = 0;
							blendedImg(x, y, 0, 2) = 0;
						}
					}
				}
				//3-11号修改
				else {
					//matchVec.dy > 0; 右侧图片需要往上左移动
					//该图像相对第一幅图像在上边,则lastcalureHeight一定是在上且为正数
					CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameB, _filenameA, leftmatchVec, -matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
					cimg_forXY(blendedImg, x, y) {
						if (x < lastblendedImg._width && y >= blendedImg._height - lastblendedImg._height) {
							//这里可以选择使用插值或者不插值，插值有渐变，但是可能会和黑色区域重叠，
							//插值的话整个区域的话面积改变
							//但是不插值会有点突兀，可以复制下面的x大于起始语句
							//不插值区间为
							if (x >= abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec)
								&& x < abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y < srcImgB._height) {    //混合


								//CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(srcImgA, srcImgB, leftmatchVec, matchVecdy);
								
								for (int k = 0;k < srcImgB._spectrum;k++)
								{
									blendedImg(x, y, 0, k) = rectImage(x - (abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec))
										, y - abs(matchVecdy), 0, k);
								}
								
								

								/*blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 0)
									* (float)(x - (abs(MaxLeft) + lastcalureWeight + (srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
									+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 0)
									* (float)((abs(MaxLeft) + lastcalureWeight + srcImgB._width) - x) / (float)abs(leftmatchVec);
								*/
							}
							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&& x <= abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width)
								&& y < srcImgB._height)
							{
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 2);
							}
							else {    //A独在部分
								blendedImg(x, y, 0, 0) = lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 0);
								blendedImg(x, y, 0, 1) = lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 1);
								blendedImg(x, y, 0, 2) = lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 2);
							}
						}
						else if (x >= (abs(MaxLeft) + (lastcalureWeight))
							&& x <= (abs(MaxLeft) + (lastcalureWeight)) + srcImgB._width
							&& y < srcImgB._height) {    //B独在部分
							blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 2);
						}
						else {    //黑色部分
							blendedImg(x, y, 0, 0) = 0;
							blendedImg(x, y, 0, 1) = 0;
							blendedImg(x, y, 0, 2) = 0;
						}
					}
				}
			}
			else
			{
				//属于整幅图像不需要增加高度的部分情况，按照3-11号重构代码

				if (matchVecdy <= 0)
				{

					//待拼接的图片相对于匹配的图像是往下左移动
					//但整幅图像不需要扩展高度,不需要扩展宽度
					if (lastcalureHeight > 0)
					{
						//该图片相对第一幅图像在上边，重叠区域为上顶面减去上偏移开始到待拼接这幅图像结束
						CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameB, _filenameA, leftmatchVec, -matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
						cimg_forXY(blendedImg, x, y)
						{
							if (x < lastblendedImg._width && y < lastblendedImg._height)
							{
								//这里可以选择使用插值或者不插值，插值有渐变，但是可能会和黑色区域重叠，
								//插值的话整个区域的话面积改变
								//但是不插值会有点突兀，可以复制下面的x大于起始语句

								//本例子为重叠区域插值,暂时方便调试，后期将其功能分析时候可以对整个区域插值
								//(srcImgB._width - leftmatchVec) 为图像b还未重合的区域
								if (x >= abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec)
									&& x < abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
									&& y >= abs(MaxUp - lastcalureHeight)
									&& y < abs(MaxUp - lastcalureHeight) + srcImgB._height - abs(matchVecdy)
									)
								{
									
									for (int k = 0;k < srcImgB._spectrum;k++)
									{
										blendedImg(x, y, 0, k) = rectImage(x - (abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec))
											, y - abs(MaxUp - lastcalureHeight), 0, k);
									}
									//CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(srcImgA, srcImgB, leftmatchVec, matchVecdy);

									/*blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
										* (float)(x - (abs(MaxLeft) + lastcalureWeight + (srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
										+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 0)
										* (float)(abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width) - x) / (float)abs(leftmatchVec);
									*/
								}
								//将b区域的值也附加进去，这部分的图像尚未进行插值操作
								else if (x >= (abs(MaxLeft) + (lastcalureWeight))
									&& x < (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width)
									&& y >= abs(MaxUp - lastcalureHeight)
									&& y < abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B独在部分
									blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 2);
								}
								else {    //A或者左边拼接好的独在部分
									blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
								}
							}
							//y部分应该是在左边图像的下边，右边,基本这里不会运行，情况都包含在上边循环
							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&& x <= (abs(MaxLeft) + (lastcalureWeight)) + srcImgB._width
								&& y >= abs(MaxUp - lastcalureHeight)
								&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B独在部分
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 2);
							}
							else {    //黑色部分
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}
					else
					{
						//该图片相对第一幅图像在下边，重叠区域为下顶面减去下偏移开始到两张图片重叠区域结束
						CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameB, _filenameA, leftmatchVec, -matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
						cimg_forXY(blendedImg, x, y)
						{
							if (x < lastblendedImg._width && y < lastblendedImg._height)
							{
								//这里可以选择使用插值或者不插值，插值有渐变，但是可能会和黑色区域重叠，
								//插值的话整个区域的话面积改变
								//但是不插值会有点突兀，可以复制下面的x大于起始语句

								//本例子为重叠区域插值,暂时方便调试，后期将其功能分析时候可以对整个区域插值
								//(srcImgB._width - leftmatchVec) 为图像b还未重合的区域
								if (x >= abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec)
									&& x < abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
									&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
									&& y < blendedImg._height - abs(MaxDown - lastcalureHeight) + (srcImgB._height - abs(matchVecdy))
									)
								{
									//CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(srcImgA, srcImgB, leftmatchVec, matchVecdy);
									
									
									for (int k = 0;k < srcImgB._spectrum;k++)
									{
										blendedImg(x, y, 0, k) = rectImage(x - (abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec))
											, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, k);
									}

									/*
									blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
										* (float)(x - (abs(MaxLeft) + lastcalureWeight + (srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
										+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0)
										* (float)(abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width) - x) / (float)abs(leftmatchVec);
									*/
								}
								//将b区域的值也附加进去，这部分的图像尚未进行插值操作
								else if (x >= (abs(MaxLeft) + (lastcalureWeight))
									&& x < (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width)
									&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
									&& y < blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B独在部分
									blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
								}
								else {    //A或者左边拼接好的独在部分
									blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
								}


							}
							//y部分应该是在左边图像的下边，右边
							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&& x <= (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width)
								&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
								&& y < blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B独在部分
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
							}
							else {    //黑色部分
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}


				}
				else
				{
					//待拼接的图片相对于匹配的到图像是往上左移动
					//但整幅图像不需要扩展高度,不需要扩展宽度
					if (lastcalureHeight > 0)
					{
						//该图片相对第一幅图像在上边，重叠区域为上顶面减去上偏移开始到待拼接这幅图像结束
						CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameB, _filenameA, leftmatchVec, -matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
						cimg_forXY(blendedImg, x, y)
						{
							if (x < lastblendedImg._width && y < lastblendedImg._height)
							{
								//这里可以选择使用插值或者不插值，插值有渐变，但是可能会和黑色区域重叠，
								//插值的话整个区域的话面积改变
								//但是不插值会有点突兀，可以复制下面的x大于起始语句

								//本例子为重叠区域插值,暂时方便调试，后期将其功能分析时候可以对整个区域插值
								//(srcImgB._width - leftmatchVec) 为图像b还未重合的区域
								if (x >= abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec)
									&& x < abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
									&& y >= abs(MaxUp - lastcalureHeight) + abs(matchVecdy)
									&& y < abs(MaxUp - lastcalureHeight) + srcImgB._height
									)
								{
									//CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(srcImgA, srcImgB, leftmatchVec, matchVecdy);
									
									for (int k = 0;k < srcImgB._spectrum;k++)
									{
										blendedImg(x, y, 0, k) = rectImage(x - (abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec))
											, y - abs(MaxUp - lastcalureHeight) + abs(matchVecdy), 0, k);
									}
									

									/*blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
										* (float)(x - (abs(MaxLeft) + lastcalureWeight + (srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec)
										+ (float)srcImgB(x - (abs(MaxLeft) + lastcalureWeight), y - abs(MaxUp - lastcalureHeight), 0, 0)
										* (float)(abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width) - x) / (float)abs(leftmatchVec);
									*/
								}
								//将b区域的值也附加进去，这部分的图像尚未进行插值操作
								else if (x >= (abs(MaxLeft) + (lastcalureWeight))
									&& x <= (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width)
									&& y >= abs(MaxUp - lastcalureHeight)
									&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B独在部分
									blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 2);
								}
								else {    //A或者左边拼接好的独在部分
									blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
								}
							}
							//y部分应该是在左边图像的下边，右边,基本这里不会运行，情况都包含在上边循环
							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&& x <= (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width)
								&& y >= abs(MaxUp - lastcalureHeight)
								&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B独在部分
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 2);
							}
							else {    //黑色部分
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}
					else
					{
						//该图片相对第一幅图像在下边，重叠区域为下顶面减去下偏移开始到两张图片重叠区域结束
						CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameB, _filenameA, leftmatchVec, -matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
						cimg_forXY(blendedImg, x, y)
						{
							if (x < lastblendedImg._width && y < lastblendedImg._height)
							{
								//这里可以选择使用插值或者不插值，插值有渐变，但是可能会和黑色区域重叠，
								//插值的话整个区域的话面积改变
								//但是不插值会有点突兀，可以复制下面的x大于起始语句

								//本例子为重叠区域插值,暂时方便调试，后期将其功能分析时候可以对整个区域插值
								//(srcImgB._width - leftmatchVec) 为图像b还未重合的区域
								if (x >= abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec)
									&& x < abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
									&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - abs(matchVecdy)
									&& y < blendedImg._height - abs(MaxDown - lastcalureHeight)
									)
								{

									//CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(srcImgA, srcImgB, leftmatchVec, matchVecdy);
								
									for (int k = 0;k < srcImgB._spectrum;k++)
									{
										blendedImg(x, y, 0, k) = rectImage(x - (abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec))
											, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - abs(matchVecdy)), 0, k);
									}
									
									
									/*
									blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
										* (float)(abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width) - x) / (float)abs(leftmatchVec)
										+ (float)srcImgB(x - (abs(MaxLeft) + lastcalureWeight), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0)
										* (float)(x - (abs(MaxLeft) + lastcalureWeight + (srcImgB._width - leftmatchVec))) / (float)abs(leftmatchVec);
									
								*/
								}
								//将b区域的值也附加进去，这部分的图像尚未进行插值操作
								else if (x >= (abs(MaxLeft) + (lastcalureWeight))
									&& x <= (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width)
									&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
									&& y < blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B独在部分
									blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
								}
								else {    //A或者左边拼接好的独在部分
									blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
								}


							}
							//y部分应该是在左边图像的下边，右边
							//一般不会再使用
							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&& x <= (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width)
								&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
								&& y < blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B独在部分
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
							}
							else {    //黑色部分
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}

				}
			}
		}

	}
	else
	{
		//需要拼接的图像在右边
		//判断当前图像相对于上一个图像是向上移动还是向下移动
		//小于0，向下移动
		//大于0，向上移动
		//公式：dy1 = img2.y - img1.y
		//dy2 = img3.y - img2.y;
		//img3相对于img1:
		//dy = dy1+dy2 = img3.y - img1.y;
		//首先先计算偏移量，看是相对与第一个图像是向上还是向下

		lastcalureHeight += matchVecdy;
		if (lastcalureHeight <= 0)
		{
			//负数比较谁最小
			//判断相对于最下的偏移量，如果小于扩增后的最大向下行，就无需要增大下行数
			//如果大于的话，就更新下的行数，同时更新最下的值
			MaxDown = MaxDown < lastcalureHeight ? MaxDown : lastcalureHeight;

		}
		else//向上移动 
		{
			//正数比较谁最大
			//判断相对于最上的偏移量，如果小于扩增后的最大向上行，就无需要增大上行数
				//如果大于的话，就更新上的行数，同时更新最上的值
			MaxUp = MaxUp > lastcalureHeight ? MaxUp : lastcalureHeight;
		}

		//计算出当前的图像相对于第一个图像横向偏移了多少
		lastcalureWeight = lastcalureWeight + srcImgB._width - matchVecdx;
		if (lastcalureWeight > 0)
			MaxRight = MaxRight > lastcalureWeight ? MaxRight : lastcalureWeight;
		else
			MaxLeft = MaxLeft < lastcalureWeight ? MaxLeft : lastcalureWeight;

		//待拼接的图片在图像的右边
		//-----------------------合并的窗口大小位置-----------------------------------------
		//dx为两幅图像的特征点之间的横的差值
		//dy为两幅图像的特征点之间的竖的差值
		//判断是否需要增加画布的长度
		//宽度上是原图像，加上上增加值同时加上下增加值
		//若用上一个图像判断是否扩增行的高度
		//增加初始化图像的大小
		//下面的原理为所有宽度叠加取代原来的传入图像的宽度，
		//减少在拼接左边的图像时候出现需要库充左边图像的情况
		//blendedImg = CImg<int>(lastblendedImg._width + srcImgB._width - matchVecdx,
		//	srcImgB._height + abs(MaxDown) + abs(MaxUp), 1, 3, 0);

		//
		blendedImg = CImg<int>(oriImageWidth + abs(MaxLeft) + abs(MaxRight),
			srcImgB._height + abs(MaxDown) + abs(MaxUp), 1, 3, 0);


		//判断新加入的图像是相对于上一个图像上移动还是下移动
		//判断x属于左边拼接好或者是第一个图像的区域
		//如果这个图像的点恰好在两个图像的重叠之处，需要做插值处理
		//计算重叠区域的插值的时候，将重合区域的变量同时处于重叠区域的宽度
		//此处需要重新讨论

		if (lastcalureWeight == MaxRight)
		{
			//该图像拼接的时候需要从右边增加空间
			//并且待拼接图像在右边
			//判断该图像是否引起高度的变化
			if (lastcalureHeight == MaxDown || lastcalureHeight == MaxUp)
			{
				//属于整幅图像需要增加高度的部分情况，按照3-9号代码
				if (matchVecdy <= 0) {
					//该图像相对第一幅图像在下边,则lastcalureHeight一定是在下且为负数
					//并且b图像（待拼接图像）的重叠区域为总的图像高度减去b图像的高度，得到起始高度
					CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameA, _filenameB, matchVecdx, matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
					cimg_forXY(blendedImg, x, y) {
						if (x < lastblendedImg._width && y < lastblendedImg._height) {
							if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x < abs(MaxLeft) + (lastcalureWeight)+matchVecdx
								&& y >= blendedImg._height - srcImgB._height
								&& y < blendedImg._height - srcImgB._height + (srcImgB._height - abs(matchVecdy))
								) {    //混合

								//CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(srcImgA, srcImgB, matchVecdx, matchVecdy);
								
								
								for (int k = 0;k < srcImgB._spectrum;k++)
								{
									blendedImg(x, y, 0, 0) = rectImage(x - (abs(MaxLeft) + (lastcalureWeight))
										, y - (blendedImg._height - srcImgB._height), 0, 0);
								}
								
								/*
								blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
									* (float)(abs(MaxLeft) + (lastcalureWeight)+matchVecdx - x) / (float)abs(matchVecdx)
									+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 0)
									* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
								*/
							}
							//y部分应该是在左边图像的下边，右边
							else if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y >= blendedImg._height - srcImgB._height && y < blendedImg._height) {    //B独在部分
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 2);
							}
							else {    //A或者左边拼接好的独在部分
								blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
								blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
								blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
							}

						}
						//y部分应该是在左边图像的下边，右边
						else if (x >= abs(MaxLeft) + (lastcalureWeight)
							&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
							&& y >= blendedImg._height - srcImgB._height && y < blendedImg._height) {    //B独在部分
							blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 2);
						}
						else {    //黑色部分
							blendedImg(x, y, 0, 0) = 0;
							blendedImg(x, y, 0, 1) = 0;
							blendedImg(x, y, 0, 2) = 0;
						}
					}
				}
				//3-11号修改
				else {
					//matchVec.dy > 0; 右侧图片需要往上左移动
					//该图像相对第一幅图像在上边,则lastcalureHeight一定是在上且为正数
					CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameA, _filenameB, matchVecdx, matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
					cimg_forXY(blendedImg, x, y) {
						if (x < lastblendedImg._width
							&& y >= blendedImg._height - lastblendedImg._height) {
							if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x < abs(MaxLeft) + (lastcalureWeight)+matchVecdx
								&& y > srcImgB._height - abs(matchVecdy)
								&& y < srcImgB._height) {    //混合

								//CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(srcImgA, srcImgB, matchVecdx, matchVecdy);
								
								for (int k = 0;k < srcImgB._spectrum;k++)
								{
									blendedImg(x, y, 0, k) = rectImage(x - (abs(MaxLeft) + (lastcalureWeight))
										, y - (srcImgB._height - abs(matchVecdy)), 0, k);
								}
								
								/*
								blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 0)
									* (float)(abs(MaxLeft) + (lastcalureWeight)+matchVecdx - x) / (float)abs(matchVecdx)
									+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 0)
									* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
								*/
							}
							else if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y < srcImgB._height) {    //B独在部分
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 2);
							}
							else {    //A独在部分
								blendedImg(x, y, 0, 0) = lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 0);
								blendedImg(x, y, 0, 1) = lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 1);
								blendedImg(x, y, 0, 2) = lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 2);
							}
						}
						else if (x >= abs(MaxLeft) + (lastcalureWeight)
							&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
							&& y < srcImgB._height) {    //B独在部分
							blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 2);
						}
						else {    //黑色部分
							blendedImg(x, y, 0, 0) = 0;
							blendedImg(x, y, 0, 1) = 0;
							blendedImg(x, y, 0, 2) = 0;
						}
					}
				}
			}
			else
			{
				//属于不需要增加高度的部分，按照3-11号代码另行处理
				if (matchVecdy <= 0)
				{
					//待拼接的图片相对于匹配的图像是往下右移动
					//但整幅图像不需要扩展高度
					if (lastcalureHeight > 0)
					{
						//该图片相对第一幅图像在上边，重叠区域为上顶面减去上偏移开始到待拼接这幅图像结束
						CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameA, _filenameB, matchVecdx, matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
						cimg_forXY(blendedImg, x, y) {
							if (x < lastblendedImg._width && y < lastblendedImg._height)
							{
								//也可更改为这样子，不将剩余的下边界做插值，但是可能会消失，效果不好
								//if (x >= (lastblendedImg._width - matchVecdx) && y >= abs(MaxUp - lastcalureHeight)
								//	&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height - abs(matchVecdy))
								if (x >= abs(MaxLeft) + (lastcalureWeight)
									&& x < abs(MaxLeft) + (lastcalureWeight)+matchVecdx
									&& y >= abs(MaxUp - lastcalureHeight)
									&& y <abs(MaxUp - lastcalureHeight) + srcImgB._height - abs(matchVecdy)) {    //混合
									
									
									for (int k = 0;k < srcImgB._spectrum;k++)
									{
										blendedImg(x, y, 0, k) = rectImage(x - (abs(MaxLeft) + (lastcalureWeight))
											, y - (abs(MaxUp - lastcalureHeight)), 0, k);
									}
									
									/*blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
										* (float)(abs(MaxLeft) + (lastcalureWeight)+matchVecdx - x) / (float)abs(matchVecdx)
										+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 0)
										* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
									*/

								}
								else if (x >= abs(MaxLeft) + (lastcalureWeight)
									&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
									&& y >= abs(MaxUp - lastcalureHeight)
									&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B独在部分
									blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 2);
								}
								else {    //A或者左边拼接好的独在部分
									blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
								}
							}
							else if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y >= abs(MaxUp - lastcalureHeight)
								&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B独在部分
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 2);
							}
							else {    //黑色部分
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}

					else
					{
						//该图片相对第一幅图像在下边，重叠区域为下顶边减去下偏移，得出第二幅图像的结尾像素距离整幅图像的距离
						//lastblendedImg._height - abs(MaxDown - lastcalureHeight)为第二幅图像结束拼接时候的高度
						CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameA, _filenameB, matchVecdx, matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
						cimg_forXY(blendedImg, x, y) {
							if (x < lastblendedImg._width && y < lastblendedImg._height)
							{
								//也可更改为这样子，不将剩余的下边界做插值，但是可能会消失，效果不好
								//if (x >= (lastblendedImg._width - matchVecdx) && y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight)
								//-srcImgB._height && y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - matchVecdy)
								if (x >= abs(MaxLeft) + (lastcalureWeight)
									&& x < abs(MaxLeft) + (lastcalureWeight)+matchVecdx
									&& y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
									&& y < lastblendedImg._height - abs(MaxDown - lastcalureHeight) - abs(matchVecdy))
								{    //混合
									
									for (int k = 0;k < srcImgB._spectrum;k++)
									{
										blendedImg(x, y, 0, k) = rectImage(x - (abs(MaxLeft) + (lastcalureWeight))
											, y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, k);
									}

									/*blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
										* (float)(abs(MaxLeft) + (lastcalureWeight)+matchVecdx - x) / (float)abs(matchVecdx)
										+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0)
										* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
									*/
								}
								else if (x >= abs(MaxLeft) + (lastcalureWeight)
									&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
									&& y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
									&& y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight))
								{    //B独在部分
									blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
								}
								else {    //A或者左边拼接好的独在部分
									blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
								}
							}
							else if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
								&& y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B独在部分
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
							}
							else {    //黑色部分
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}

				}
				else
				{
					//待拼接的图片相对于匹配的图像是往上右移动
					//但整幅图像不需要扩展高度

					if (lastcalureHeight > 0)
					{
						//该图片相对第一幅图像在上边，重叠区域为上顶面减去上偏移开始到待拼接这幅图像结束
						CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameA, _filenameB, matchVecdx, matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
						cimg_forXY(blendedImg, x, y) {
							if (x < lastblendedImg._width && y < lastblendedImg._height)
							{
								//也可更改为这样子，不将剩余的下边界做插值，但是可能会消失，效果不好
								//if (x >= (lastblendedImg._width - matchVecdx) && y >= abs(MaxUp - lastcalureHeight)
								//	&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height - abs(matchVecdy))
								if (x >= abs(MaxLeft) + (lastcalureWeight)
									&& x < abs(MaxLeft) + (lastcalureWeight)+matchVecdx
									&& y >= abs(MaxUp - lastcalureHeight) + abs(matchVecdy)
									&& y < abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //混合
									
									
									for (int k = 0;k < srcImgB._spectrum;k++)
									{
										blendedImg(x, y, 0, k) = rectImage(x - (abs(MaxLeft) + (lastcalureWeight))
											, y - (abs(MaxUp - lastcalureHeight) + abs(matchVecdy)), 0, k);
									}
									
									/*blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
										* (float)((abs(MaxLeft) + (lastcalureWeight)+matchVecdx) - x) / (float)abs(matchVecdx)
										+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 0)
										* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
									*/
								}
								else if (x >= abs(MaxLeft) + (lastcalureWeight)
									&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
									&& y >= abs(MaxUp - lastcalureHeight)
									&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B独在部分
									blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 2);
								}
								else {    //A或者左边拼接好的独在部分
									blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
								}
							}
							else if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y >= abs(MaxUp - lastcalureHeight)
								&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B独在部分
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 2);
							}
							else {    //黑色部分
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}

					else
					{
						//该图片相对第一幅图像在下边，重叠区域为下顶边减去下偏移，得出第二幅图像的结尾像素距离整幅图像的距离
						//lastblendedImg._height - abs(MaxDown - lastcalureHeight)为第二幅图像结束拼接时候的高度
						CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameA, _filenameB, matchVecdx, matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
						cimg_forXY(blendedImg, x, y) {
							if (x < lastblendedImg._width && y < lastblendedImg._height)
							{
								//也可更改为这样子，不将剩余的下边界做插值，但是可能会消失，效果不好
								//if (x >= (lastblendedImg._width - matchVecdx) && y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight)
								//-srcImgB._height && y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - matchVecdy)
								if (x >= abs(MaxLeft) + (lastcalureWeight)
									&& x < abs(MaxLeft) + (lastcalureWeight)+matchVecdx
									&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - (srcImgB._height - matchVecdy)
									&& y < blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //混合
									
									
									for (int k = 0;k < srcImgB._spectrum;k++)
									{
										blendedImg(x, y, 0, k) = rectImage(x - (abs(MaxLeft) + (lastcalureWeight))
											, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - (srcImgB._height - matchVecdy)), 0, k);
									}
									/*blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
										* (float)(abs(MaxLeft) + (lastcalureWeight)+matchVecdx - x) / (float)abs(matchVecdx)
										+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0)
										* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
									*/
									
								}
								else if (x >= abs(MaxLeft) + (lastcalureWeight)
									&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
									&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
									&& y <= blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B独在部分
									blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
								}
								else {    //A或者左边拼接好的独在部分
									blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
								}
							}
							else if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
								&& y <= blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B独在部分
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
							}
							else {    //黑色部分
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}

				}

			}
		}
		else
		{
			//该图像拼接的时候不需要从右边新增加空间
			//待拼接图像在原来的宽度中
			//判断该图像是否引起高度的变化
			if (lastcalureHeight == MaxDown || lastcalureHeight == MaxUp)
			{
				//属于整幅图像需要增加高度的部分情况，按照3-14号代码
				if (matchVecdy <= 0) {
					//该图像相对第一幅图像在下边,则lastcalureHeight一定是在下且为负数
					//并且b图像（待拼接图像）的重叠区域为总的图像高度减去b图像的高度，得到起始高度
					CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameA, _filenameB, matchVecdx, matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
					cimg_forXY(blendedImg, x, y) {
						if (x < lastblendedImg._width && y < lastblendedImg._height) {
							if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x < abs(MaxLeft) + (lastcalureWeight)+matchVecdx
								&& y >= blendedImg._height - srcImgB._height
								&& y < blendedImg._height - srcImgB._height + abs(matchVecdy)
								) {    //混合
								
								
								for (int k = 0;k < srcImgB._spectrum;k++)
								{
									blendedImg(x, y, 0, k) = rectImage(x - (abs(MaxLeft) + (lastcalureWeight))
										, y - (blendedImg._height - srcImgB._height), 0, k);
								}
								/*blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
									* (float)(abs(MaxLeft) + (lastcalureWeight)+matchVecdx - x) / (float)abs(matchVecdx)
									+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 0)
									* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
								*/

							}
							else if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y >= blendedImg._height - srcImgB._height
								&& y <= blendedImg._height
								)
							{
								//B图像在A图像中的部分不需要计算插值的部分
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 2);
							}
							else {    //A或者左边拼接好的独在部分
								blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
								blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
								blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
							}

						}
						//y部分应该是在左边图像的下边，右边
						else if (x >= abs(MaxLeft) + (lastcalureWeight)
							&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
							&& y >= blendedImg._height - srcImgB._height
							&& y < blendedImg._height) {    //B独在部分
							blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 2);
						}
						else {    //黑色部分
							blendedImg(x, y, 0, 0) = 0;
							blendedImg(x, y, 0, 1) = 0;
							blendedImg(x, y, 0, 2) = 0;
						}
					}
				}
				//3-11号修改
				else {
					//matchVec.dy > 0; 右侧图片需要往上右移动
					//该图像相对第一幅图像在上边,则lastcalureHeight一定是在下且为正数
					CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameA, _filenameB, matchVecdx, matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
					cimg_forXY(blendedImg, x, y) {
						if (x < lastblendedImg._width
							&& y >= blendedImg._height - lastblendedImg._height
							&& y < blendedImg._height)
						{
							if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x < abs(MaxLeft) + (lastcalureWeight)+matchVecdx
								&& y > matchVecdy
								&& y < srcImgB._height
								)
							{    //混合
								
								for (int k = 0;k < srcImgB._spectrum;k++)
								{
									blendedImg(x, y, 0, k) = rectImage(x - (abs(MaxLeft) + (lastcalureWeight))
										, y - (matchVecdy), 0, k);
								}

								/*blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 0)
									* (float)(abs(MaxLeft) + (lastcalureWeight)+matchVecdx - x) / (float)abs(matchVecdx)
									+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 0)
									* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
								*/
							}
							else if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y >= blendedImg._height - lastblendedImg._height
								&& y <= srcImgB._height
								)
							{
								//B图像在A图像中的部分不需要计算插值的部分
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 2);
							}

							else {    //A独在部分
								blendedImg(x, y, 0, 0) = lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 0);
								blendedImg(x, y, 0, 1) = lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 1);
								blendedImg(x, y, 0, 2) = lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 2);
							}
						}
						else if (
							x >= abs(MaxLeft) + (lastcalureWeight)
							&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
							&& y >= blendedImg._height - lastblendedImg._height
							&& y <= srcImgB._height
							) {    //B独在部分
							blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 2);
						}
						else {    //黑色部分
							blendedImg(x, y, 0, 0) = 0;
							blendedImg(x, y, 0, 1) = 0;
							blendedImg(x, y, 0, 2) = 0;
						}
					}
				}
			}
			else
			{
				//属于不需要增加高度的部分，按照3-14号代码另行处理
				if (matchVecdy <= 0)
				{
					//待拼接的图片相对于匹配的图像是往下右移动
					//但整幅图像不需要扩展高度
					if (lastcalureHeight > 0)
					{
						//该图片相对第一幅图像在上边，重叠区域为上顶面减去上偏移开始到待拼接这幅图像结束
						CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameA, _filenameB, matchVecdx, matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
						cimg_forXY(blendedImg, x, y) {
							if (x < lastblendedImg._width && y < lastblendedImg._height)
							{
								//也可更改为这样子，不将剩余的下边界做插值，但是可能会消失，效果不好
								//if (x >= (lastblendedImg._width - matchVecdx) && y >= abs(MaxUp - lastcalureHeight)
								//	&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height - abs(matchVecdy))
								if (x >= abs(MaxLeft) + (lastcalureWeight)
									&& x < abs(MaxLeft) + (lastcalureWeight)+matchVecdx
									&& y >= abs(MaxUp - lastcalureHeight)
									&& y < abs(MaxUp - lastcalureHeight) + srcImgB._height - abs(matchVecdy)) {    //混合
									
									for (int k = 0;k < srcImgB._spectrum;k++)
									{
										blendedImg(x, y, 0, k) = rectImage(x - (abs(MaxLeft) + (lastcalureWeight))
											, y - (abs(MaxUp - lastcalureHeight)), 0, k);
									}
									/*blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
										* (float)(abs(MaxLeft) + (lastcalureWeight)+matchVecdx - x) / (float)abs(matchVecdx)
										+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 0)
										* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
									*/

								}
								else if (x >= abs(MaxLeft) + (lastcalureWeight)
									&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
									&& y >= abs(MaxUp - lastcalureHeight)
									&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height
									)
								{
									blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 2);
								}
								else {    //A或者左边拼接好的独在部分
									blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
								}
							}
							else if ((x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y >= abs(MaxUp - lastcalureHeight)
								&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height)) {    //B独在部分
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 2);
							}
							else {    //黑色部分
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}

					else
					{
						//该图片相对第一幅图像在下边，重叠区域为下顶边减去下偏移，得出第二幅图像的结尾像素距离整幅图像的距离
						//lastblendedImg._height - abs(MaxDown - lastcalureHeight)为第二幅图像结束拼接时候的高度
						CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameA, _filenameB, matchVecdx, matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
						cimg_forXY(blendedImg, x, y) {
							if (x < lastblendedImg._width && y < lastblendedImg._height)
							{
								//也可更改为这样子，不将剩余的下边界做插值，但是可能会消失，效果不好
								//if (x >= (lastblendedImg._width - matchVecdx) && y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight)
								//-srcImgB._height && y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - matchVecdy)
								if (x >= abs(MaxLeft) + (lastcalureWeight)
									&& x < abs(MaxLeft) + (lastcalureWeight)+matchVecdx
									&& y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
									&& y < lastblendedImg._height - abs(MaxDown - lastcalureHeight) - abs(matchVecdy))
								{    //混合
									
									for (int k = 0;k < srcImgB._spectrum;k++)
									{
										blendedImg(x, y, 0, k) = rectImage(x - (abs(MaxLeft) + (lastcalureWeight))
											, y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
									}

									/*blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
										* (float)((abs(MaxLeft) + (lastcalureWeight)+matchVecdx) - x) / (float)abs(matchVecdx)
										+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0)
										* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
									*/

								}
								else if (x >= abs(MaxLeft) + (lastcalureWeight)
									&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
									&& y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
									&& y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight)
									)
								{
									blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
								}
								else {    //A或者左边拼接好的独在部分
									blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
								}
							}
							else if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
								&& y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight)
								)
							{	//b区域的值
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
							}
							else {    //黑色部分
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}

				}
				else
				{
					//待拼接的图片相对于匹配的图像是往上右移动
					//但整幅图像不需要扩展高度

					if (lastcalureHeight > 0)
					{
						//该图片相对第一幅图像在上边，重叠区域为上顶面减去上偏移开始到待拼接这幅图像结束
						CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameA, _filenameB, matchVecdx, matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
						cimg_forXY(blendedImg, x, y) {
							if (x < lastblendedImg._width && y < lastblendedImg._height)
							{

								if (x >= abs(MaxLeft) + (lastcalureWeight)
									&& x < abs(MaxLeft) + (lastcalureWeight)+matchVecdx
									&& y >= abs(MaxUp - lastcalureHeight) + abs(matchVecdy)
									&& y < abs(MaxUp - lastcalureHeight) + srcImgB._height
									) {    //混合

									
									for (int k = 0;k < srcImgB._spectrum;k++)
									{
										blendedImg(x, y, 0, k) = rectImage(x - (abs(MaxLeft) + (lastcalureWeight))
											, y - (abs(MaxUp - lastcalureHeight) + abs(matchVecdy)), 0, k);
									}
									/*blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
										* (float)((abs(MaxLeft) + (lastcalureWeight)+matchVecdx) - x) / (float)abs(matchVecdx)
										+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 0)
										* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);
									*/

								}
								else if (x >= abs(MaxLeft) + (lastcalureWeight)
									&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
									&& y >= abs(MaxUp - lastcalureHeight)
									&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height
									)
								{
									blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 2);
								}
								else {    //A或者左边拼接好的独在部分
									blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
								}
							}
							else if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y >= abs(MaxUp - lastcalureHeight)
								&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height
								)
							{
								//B独在的部分
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 2);
							}
							else {    //黑色部分
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}

					else
					{
						//该图片相对第一幅图像在下边，重叠区域为下顶边减去下偏移，得出第二幅图像的结尾像素距离整幅图像的距离
						//lastblendedImg._height - abs(MaxDown - lastcalureHeight)为第二幅图像结束拼接时候的高度
						CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameA, _filenameB, matchVecdx, matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
						cimg_forXY(blendedImg, x, y) {
							if (x < lastblendedImg._width && y < lastblendedImg._height)
							{
								//也可更改为这样子，不将剩余的下边界做插值，但是可能会消失，效果不好
								//if (x >= (lastblendedImg._width - matchVecdx) && y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight)
								//-srcImgB._height && y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - matchVecdy)
								if (x >= abs(MaxLeft) + (lastcalureWeight)
									&& x < abs(MaxLeft) + (lastcalureWeight)+matchVecdx
									&& y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - abs(matchVecdy)
									&& y < lastblendedImg._height - abs(MaxDown - lastcalureHeight))
								{    //混合
									
									
									for (int k = 0;k < srcImgB._spectrum;k++)
									{
										blendedImg(x, y, 0, k) = rectImage(x - (abs(MaxLeft) + (lastcalureWeight))
											, y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - abs(matchVecdy)), 0, k);
									}

									/*blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
										* (float)((abs(MaxLeft) + (lastcalureWeight)+matchVecdx) - x) / (float)abs(matchVecdx)
										+ (float)srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0)
										* (float)(x - (abs(MaxLeft) + (lastcalureWeight))) / (float)abs(matchVecdx);*/
									
								}
								else if (x >= abs(MaxLeft) + (lastcalureWeight)
									&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
									&& y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
									&& y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight)
									)
								{
									blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
								}
								else {    //A或者左边拼接好的独在部分
									blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
								}
							}
							else if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
								&& y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight)
								)
							{	//B部分数值附加进入
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
							}
							else {    //黑色部分
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}

				}

			}

		}


	}
	blendedImg = blendedImg.normalize(0, 255);

	blendedImg.display("blendedImg");

	if (StitchPictureNum == 0)
		StitchPictureNum = 2;
	else
		StitchPictureNum++;
	blendedImg.save((char*)"test1.bmp");
	return blendedImg;

}


/*
void MyMorePictureStitch::blendingMainProcess(char* _filenameA, char* _filenameB, CImg<int> lastblendedImg) {

	//--------------------------------------------------------------
	//-------这一部分代码可以进行重写修改拼接过程
	//进行修改,根据传入的图片名字自动选择图片的计算

	CImg<int> srcImgA, srcImgB;
	//修改为加载图片
	//srcImgA.load_bmp(_filenameA);
	//srcImgB.load_bmp(_filenameB);
	srcImgA.load(_filenameA);
	srcImgB.load(_filenameB);

	//判断该图像待拼接图像是否在右边，是的话坐上记录，并且进行图像扩展的调整
	if (matchVec.dx > srcImgB._width) 
	{
		//洗的记录值进行登记替换
		//待真实的 = （拼接图像 - dx/2）*2；
		//可以分析纸2
		int leftmatchVec = (srcImgB._width - matchVec.dx / 2) * 2;
	}
	else
	{



		//-----------------------合并的窗口大小位置-----------------------------------------
		//dx为两幅图像的特征点之间的横的差值
		//dy为两幅图像的特征点之间的竖的差值
		blendedImg = CImg<int>(srcImgA._width + srcImgB._width - matchVec.dx,
			srcImgA._height + abs(matchVec.dy), 1, 3, 0);
	}
	cimg_forXY(blendedImg, x, y) {
		if (matchVec.dy <= 0) {    //右侧图片需要往下左移动
			if (x < srcImgA._width && y < srcImgA._height) {
				if (x >= (srcImgA._width - matchVec.dx) && y >= (0 - matchVec.dy)) {    //混合
					blendedImg(x, y, 0, 0) = (float)srcImgA(x, y, 0, 0)
						* (float)(srcImgA._width - x) / (float)abs(matchVec.dx)
						+ (float)srcImgB(x - (srcImgA._width - matchVec.dx), y - (0 - matchVec.dy), 0, 0)
						* (float)(x - (srcImgA._width - matchVec.dx)) / (float)abs(matchVec.dx);
					blendedImg(x, y, 0, 1) = (float)srcImgA(x, y, 0, 1)
						* (float)(srcImgA._width - x) / (float)abs(matchVec.dx)
						+ (float)srcImgB(x - (srcImgA._width - matchVec.dx), y - (0 - matchVec.dy), 0, 1)
						* (float)(x - (srcImgA._width - matchVec.dx)) / (float)abs(matchVec.dx);
					blendedImg(x, y, 0, 2) = (float)srcImgA(x, y, 0, 2)
						* (float)(srcImgA._width - x) / (float)abs(matchVec.dx)
						+ (float)srcImgB(x - (srcImgA._width - matchVec.dx), y - (0 - matchVec.dy), 0, 2)
						* (float)(x - (srcImgA._width - matchVec.dx)) / (float)abs(matchVec.dx);
				}
				else {    //A独在部分
					blendedImg(x, y, 0, 0) = srcImgA(x, y, 0, 0);
					blendedImg(x, y, 0, 1) = srcImgA(x, y, 0, 1);
					blendedImg(x, y, 0, 2) = srcImgA(x, y, 0, 2);
				}
			}
			else if (x >= (srcImgA._width - matchVec.dx)
				&& y >= (0 - matchVec.dy) && y < (0 - matchVec.dy) + srcImgB._height) {    //B独在部分
				blendedImg(x, y, 0, 0) = srcImgB(x - (srcImgA._width - matchVec.dx), y - (0 - matchVec.dy), 0, 0);
				blendedImg(x, y, 0, 1) = srcImgB(x - (srcImgA._width - matchVec.dx), y - (0 - matchVec.dy), 0, 1);
				blendedImg(x, y, 0, 2) = srcImgB(x - (srcImgA._width - matchVec.dx), y - (0 - matchVec.dy), 0, 2);
			}
			else {    //黑色部分
				blendedImg(x, y, 0, 0) = 0;
				blendedImg(x, y, 0, 1) = 0;
				blendedImg(x, y, 0, 2) = 0;
			}
		}
		else {    //matchVec.dy > 0; 右侧图片需要往上左移动
			if (x < srcImgA._width && y >= matchVec.dy) {
				if (x >= (srcImgA._width - matchVec.dx) && y < srcImgB._height) {    //混合
					blendedImg(x, y, 0, 0) = (float)srcImgA(x, y - matchVec.dy, 0, 0)
						* (float)(srcImgA._width - x) / (float)abs(matchVec.dx)
						+ (float)srcImgB(x - (srcImgA._width - matchVec.dx), y, 0, 0)
						* (float)(x - (srcImgA._width - matchVec.dx)) / (float)abs(matchVec.dx);
					blendedImg(x, y, 0, 1) = (float)srcImgA(x, y - matchVec.dy, 0, 1)
						* (float)(srcImgA._width - x) / (float)abs(matchVec.dx)
						+ (float)srcImgB(x - (srcImgA._width - matchVec.dx), y, 0, 1)
						* (float)(x - (srcImgA._width - matchVec.dx)) / (float)abs(matchVec.dx);
					blendedImg(x, y, 0, 2) = (float)srcImgA(x, y - matchVec.dy, 0, 2)
						* (float)(srcImgA._width - x) / (float)abs(matchVec.dx)
						+ (float)srcImgB(x - (srcImgA._width - matchVec.dx), y, 0, 2)
						* (float)(x - (srcImgA._width - matchVec.dx)) / (float)abs(matchVec.dx);
				}
				else {    //A独在部分
					blendedImg(x, y, 0, 0) = srcImgA(x, y - matchVec.dy, 0, 0);
					blendedImg(x, y, 0, 1) = srcImgA(x, y - matchVec.dy, 0, 1);
					blendedImg(x, y, 0, 2) = srcImgA(x, y - matchVec.dy, 0, 2);
				}
			}
			else if (x >= (srcImgA._width - matchVec.dx) && y < srcImgB._height) {    //B独在部分
				blendedImg(x, y, 0, 0) = srcImgB(x - (srcImgA._width - matchVec.dx), y, 0, 0);
				blendedImg(x, y, 0, 1) = srcImgB(x - (srcImgA._width - matchVec.dx), y, 0, 1);
				blendedImg(x, y, 0, 2) = srcImgB(x - (srcImgA._width - matchVec.dx), y, 0, 2);
			}
			else {    //黑色部分
				blendedImg(x, y, 0, 0) = 0;
				blendedImg(x, y, 0, 1) = 0;
				blendedImg(x, y, 0, 2) = 0;
			}
		}
	}
	blendedImg.display("blendedImg");

	//记录拼接的数量增加
	if (StitchPictureNum == 0)
		StitchPictureNum = 2;
	else
		StitchPictureNum++;

}
*/
//----------2019-4-17整合-----------
//1.先将函数进行重载，判读其选择是直接渐入渐出融合还是进行最佳缝合线多分辨率融合
//默认设置，渐入渐出融合

//2.修改多分辨率融合部分代码，提高代码可读性
CImg<int> MyMorePictureStitch::MyStitchImage4(char * _filenameA, char * _filenameB, CImg<int> lastblendedImg, int matchVecdx, int matchVecdy, char * StitchType, vector<char*> Other)
{
	if (strcmp(StitchType, (char*)"GradualFusionBlend") == 0)
	{
		return MyStitchImageByGradualFusionBlend(_filenameA, _filenameB, lastblendedImg, matchVecdx, matchVecdy);
	}
	else if(strcmp(StitchType, (char*)"OptimalSeamBlend") == 0)
		return MyStitchImageByOptimalSeamBlend(_filenameA, _filenameB, lastblendedImg, matchVecdx, matchVecdy, Other);
}


CImg<int> MyMorePictureStitch::MyStitchImage4(char * _filenameA, char * _filenameB, CImg<int> lastblendedImg, int matchVecdx, int matchVecdy, char * StitchType)
{
	vector<char*> Others;
	Others.push_back((char*)"no thing exist") ;
	return MyStitchImage4(_filenameA, _filenameB, lastblendedImg, matchVecdx, matchVecdy, StitchType, Others);
}

CImg<int> MyMorePictureStitch::MyStitchImage4(char * _filenameA, char * _filenameB, CImg<int> lastblendedImg, int matchVecdx, int matchVecdy)
{
	char* StitchType = (char*)"GradualFusionBlend";
	vector<char*> Others;
	
	Others.push_back((char*)"no thing exist");
	return MyStitchImage4(_filenameA, _filenameB, lastblendedImg, matchVecdx, matchVecdy, StitchType, Others);
}

void MyMorePictureStitch::saveBlendedImg(char* blendedImgAddr) {
	blendedImg.save(blendedImgAddr);
}

//放回CImg的格式的拼接后图像
CImg<int> MyMorePictureStitch::getBlendedImg() {
	return blendedImg;
}