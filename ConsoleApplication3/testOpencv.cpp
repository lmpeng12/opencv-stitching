#include "testOpencv.h"




testOpencv::testOpencv()
{
}

char* testOpencv::paststr(char * leftImage, char * rightImage, char * templateImage)
{
	//MyLaplacianBlending testlap = MyLaplacianBlending();
	//char* myreplacianoperatorImage = testlap.getLaplacianBlendRectImage(leftImage, rightImage, templateImage);
	//return myreplacianoperatorImage;
	return (char*)"null1";
}


testOpencv::~testOpencv()
{
}


//�²���ԭ����ͼ�񣬷�����С2���ߴ��ͼ��  
CvMat * MyhalfSizeImage(CvMat * im) {
	unsigned int i, j;
	int w = im->cols / 2;
	int h = im->rows / 2;
	CvMat *imnew = cvCreateMat(h, w, CV_32FC1);

#define Im(ROW,COL) ((float *)(im->data.fl + im->step/sizeof(float) *(ROW)))[(COL)]  
#define Imnew(ROW,COL) ((float *)(imnew->data.fl + imnew->step/sizeof(float) *(ROW)))[(COL)]  
	for (j = 0; j < h; j++)
		for (i = 0; i < w; i++)
			Imnew(j, i) = Im(j * 2, i * 2);
	return imnew;
}

//�ϲ���ԭ����ͼ�񣬷��طŴ�2���ߴ��ͼ��  
CvMat * doubleSizeImage(CvMat * im) {
	unsigned int i, j;
	int w = im->cols * 2;
	int h = im->rows * 2;
	CvMat *imnew = cvCreateMat(h, w, CV_32FC1);

#define Im(ROW,COL) ((float *)(im->data.fl + im->step/sizeof(float) *(ROW)))[(COL)]  
#define Imnew(ROW,COL) ((float *)(imnew->data.fl + imnew->step/sizeof(float) *(ROW)))[(COL)]  

	for (j = 0; j < h; j++)
		for (i = 0; i < w; i++)
			Imnew(j, i) = Im(j / 2, i / 2);

	return imnew;
}

//�ϲ���ԭ����ͼ�񣬷��طŴ�2���ߴ�����Բ�ֵͼ��  
CvMat * doubleSizeImage2(CvMat * im) {
	unsigned int i, j;
	int w = im->cols * 2;
	int h = im->rows * 2;
	CvMat *imnew = cvCreateMat(h, w, CV_32FC1);

#define Im(ROW,COL) ((float *)(im->data.fl + im->step/sizeof(float) *(ROW)))[(COL)]  
#define Imnew(ROW,COL) ((float *)(imnew->data.fl + imnew->step/sizeof(float) *(ROW)))[(COL)]  

	// fill every pixel so we don't have to worry about skipping pixels later  
	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++) {
			Imnew(j, i) = Im(j / 2, i / 2);
		}
	}
	/*
	A B C
	E F G
	H I J
	pixels A C H J are pixels from original image
	pixels B E G I F are interpolated pixels
	*/
	// interpolate pixels B and I  
	for (j = 0; j < h; j += 2)
		for (i = 1; i < w - 1; i += 2)
			Imnew(j, i) = 0.5*(Im(j / 2, i / 2) + Im(j / 2, i / 2 + 1));
	// interpolate pixels E and G  
	for (j = 1; j < h - 1; j += 2)
		for (i = 0; i < w; i += 2)
			Imnew(j, i) = 0.5*(Im(j / 2, i / 2) + Im(j / 2 + 1, i / 2));
	// interpolate pixel F  
	for (j = 1; j < h - 1; j += 2)
		for (i = 1; i < w - 1; i += 2)
			Imnew(j, i) = 0.25*(Im(j / 2, i / 2) + Im(j / 2 + 1, i / 2) + Im(j / 2, i / 2 + 1) + Im(j / 2 + 1, i / 2 + 1));
	return imnew;
}

//��˹�˲�����
void GuassFilter(CvMat *pGrayMat, CvMat* pFilterMat, int nWidth, int nHeight, double dSigma)
{
	////////////////////////����˵��///////////////////////////////////
	//pGrayMat:������ͼ������
	//pFilterMat:�����˹�˲����
	//nWidth:ͼ����
	//nHeight:ͼ��߶�
	//dSigma:��˹�˲�����������
	int nWidowSize = (int)(1 + 2 * ceil(3 * dSigma));  //�����˲����ڵĴ�С
	int nCenter = (nWidowSize) / 2;                //�����˲��������ĵ�����
	//���ɶ�ά�ĸ�˹�˲�ϵ��
	double* pdKernal = new double[nWidowSize*nWidowSize]; //����һά��˹������
	double  dSum = 0.0;	                                  //��ͣ����й�һ��	
	/////////////��ά��˹������ʽ//////////////////////     
	//	                         x*x+y*y        ///////
	//	                   -1*--------------	///////
	//          1               2*Sigma*Sigma	///////
	//   ---------------- e	///////
	//   2*pi*Sigma*Sigma	///////
	///////////////////////////////////////////////////
	for (int i = 0; i < nWidowSize; i++)
	{
		for (int j = 0; j < nWidowSize; j++)
		{
			int nDis_x = i - nCenter;
			int nDis_y = j - nCenter;
			pdKernal[i + j * nWidowSize] = exp(-(1 / 2)*(nDis_x*nDis_x + nDis_y * nDis_y)
				/ (dSigma*dSigma)) / (2 * 3.1415926*dSigma*dSigma);
			dSum += pdKernal[i + j * nWidowSize];
		}
	}
	//���й�һ��
	for (i = 0; i < nWidowSize; i++)
	{
		for (int j = 0; j < nWidowSize; j++)
		{
			pdKernal[i + j * nWidowSize] /= dSum;
		}
	}
	for (i = 0; i < nHeight; i++)
	{
		for (int j = 0; j < nWidth; j++)
		{
			double dFilter = 0.0;
			double dSum = 0.0;
			for (int x = (-nCenter); x <= nCenter; x++)	//��
			{
				for (int y = (-nCenter); y <= nCenter; y++)	//��
				{
					if ((j + x) >= 0 && (j + x) < nWidth && (i + y) >= 0 && (i + y) < nHeight)	//�жϱ�Ե
					{
						double ImageData = cvmGet(pGrayMat, i + y, j + x);
						dFilter += ImageData * pdKernal[(y + nCenter)*nWidowSize + (x + nCenter)];
						dSum += pdKernal[(y + nCenter)*nWidowSize + (x + nCenter)];
					}
				}
			}
			cvmSet(pFilterMat, i, j, dFilter / dSum);
		}
	}
	delete[]pdKernal;
}


