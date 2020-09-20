#include "MyLaplacianBlending.h"

// g++ main.cpp -o main.out -L/opt/X11/lib -lX11 -pthread
#ifndef debug
MyLaplacianBlending::MyLaplacianBlending() {}
MyLaplacianBlending::~MyLaplacianBlending() {}
CImg<double> MyLaplacianBlending::MyLaplacianBlend(std::string leftImageString, std::string rightImageString, std::string maskNameString)
{
	// Interation with user
	std::string maskString;
	std::vector<std::string> images;

	images.push_back(leftImageString);
	images.push_back(rightImageString);
	maskString = maskNameString;
	const int pyramidSize = 3;

	std::vector<std::vector<CImg <double> > > pyramidsP;
	std::vector<std::vector<CImg <double> > > pyramidsG;

	GaussianPyramid gPyramid;
	double filter[5] = { 1.0 / 16, 4.0 / 16, 6.0 / 16, 4.0 / 16, 1.0 / 16 };
	gPyramid.generateFilter(filter);

	LaplacianPyramid lPyramid;
	lPyramid.generateFilter(filter);

	for (int i = 0; i < images.size(); i++) {
		const char *const imagePath = images[i].c_str();
		CImg<double> image(imagePath);

		std::vector<CImg<double> > gaussianPyramids;
		std::vector<CImg<double> > laplacianPyramids;

		gaussianPyramids.push_back(image);
		CImg<double> reducedImage = image;
		for (int p = 1; p < pyramidSize; p++) {
			CImg<double> imageGenerated = gPyramid.reduce(reducedImage);
			gaussianPyramids.push_back(imageGenerated);
			reducedImage = imageGenerated;
		}

		for (int p = 1; p < pyramidSize; p++) {
			int w = (gaussianPyramids[p - 1].width() - 2 * gaussianPyramids[p].width());
			int h = (gaussianPyramids[p - 1].height() - 2 * gaussianPyramids[p].height());
			laplacianPyramids.push_back(gaussianPyramids[p - 1] - 4 * lPyramid.expand(gaussianPyramids[p], w, h));
		}
		laplacianPyramids.push_back(gaussianPyramids[pyramidSize - 1]);

		pyramidsG.push_back(gaussianPyramids);
		pyramidsP.push_back(laplacianPyramids);

		// // Collapsing Original Image
		// int w = (gaussianPyramids[pyramidSize - 2].width() - 2*gaussianPyramids[pyramidSize - 1].width());
		// int h = (gaussianPyramids[pyramidSize - 2].height() - 2*gaussianPyramids[pyramidSize - 1].height());
		// CImg<double> collapseImage = laplacianPyramids[pyramidSize - 2] + 4*lPyramid.expand(laplacianPyramids[pyramidSize - 1],w,h);
		// for (int a = pyramidSize - 3; a >= 0; a--){
		// 	w = (laplacianPyramids[a].width() - 2*collapseImage.width());
		// 	h = (laplacianPyramids[a].height() - 2*collapseImage.height());
		// 	collapseImage = laplacianPyramids[a] + 4*lPyramid.expand(collapseImage,w,h);
		// }
		// collapseImage.display();
	}
	cout << "�����˹��������������˹�ںϲ���" << endl;
	// Creating Mask Gaussian Pyramid
	const char *const maskPath = maskString.c_str();
	CImg<double> image(maskPath);
	std::vector<CImg<double> > maskGaussianPyramid;

	maskGaussianPyramid.push_back(image);
	CImg<double> reducedImage = image;
	for (int p = 1; p < pyramidSize; p++) {
		CImg<double> maskReduced = gPyramid.reduce(reducedImage);
		maskGaussianPyramid.push_back(maskReduced);
		reducedImage = maskReduced;
	}

	// Blending images
	std::vector< CImg<double> > newLaplacianPyramid;
	for (int i = 0; i < pyramidSize; i++) {
		CImg<double> lsImageLaplacian(pyramidsP[0][i].width(), pyramidsP[0][i].height(), 1, pyramidsP[0][i].spectrum(), 0);
		for (int canal = 0; canal < pyramidsP[0][i].spectrum(); canal++) {
			for (int x = 0; x < pyramidsP[0][i].width(); x++) {
				for (int y = 0; y < pyramidsP[0][i].height(); y++) {
					lsImageLaplacian(x, y, 0, canal) = ((maskGaussianPyramid[i](x, y, 0, 0) / 255) * (pyramidsP[0][i](x, y, 0, canal)))
						+ ((1 - (maskGaussianPyramid[i](x, y, 0, 0) / 255))*(pyramidsP[1][i](x, y, 0, canal)));
				}
			}
		}
		newLaplacianPyramid.push_back(lsImageLaplacian);
	}

	cout << "�����ںϲ���" << endl;
	int w = (newLaplacianPyramid[pyramidSize - 2].width() - 2 * newLaplacianPyramid[pyramidSize - 1].width());
	int h = (newLaplacianPyramid[pyramidSize - 2].height() - 2 * newLaplacianPyramid[pyramidSize - 1].height());
	CImg<double> collapsedImage = newLaplacianPyramid[pyramidSize - 2] + 4 * lPyramid.expand(newLaplacianPyramid[pyramidSize - 1], w, h);
	for (int i = pyramidSize - 3; i >= 0; i--) {
		w = (newLaplacianPyramid[i].width() - 2 * collapsedImage.width());
		h = (newLaplacianPyramid[i].height() - 2 * collapsedImage.height());
		collapsedImage = newLaplacianPyramid[i] + 4 * lPyramid.expand(collapsedImage, w, h);
	}

	// collapsedImage = lPyramid.filterImage(collapsedImage);;
	// collapsedImage.blur(0.7).display();
	
	//����ʾ
	//collapsedImage.display();
	//collapsedImage.normalize(0, 255).save("result.bmp");

	return collapsedImage.normalize(0, 255);
}


#endif // !debug


#ifdef OpencvDEBUGType



#include "opencv2/opencv.hpp"
using namespace cv;

//openCV�ο�����
//opencv��ͼ���x,y �����Լ� height, width,rows,cols ���ǵĹ�ϵ����������
//rows ��ʵ�����У�һ��һ��Ҳ����y ����height�߶�Ҳ����y����
//cols  Ҳ�����У�һ��һ��Ҳ����x����width���Ҳ����x����

#ifndef _debug
Mat_<Vec3f> LaplacianBlend(const Mat_<Vec3f>& l, const Mat_<Vec3f>& r, const Mat_<float>& m) {
	MyLaplacianBlending lb(l, r, m, 5);
	Mat_<Vec3f> returnImage = lb.blend();
	//�ͷ��ڴ�
	//���ܻ���ִ���
	//�����ڴ�����ͽ���������ɾ��
	lb.~MyLaplacianBlending();
	return returnImage;
}



#endif // !_debug

#ifndef __DEBUG

//2019-4-11 ���и����޸Ĵ���
//��û��ڶ�ֱ��ʣ���Ƶ�ף�ͼ���ں�
//�������������ص�����ͼ���ұ��ص�ͼ�񣬷��������Һ�ͼ��
char* MyLaplacianBlending::getLaplacianBlendRectImage(char* leftImage, char* rightImage, char* templateImage)
{
	
	//˼·����ȡͼ��
	//�ش���ʱ���Ǳ�����ͼ������
	Mat l8u = imread(leftImage);
	Mat r8u = imread(rightImage);
	Mat C = imread(templateImage);
	
#ifdef CImg���
	//ͼ���� ------>ͼ�������
	//ͼ��߶� ------>ͼ�������
	
int ImgWeight, ImgHeight;
	ImgWeight = leftImage._width;
	ImgHeight = leftImage._height;

	Mat l8u(ImgHeight, ImgWeight, CV_32FC3, Scalar(0, 0, 0));
	Mat r8u(ImgHeight, ImgWeight, CV_32FC3, Scalar(0, 0, 0));
	Mat C(ImgHeight, ImgWeight, CV_32FC3, Scalar(0, 0, 0));
	
	//���´�����ֵ����
	for(int i = 0;i<ImgHeight;i++)
		for (int j = 0;j < ImgWeight;j++) 
		{
			//���к��н��в���
			//��ߵ�ͼ��
			//��Ч�ʽ����������ٲ���ָ���㱼~~~~~~~~~~~~
			//����opencv��bgr���ø���˳�򣬵��ǽ���ʹ�ò���ֻص�ԭ������rgb->bgr->rgb

			l8u.at<cv::Vec3b>(i, j)[0] = leftImage(j, i, 0, 0);
			l8u.at<cv::Vec3b>(i, j)[1] = leftImage(j, i, 0, 1);
			l8u.at<cv::Vec3b>(i, j)[2] = leftImage(j, i, 0, 2);

			//�ұߵ�ͼ��
			r8u.at<cv::Vec3b>(i, j)[0] = rightImage(j, i, 0, 0);
			r8u.at<cv::Vec3b>(i, j)[1] = rightImage(j, i, 0, 1);
			r8u.at<cv::Vec3b>(i, j)[2] = rightImage(j, i, 0, 2);

			//�հ׵�ͼ��

			C.at<cv::Vec3b>(i, j)[0] = templateImage(j, i, 0, 0);
			C.at<cv::Vec3b>(i, j)[1] = templateImage(j, i, 0, 1);
			C.at<cv::Vec3b>(i, j)[2] = templateImage(j, i, 0, 2);
		}
	
	//imshow("left", l8u);
	//imshow("right", r8u);
	
	
	//�������
	CImg<int> AfterLaplacianBlendImage = CImg<int>(ImgWeight, ImgHeight, 1, 3, 0);

	//���¸�ֵ���ؿ���ʹ�õĲ���
	for (int i = 0;i < ImgHeight;i++)
		for (int j = 0;j < ImgWeight;j++)
		{
			AfterLaplacianBlendImage(j, i, 0, 0) = blend.at<cv::Vec3b>(i, j)[0];
			AfterLaplacianBlendImage(j, i, 0, 1) = blend.at<cv::Vec3b>(i, j)[1];
			AfterLaplacianBlendImage(j, i, 0, 2) = blend.at<cv::Vec3b>(i, j)[2];
		}

	return AfterLaplacianBlendImage;
#endif // !CImg���

	Mat_<Vec3f> l;
	l8u.convertTo(l, CV_32F, 1.0 / 255.0);//Vec3f��ʾ������ͨ������ l[row][column][depth]
	Mat_<Vec3f> r;
	r8u.convertTo(r, CV_32F, 1.0 / 255.0);
	//Mat_<float> m(l.rows,l.cols,0.0); //��mȫ����ֵΪ0
	//m(Range::all(),Range(0,m.cols/2)) = 1.0; //ȡmȫ����&[0,m.cols/2]�У���ֵΪ1.0
	//�������ģ��mҪ�� �ĳ���ѷ���ߵõ��Ľ��C������
	Mat_<float> m(l.rows, l.cols, 0.0);
	
	for (int i = 0;i < l.rows;i++)
	{
		for (int j = 0;j < l.cols;j++)
		{
			// ��Ϊ��Ҫ��ֻ��λ��
			if (C.at<Vec3b>(i, j)[0] != 0 && C.at<Vec3b>(i, j)[1] != 0 && C.at<Vec3b>(i, j)[2] != 0)
				m(i, j) = 1.0;
		}
	}
	Mat_<Vec3f> blend = LaplacianBlend(l, r, m);
	imshow("blended", blend);
	waitKey(0);
	imwrite("Output/temp/TempLaplacianBlendImage.jpg", blend);
	return (char*)"Output/temp/TempLaplacianBlendImage.jpg";
	
	
}

MyLaplacianBlending::MyLaplacianBlending()
{
}

#endif // __DEBUG

MyLaplacianBlending::~MyLaplacianBlending()
{
}
#endif // OpencvDEBUGType