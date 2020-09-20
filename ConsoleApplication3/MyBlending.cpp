#include "MyBlending.h"

MyBlending::MyBlending() {
}

MyBlending::~MyBlending() {
}

MyBlending::MyBlending(int sx, int sy) {
	matchVec.dx = sx;
	matchVec.dy = sy;
}


void MyBlending::blendingMainProcess(char* _filenameA, char* _filenameB) {

	//--------------------------------------------------------------
	//-------这一部分代码可以进行重写修改拼接过程
	//进行修改,根据传入的图片名字自动选择图片的计算
	string tempfilenameA = _filenameA;
	string tempfilenameB = _filenameB;

	printf("%d\n", _filenameA);
	printf("%d\n",&tempfilenameA);
	//修改为加载图片
	//srcImgA.load_bmp(_filenameA);
	//srcImgB.load_bmp(_filenameB);
	srcImgA.load(_filenameA);
	srcImgB.load(_filenameB);


	//-----------------------合并的窗口大小位置-----------------------------------------
	//dx为两幅图像的特征点之间的横的差值
	//dy为两幅图像的特征点之间的竖的差值
	blendedImg = CImg<int>(srcImgA._width + srcImgB._width - matchVec.dx, 
		srcImgA._height + abs(matchVec.dy), 1, 3, 0);

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
}


void MyBlending::saveBlendedImg(char* blendedImgAddr) {
	blendedImg.save(blendedImgAddr);
}

//放回CImg的格式的拼接后图像
CImg<int> MyBlending::getBlendedImg() {
	return blendedImg;
}