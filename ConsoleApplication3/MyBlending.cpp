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
	//-------��һ���ִ�����Խ�����д�޸�ƴ�ӹ���
	//�����޸�,���ݴ����ͼƬ�����Զ�ѡ��ͼƬ�ļ���
	string tempfilenameA = _filenameA;
	string tempfilenameB = _filenameB;

	printf("%d\n", _filenameA);
	printf("%d\n",&tempfilenameA);
	//�޸�Ϊ����ͼƬ
	//srcImgA.load_bmp(_filenameA);
	//srcImgB.load_bmp(_filenameB);
	srcImgA.load(_filenameA);
	srcImgB.load(_filenameB);


	//-----------------------�ϲ��Ĵ��ڴ�Сλ��-----------------------------------------
	//dxΪ����ͼ���������֮��ĺ�Ĳ�ֵ
	//dyΪ����ͼ���������֮������Ĳ�ֵ
	blendedImg = CImg<int>(srcImgA._width + srcImgB._width - matchVec.dx, 
		srcImgA._height + abs(matchVec.dy), 1, 3, 0);

	cimg_forXY(blendedImg, x, y) {
		if (matchVec.dy <= 0) {    //�Ҳ�ͼƬ��Ҫ�������ƶ�
			if (x < srcImgA._width && y < srcImgA._height) {
				if (x >= (srcImgA._width - matchVec.dx) && y >= (0 - matchVec.dy)) {    //���
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
				else {    //A���ڲ���
					blendedImg(x, y, 0, 0) = srcImgA(x, y, 0, 0);
					blendedImg(x, y, 0, 1) = srcImgA(x, y, 0, 1);
					blendedImg(x, y, 0, 2) = srcImgA(x, y, 0, 2);
				}
			}
			else if (x >= (srcImgA._width - matchVec.dx) 
				&& y >= (0 - matchVec.dy) && y < (0 - matchVec.dy) + srcImgB._height) {    //B���ڲ���
				blendedImg(x, y, 0, 0) = srcImgB(x - (srcImgA._width - matchVec.dx), y - (0 - matchVec.dy), 0, 0);
				blendedImg(x, y, 0, 1) = srcImgB(x - (srcImgA._width - matchVec.dx), y - (0 - matchVec.dy), 0, 1);
				blendedImg(x, y, 0, 2) = srcImgB(x - (srcImgA._width - matchVec.dx), y - (0 - matchVec.dy), 0, 2);
			}
			else {    //��ɫ����
				blendedImg(x, y, 0, 0) = 0;
				blendedImg(x, y, 0, 1) = 0;
				blendedImg(x, y, 0, 2) = 0;
			}
		}
		else {    //matchVec.dy > 0; �Ҳ�ͼƬ��Ҫ�������ƶ�
			if (x < srcImgA._width && y >= matchVec.dy) {
				if (x >= (srcImgA._width - matchVec.dx) && y < srcImgB._height) {    //���
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
				else {    //A���ڲ���
					blendedImg(x, y, 0, 0) = srcImgA(x, y - matchVec.dy, 0, 0);
					blendedImg(x, y, 0, 1) = srcImgA(x, y - matchVec.dy, 0, 1);
					blendedImg(x, y, 0, 2) = srcImgA(x, y - matchVec.dy, 0, 2);
				}
			}
			else if (x >= (srcImgA._width - matchVec.dx) && y < srcImgB._height) {    //B���ڲ���
				blendedImg(x, y, 0, 0) = srcImgB(x - (srcImgA._width - matchVec.dx), y, 0, 0);
				blendedImg(x, y, 0, 1) = srcImgB(x - (srcImgA._width - matchVec.dx), y, 0, 1);
				blendedImg(x, y, 0, 2) = srcImgB(x - (srcImgA._width - matchVec.dx), y, 0, 2);
			}
			else {    //��ɫ����
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

//�Ż�CImg�ĸ�ʽ��ƴ�Ӻ�ͼ��
CImg<int> MyBlending::getBlendedImg() {
	return blendedImg;
}