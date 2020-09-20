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
	//������ͼ������dy�洢����
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
//���ڰ汾
CImg<int> MyMorePictureStitch::MyStitchImage(char* _filenameA, char* _filenameB, CImg<int> lastblendedImg,int matchVecdx,int matchVecdy) {
	CImg<int> srcImgA, srcImgB;
	//������ͼ��֮���x��y��ƫ�������д洢
	SaveSequentialImagesDifference(_filenameA, _filenameB, matchVecdx, matchVecdy);

	//��ȡͼ��
	srcImgA.load(_filenameA);
	srcImgB.load(_filenameB);

	//�жϴ������һ��ͼ����Ƿ�Ϊ��
	//��Ϊ������丳��ͼ��a����ֵ
	if(lastblendedImg==NULL)
	{
		lastblendedImg = srcImgA;
	}

	//�ж��������ʼ��ͼƬ
	if (StitchPictureNum == 0) 
	{
		//���ǵĻ�����ʼͼƬ�Ŀ�Ⱥ͸߶ȶ���������
		oriImageWidth = lastblendedImg._width;
		oriImageHeight = lastblendedImg._height;
	}

	//�жϸ�ͼ���ƴ��ͼ���Ƿ����ұߣ��ǵĻ����ϼ�¼�����ҽ���ͼ����չ�ĵ���
	if (matchVecdx > srcImgB._width)
	{
		//2019-03-09�޸�����
		//��Ҫƴ�ӵ�ͼƬ�����
		//�µļ�¼ֵ���еǼ��滻
		//����ʵ�� = ��ƴ��ͼ�� - dx/2��*2��
		//���Է���ֽ2��ƴ��ȫ��ͼƬ
		//leftmatchVec�������غ�����
		//matchVecxΪ���ʣ��Ĵ�ƴ�����
		int leftmatchVec = (srcImgB._width* 2 - matchVecdx);

		//��ƴ���ұ�ͼ��ͬ���ǣ���Ҫ��dy��ȡ��ֵ
		//�жϵ�ǰͼ���������һ��ͼ���������ƶ����������ƶ�
		//С��0�������ƶ�
		//����0�������ƶ�
		//��ʽ��dy1 = img2.y - img1.y
		//dy2 = img3.y - img2.y;
		//img3�����img1:
		//dy = dy1+dy2 = img3.y - img1.y;
		//�����ȼ���ƫ����������������һ��ͼ�������ϻ�������
		//ȡ����
		lastcalureHeight = lastcalureHeight + (0- matchVecdy);

		if (lastcalureHeight <= 0)
		{
			//�����Ƚ�˭��С
			//�ж���������µ�ƫ���������С�����������������У�������Ҫ����������
			//������ڵĻ����͸����µ�������ͬʱ�������µ�ֵ
			MaxDown = MaxDown < lastcalureHeight ? MaxDown : lastcalureHeight;

		}
		else//�����ƶ� 
		{
			//�����Ƚ�˭���
			//�ж���������ϵ�ƫ���������С�����������������У�������Ҫ����������
				//������ڵĻ����͸����ϵ�������ͬʱ�������ϵ�ֵ
			MaxUp = MaxUp > matchVecdy ? MaxUp : matchVecdy;
		}

		//�������ǰ��ͼ������ڵ�һ��ͼ�����ƫ���˶���
		lastcalureWeight = lastcalureWeight -(srcImgB._width - leftmatchVec) ;
		if (lastcalureWeight > 0)
			MaxRight = MaxRight > lastcalureWeight ? MaxRight : lastcalureWeight;
		else
			MaxLeft = MaxLeft < lastcalureWeight ? MaxLeft : lastcalureWeight;

		//��ƴ�ӵ�ͼƬ��ͼ������
		//��ߵ�ͼ����������ķ����ƫ��ֵ���෴��
		//dx�����µı�ʾ����
		//-----------------------�ϲ��Ĵ��ڴ�Сλ��-----------------------------------------
		//dxΪ����ͼ���������֮��ĺ�Ĳ�ֵ
		//dyΪ����ͼ���������֮������Ĳ�ֵ
		//�ж��Ƿ���Ҫ���ӻ����ĳ���
		//�������ԭͼ�񣬼���������ֵͬʱ����������ֵ
		//������һ��ͼ���ж��Ƿ������еĸ߶�
		//���ӳ�ʼ��ͼ��Ĵ�С
		//�������ԭͼ�񣬼���������ֵͬʱ����������ֵ
		//�����ԭ��Ϊ���п�ȵ���ȡ��ԭ���Ĵ���ͼ��Ŀ�ȣ�
		//������ƴ����ߵ�ͼ��ʱ�������Ҫ������ͼ������
		//blendedImg = CImg<int>(lastblendedImg._width + srcImgB._width - matchVecdx,
		//	srcImgB._height + abs(MaxDown) + abs(MaxUp), 1, 3, 0);
		blendedImg = CImg<int>(oriImageWidth + abs(MaxLeft) + abs(MaxRight),
			srcImgB._height + abs(MaxDown) + abs(MaxUp), 1, 3, 0);

		//�ж��¼����ͼ�����������һ��ͼ�����ƶ��������ƶ�
	//�ж�x�������ƴ�Ӻû����ǵ�һ��ͼ�������
	//������ͼ��ĵ�ǡ��������ͼ����ص�֮������Ҫ����ֵ����
	//�����ص�����Ĳ�ֵ��ʱ�򣬽��غ�����ı���ͬʱ�����ص�����Ŀ��
	//�˴���Ҫ��������
		//�˴���ƴ��������ڴ�ƴ��ͼ����ƥ���ͼ����ұ�
		cimg_forXY(blendedImg, x, y) {
			if (matchVecdy <= 0) {    //�Ҳ�ͼƬ��Ҫ�������ƶ�
				if (x < lastblendedImg._width && y < lastblendedImg._height) {
					//�˴��Ĺ�ʽΪ�ۼ�ƫ����������ͼ��δ��ֵ�Ĳ��������� = ʵ���ص�����
					if (x >= (lastcalureWeight + (srcImgB._width- leftmatchVec)) && y >= (0 - lastcalureHeight)) {    //���
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
					else {    //A�������ƴ�ӺõĶ��ڲ���
						blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
						blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
						blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
					}

				}
				//y����Ӧ���������ͼ����±ߣ��ұ�
				else if (x >= (lastblendedImg._width - matchVecdx)
					&& y >= (0 - lastcalureHeight) && y < (0 - lastcalureHeight) + srcImgB._height) {    //B���ڲ���
					blendedImg(x, y, 0, 0) = srcImgB(x - (lastblendedImg._width - matchVecdx), y - (0 - lastcalureHeight), 0, 0);
					blendedImg(x, y, 0, 1) = srcImgB(x - (lastblendedImg._width - matchVecdx), y - (0 - lastcalureHeight), 0, 1);
					blendedImg(x, y, 0, 2) = srcImgB(x - (lastblendedImg._width - matchVecdx), y - (0 - lastcalureHeight), 0, 2);
				}
				else {    //��ɫ����
					blendedImg(x, y, 0, 0) = 0;
					blendedImg(x, y, 0, 1) = 0;
					blendedImg(x, y, 0, 2) = 0;
				}
			}
			//3-9���޸�
			/**/else {    //matchVec.dy > 0; �Ҳ�ͼƬ��Ҫ�������ƶ�
				if (x < lastblendedImg._width && y >= lastcalureHeight) {
					if (x >= (lastblendedImg._width - matchVecdx) && y < srcImgB._height) {    //���
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
					else {    //A���ڲ���
						blendedImg(x, y, 0, 0) = srcImgA(x, y - lastcalureHeight, 0, 0);
						blendedImg(x, y, 0, 1) = srcImgA(x, y - lastcalureHeight, 0, 1);
						blendedImg(x, y, 0, 2) = srcImgA(x, y - lastcalureHeight, 0, 2);
					}
				}
				else if (x >= (lastblendedImg._width - matchVecdx) && y < srcImgB._height) {    //B���ڲ���
					blendedImg(x, y, 0, 0) = srcImgB(x - (lastblendedImg._width - matchVecdx), y, 0, 0);
					blendedImg(x, y, 0, 1) = srcImgB(x - (lastblendedImg._width - matchVecdx), y, 0, 1);
					blendedImg(x, y, 0, 2) = srcImgB(x - (lastblendedImg._width - matchVecdx), y, 0, 2);
				}
				else {    //��ɫ����
					blendedImg(x, y, 0, 0) = 0;
					blendedImg(x, y, 0, 1) = 0;
					blendedImg(x, y, 0, 2) = 0;
				}
			}

		}



	}
	else
	{
		//��Ҫƴ�ӵ�ͼ�����ұ�
		//�жϵ�ǰͼ���������һ��ͼ���������ƶ����������ƶ�
		//С��0�������ƶ�
		//����0�������ƶ�
		//��ʽ��dy1 = img2.y - img1.y
		//dy2 = img3.y - img2.y;
		//img3�����img1:
		//dy = dy1+dy2 = img3.y - img1.y;
		//�����ȼ���ƫ����������������һ��ͼ�������ϻ�������
		lastcalureHeight += matchVecdy;
		if (lastcalureHeight <= 0)
		{
			//�����Ƚ�˭��С
			//�ж���������µ�ƫ���������С�����������������У�������Ҫ����������
			//������ڵĻ����͸����µ�������ͬʱ�������µ�ֵ
			MaxDown = MaxDown < lastcalureHeight ? MaxDown : lastcalureHeight;

		}
		else//�����ƶ� 
		{
			//�����Ƚ�˭���
			//�ж���������ϵ�ƫ���������С�����������������У�������Ҫ����������
				//������ڵĻ����͸����ϵ�������ͬʱ�������ϵ�ֵ
			MaxUp = MaxUp > lastcalureHeight ? MaxUp : lastcalureHeight;
		}

		//�������ǰ��ͼ������ڵ�һ��ͼ�����ƫ���˶���
		lastcalureWeight = lastcalureWeight + srcImgB._width - matchVecdx;
		if (lastcalureWeight > 0)
			MaxRight = MaxRight > lastcalureWeight ? MaxRight : lastcalureWeight;
		else
			MaxLeft = MaxLeft < lastcalureWeight ? MaxLeft : lastcalureWeight;

		//��ƴ�ӵ�ͼƬ��ͼ����ұ�
		//-----------------------�ϲ��Ĵ��ڴ�Сλ��-----------------------------------------
		//dxΪ����ͼ���������֮��ĺ�Ĳ�ֵ
		//dyΪ����ͼ���������֮������Ĳ�ֵ
		//�ж��Ƿ���Ҫ���ӻ����ĳ���
		//�������ԭͼ�񣬼���������ֵͬʱ����������ֵ
		//������һ��ͼ���ж��Ƿ������еĸ߶�
		//���ӳ�ʼ��ͼ��Ĵ�С
		//�����ԭ��Ϊ���п�ȵ���ȡ��ԭ���Ĵ���ͼ��Ŀ�ȣ�
		//������ƴ����ߵ�ͼ��ʱ�������Ҫ������ͼ������
		//blendedImg = CImg<int>(lastblendedImg._width + srcImgB._width - matchVecdx,
		//	srcImgB._height + abs(MaxDown) + abs(MaxUp), 1, 3, 0);

		//
		blendedImg = CImg<int>(oriImageWidth + abs(MaxLeft) + abs(MaxRight),
			srcImgB._height + abs(MaxDown) + abs(MaxUp), 1, 3, 0);
	

	//�ж��¼����ͼ�����������һ��ͼ�����ƶ��������ƶ�
	//�ж�x�������ƴ�Ӻû����ǵ�һ��ͼ�������
	//������ͼ��ĵ�ǡ��������ͼ����ص�֮������Ҫ����ֵ����
	//�����ص�����Ĳ�ֵ��ʱ�򣬽��غ�����ı���ͬʱ�����ص�����Ŀ��
	//�˴���Ҫ��������
	cimg_forXY(blendedImg, x, y) {
		if (matchVecdy <= 0) {    //�Ҳ�ͼƬ��Ҫ�������ƶ�
			//�жϸ�ͼ��������ڵ�һ��ͼ��λ�������»�������
			

				if (lastcalureHeight == MaxDown|| lastcalureHeight==MaxUp) 
				{
				//�������Ӹ߶ȵĲ������������3-9�Ŵ���

				}
				else
				{
					//���ڲ���Ҫ���Ӹ߶ȵĲ��֣�����3-10�Ŵ������д���
					if (x < lastblendedImg._width && y < lastblendedImg._height) {
						//�жϴ�ƴ�ӵ�ͼ�����һ��ͼ����Ƚ����ڻ������±�
						if (lastcalureHeight > 0) 
						{
						//��ͼƬ��Ե�һ��ͼ�����ϱߣ��ص�����Ϊ�϶����ȥ��ƫ�ƿ�ʼ����ƴ�����ͼ�����
						}
						else 
						{
						//��ͼƬ��Ե�һ��ͼ�����±ߣ��ص�����Ϊ�¶��߼�ȥ��ƫ�ƣ��ó��ڶ���ͼ��Ľ�β���ؾ�������ͼ��ľ���
							//lastblendedImg._height - abs(MaxDown - lastcalureHeight)Ϊ�ڶ���ͼ�����ƴ��ʱ��ĸ߶�
							if (x >= (lastblendedImg._width - matchVecdx) && y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight)
								-srcImgB._height && y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight)- matchVecdy) {    //���
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
							else {    //A�������ƴ�ӺõĶ��ڲ���
								blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
								blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
								blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
							}
						
						}
						if (x >= (lastblendedImg._width - matchVecdx) && y >= (0 - lastcalureHeight)&&y <= abs(MaxDown- lastcalureHeight)) {    //���
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
						else {    //A�������ƴ�ӺõĶ��ڲ���
							blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
							blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
							blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
						}

					}
					//y����Ӧ���������ͼ����±ߣ��ұ�
					else if (x >= (lastblendedImg._width - matchVecdx)
						&& y >= (0 - lastcalureHeight) && y < (0 - lastcalureHeight) + srcImgB._height) {    //B���ڲ���
						blendedImg(x, y, 0, 0) = srcImgB(x - (lastblendedImg._width - matchVecdx), y - (0 - lastcalureHeight), 0, 0);
						blendedImg(x, y, 0, 1) = srcImgB(x - (lastblendedImg._width - matchVecdx), y - (0 - lastcalureHeight), 0, 1);
						blendedImg(x, y, 0, 2) = srcImgB(x - (lastblendedImg._width - matchVecdx), y - (0 - lastcalureHeight), 0, 2);
					}
					else {    //��ɫ����
						blendedImg(x, y, 0, 0) = 0;
						blendedImg(x, y, 0, 1) = 0;
						blendedImg(x, y, 0, 2) = 0;
					}

				}
			

			if (x < lastblendedImg._width && y < lastblendedImg._height) {
				if (x >= (lastblendedImg._width - matchVecdx) && y >= (0 - lastcalureHeight)) {    //���
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
				else {    //A�������ƴ�ӺõĶ��ڲ���
					blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
					blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
					blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
				}
				
			}
			//y����Ӧ���������ͼ����±ߣ��ұ�
			else if (x >= (lastblendedImg._width - matchVecdx)
				&& y >= (0 - lastcalureHeight) && y < (0 - lastcalureHeight) + srcImgB._height) {    //B���ڲ���
				blendedImg(x, y, 0, 0) = srcImgB(x - (lastblendedImg._width - matchVecdx), y - (0 - lastcalureHeight), 0, 0);
				blendedImg(x, y, 0, 1) = srcImgB(x - (lastblendedImg._width - matchVecdx), y - (0 - lastcalureHeight), 0, 1);
				blendedImg(x, y, 0, 2) = srcImgB(x - (lastblendedImg._width - matchVecdx), y - (0 - lastcalureHeight), 0, 2);
			}
			else {    //��ɫ����
				blendedImg(x, y, 0, 0) = 0;
				blendedImg(x, y, 0, 1) = 0;
				blendedImg(x, y, 0, 2) = 0;
			}
		}
		//3-9���޸�
		/**/else {    //matchVec.dy > 0; �Ҳ�ͼƬ��Ҫ�������ƶ�
			if (x < lastblendedImg._width && y >= lastcalureHeight) {
				if (x >= (lastblendedImg._width - matchVecdx) && y < srcImgB._height) {    //���
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
				else {    //A���ڲ���
					blendedImg(x, y, 0, 0) = lastblendedImg(x, y - lastcalureHeight, 0, 0);
					blendedImg(x, y, 0, 1) = lastblendedImg(x, y - lastcalureHeight, 0, 1);
					blendedImg(x, y, 0, 2) = lastblendedImg(x, y - lastcalureHeight, 0, 2);
				}
			}
			else if (x >= (lastblendedImg._width - matchVecdx) && y < srcImgB._height) {    //B���ڲ���
				blendedImg(x, y, 0, 0) = srcImgB(x - (lastblendedImg._width - matchVecdx), y, 0, 0);
				blendedImg(x, y, 0, 1) = srcImgB(x - (lastblendedImg._width - matchVecdx), y, 0, 1);
				blendedImg(x, y, 0, 2) = srcImgB(x - (lastblendedImg._width - matchVecdx), y, 0, 2);
			}
			else {    //��ɫ����
				blendedImg(x, y, 0, 0) = 0;
				blendedImg(x, y, 0, 1) = 0;
				blendedImg(x, y, 0, 2) = 0;
			}
		}

	}

	}

	//���ע��Ӧ����û���ô���
/*cimg_forXY(blendedImg, x, y) {
	if (matchVecdy <= 0) {    //�Ҳ�ͼƬ��Ҫ�������ƶ�
		if (x < lastblendedImg._width && y < lastblendedImg._height) {
			//�˴��Ĺ�ʽΪ�ۼ�ƫ����������ͼ��δ��ֵ�Ĳ��������� = ʵ���ص�����
			if (x >= (lastcalureWeight + (srcImgB._width - leftmatchVec)) && y >= (0 - lastcalureHeight)) {    //���
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
			else {    //A�������ƴ�ӺõĶ��ڲ���
				blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
				blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
				blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
			}

		}
		//y����Ӧ���������ͼ����±ߣ��ұ�
		else if (x >= (lastblendedImg._width - matchVecdx)
			&& y >= (0 - lastcalureHeight) && y < (0 - lastcalureHeight) + srcImgB._height) {    //B���ڲ���
			blendedImg(x, y, 0, 0) = srcImgB(x - (lastblendedImg._width - matchVecdx), y - (0 - lastcalureHeight), 0, 0);
			blendedImg(x, y, 0, 1) = srcImgB(x - (lastblendedImg._width - matchVecdx), y - (0 - lastcalureHeight), 0, 1);
			blendedImg(x, y, 0, 2) = srcImgB(x - (lastblendedImg._width - matchVecdx), y - (0 - lastcalureHeight), 0, 2);
		}
		else {    //��ɫ����
			blendedImg(x, y, 0, 0) = 0;
			blendedImg(x, y, 0, 1) = 0;
			blendedImg(x, y, 0, 2) = 0;
		}
	}
	//3-9���޸�
	else {    //matchVec.dy > 0; �Ҳ�ͼƬ��Ҫ�������ƶ�
		if (x < lastblendedImg._width && y >= lastcalureHeight) {
			if (x >= (lastblendedImg._width - matchVecdx) && y < srcImgB._height) {    //���
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
			else {    //A���ڲ���
				blendedImg(x, y, 0, 0) = srcImgA(x, y - lastcalureHeight, 0, 0);
				blendedImg(x, y, 0, 1) = srcImgA(x, y - lastcalureHeight, 0, 1);
				blendedImg(x, y, 0, 2) = srcImgA(x, y - lastcalureHeight, 0, 2);
			}
		}
		else if (x >= (lastblendedImg._width - matchVecdx) && y < srcImgB._height) {    //B���ڲ���
			blendedImg(x, y, 0, 0) = srcImgB(x - (lastblendedImg._width - matchVecdx), y, 0, 0);
			blendedImg(x, y, 0, 1) = srcImgB(x - (lastblendedImg._width - matchVecdx), y, 0, 1);
			blendedImg(x, y, 0, 2) = srcImgB(x - (lastblendedImg._width - matchVecdx), y, 0, 2);
		}
		else {    //��ɫ����
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

//3-11�汾����汾����
//1.ģ�黯����������������������������
//�����еı������е�������ƶ����벻ͬ�������֧��������ڹ�����������Ӱ��ʹ��

//2.�����������ķ����޸�

//3-13 �޸����ͼ���˼�룬���ͼ������ұߵ�������󲿷ֶ����ұ������ߵ�����ϵ
//��Ҫ�޸�˼·�ұ���������ͼ��Ϊ

//3-14�����ұ�ͼ��δ���ӿ�ȵ����
//ͬʱ�����ƴ�ӵ�һЩͼ�����ش���������µķ����������޸�bug

//3-15������ƴ�ӵĵط��������޸�
//3-31���ںϲ��ֵĴ����������µ������ѡ�������ںϷ�ʽ����Ҫ�Ƕ��ں������ͼ����г�����ǰ����һ�ݷ���ٶ���
//2019-4-17�����޸�����
CImg<int> MyMorePictureStitch::MyStitchImageByGradualFusionBlend(char* _filenameA, char* _filenameB, CImg<int> lastblendedImg, int matchVecdx, int matchVecdy)
{
	CImg<int> srcImgA, srcImgB;
	//������ͼ��֮���x��y��ƫ�������д洢
	SaveSequentialImagesDifference(_filenameA, _filenameB, matchVecdx, matchVecdy);

	//��ȡͼ��
	srcImgA.load(_filenameA);
	srcImgB.load(_filenameB);

	//�жϴ������һ��ͼ����Ƿ�Ϊ��
	//��Ϊ������丳��ͼ��a����ֵ
	if (lastblendedImg == NULL)
	{
		lastblendedImg = srcImgA;
	}

	//�ж��������ʼ��ͼƬ
	if (StitchPictureNum == 0)
	{
		//���ǵĻ�����ʼͼƬ�Ŀ�Ⱥ͸߶ȶ���������
		oriImageWidth = lastblendedImg._width;
		oriImageHeight = lastblendedImg._height;
	}

	//�жϸ�ͼ���ƴ��ͼ���Ƿ������
	//�ǵĻ����ϼ�¼�����ҽ���ͼ����չ�ĵ���
	if (matchVecdx > srcImgB._width)
	{
		//2019-03-09�޸�����
		//��Ҫƴ�ӵ�ͼƬ�����
		//�µļ�¼ֵ���еǼ��滻
		//����ʵ�� = ��ƴ��ͼ�� - dx/2��*2��
		//���Է���ֽ2��ƴ��ȫ��ͼƬ
		//leftmatchVec�������غ�����
		//matchVecxΪ���ʣ��Ĵ�ƴ�����
		int leftmatchVec = (srcImgB._width * 2 - matchVecdx );

		//��ƴ���ұ�ͼ��ͬ���ǣ���Ҫ��dy��ȡ��ֵ
		//�жϵ�ǰͼ���������һ��ͼ���������ƶ����������ƶ�
		//С��0�������ƶ�
		//����0�������ƶ�
		//��ʽ��dy1 = img2.y - img1.y
		//dy2 = img3.y - img2.y;
		//img3�����img1:
		//dy = dy1+dy2 = img3.y - img1.y;
		//�����ȼ���ƫ����������������һ��ͼ�������ϻ�������
		//ȡ����
		//ͬʱ��ƫ����ȡ������ת��Ϊ��ȷ��ͼƬ����λ�����ƫ����
		//matchVecdy = 0 - matchVecdy;
		lastcalureHeight = lastcalureHeight + (matchVecdy);

		if (lastcalureHeight <= 0)
		{
			//�����Ƚ�˭��С
			//�ж���������µ�ƫ���������С�����������������У�������Ҫ����������
			//������ڵĻ����͸����µ�������ͬʱ�������µ�ֵ
			MaxDown = MaxDown < lastcalureHeight ? MaxDown : lastcalureHeight;

		}
		else//�����ƶ� 
		{
			//�����Ƚ�˭���
			//�ж���������ϵ�ƫ���������С�����������������У�������Ҫ����������
				//������ڵĻ����͸����ϵ�������ͬʱ�������ϵ�ֵ
			MaxUp = MaxUp > lastcalureHeight ? MaxUp : lastcalureHeight;
		}

		//�������ǰ��ͼ������ڵ�һ��ͼ�����ƫ���˶���
		lastcalureWeight = lastcalureWeight - (srcImgB._width - leftmatchVec);
		if (lastcalureWeight > 0)
			MaxRight = MaxRight > lastcalureWeight ? MaxRight : lastcalureWeight;
		else
			MaxLeft = MaxLeft < lastcalureWeight ? MaxLeft : lastcalureWeight;

		//��ƴ�ӵ�ͼƬ��ͼ������
		//��ߵ�ͼ����������ķ����ƫ��ֵ���෴��
		//dx�����µı�ʾ����
		//-----------------------�ϲ��Ĵ��ڴ�Сλ��-----------------------------------------
		//dxΪ����ͼ���������֮��ĺ�Ĳ�ֵ
		//dyΪ����ͼ���������֮������Ĳ�ֵ
		//�ж��Ƿ���Ҫ���ӻ����ĳ���
		//�������ԭͼ�񣬼���������ֵͬʱ����������ֵ
		//������һ��ͼ���ж��Ƿ������еĸ߶�
		//���ӳ�ʼ��ͼ��Ĵ�С
		//�������ԭͼ�񣬼���������ֵͬʱ����������ֵ
		//�����ԭ��Ϊ���п�ȵ���ȡ��ԭ���Ĵ���ͼ��Ŀ�ȣ�
		//������ƴ����ߵ�ͼ��ʱ�������Ҫ������ͼ������
		//blendedImg = CImg<int>(lastblendedImg._width + srcImgB._width - matchVecdx,
		//	srcImgB._height + abs(MaxDown) + abs(MaxUp), 1, 3, 0);
		blendedImg = CImg<int>(oriImageWidth + abs(MaxLeft) + abs(MaxRight),
			srcImgB._height + abs(MaxDown) + abs(MaxUp), 1, 3, 0);

		//�ж��¼����ͼ�����������һ��ͼ�����ƶ��������ƶ�
		//�ж�x�������ƴ�Ӻû����ǵ�һ��ͼ�������
		//������ͼ��ĵ�ǡ��������ͼ����ص�֮������Ҫ����ֵ����
		//�����ص�����Ĳ�ֵ��ʱ�򣬽��غ�����ı���ͬʱ�����ص�����Ŀ��
		//�˴���Ҫ��������
		//�ж��Ƿ����߿�ʼ��չͼ��
		if (lastcalureWeight == MaxLeft) 
		{
			//��ͼ��ƴ�ӵ�ʱ����Ҫ��������ӿռ�
			//���Ҵ�ƴ��ͼ�������
			//�жϸ�ͼ���Ƿ�����߶ȵı仯
			if (lastcalureHeight == MaxDown || lastcalureHeight == MaxUp)
			{
				//��������ͼ����Ҫ���Ӹ߶ȵĲ��������
				
				if (matchVecdy <= 0) {
					//��ͼ����Ե�һ��ͼ�����±�,��lastcalureHeightһ����������Ϊ����
					//����bͼ�񣨴�ƴ��ͼ�񣩵��ص�����Ϊ�ܵ�ͼ��߶ȼ�ȥbͼ��ĸ߶ȣ��õ���ʼ�߶�
					//����Ҫע����ǣ�ͼ�����ʼλ�úʹ�ƴ��λ����ʼ��һ������ͬһ�������ϣ�����ʹA��ƴ��ͼƬ
					//������ͼ����ڲ������߽�ط�����ͼ
					cimg_forXY(blendedImg, x, y) {
						if (x>= blendedImg._width - lastblendedImg._width
							&& x < blendedImg._width
							&& y < lastblendedImg._height) {
							//�������ѡ��ʹ�ò�ֵ���߲���ֵ����ֵ�н��䣬���ǿ��ܻ�ͺ�ɫ�����ص���
							//��ֵ�Ļ���������Ļ�����ı�
							//���ǲ���ֵ���е�ͻأ�����Ը��������x������ʼ���
							//������Ϊ�ص������ֵ,��ʱ������ԣ����ڽ��书�ܷ���ʱ����Զ����������ֵ
							//Ϊͼ��b��δ�غϵ�����
							//abs(MaxLeft) + (lastcalureWeight)== 0�໥����
							if (x >= abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec)
								&& x < abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y >= blendedImg._height - srcImgB._height
								&& y < blendedImg._height - srcImgB._height + (srcImgB._height - abs(matchVecdy))
								) 
							{    //���
								//3-12���·��֣�����ȫ���������˼·�޸�
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
								&& y < blendedImg._height) {    //B���ڲ���
								blendedImg(x, y, 0, 0) = srcImgB(x, y - (blendedImg._height - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x, y - (blendedImg._height - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x, y - (blendedImg._height - srcImgB._height), 0, 2);
							}
							else {    //A�������ƴ�ӺõĶ��ڲ���
								blendedImg(x, y, 0, 0) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y , 0, 0);
								blendedImg(x, y, 0, 1) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 1);
								blendedImg(x, y, 0, 2) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 2);
							}

						}
						//y����Ӧ���������ͼ����±ߣ��ұ�
						else if (x >= (abs(MaxLeft) + (lastcalureWeight)) 
							&& x < blendedImg._width - lastblendedImg._width
							&& y >= blendedImg._height - srcImgB._height 
							&& y < blendedImg._height) {    //B���ڲ���
							blendedImg(x, y, 0, 0) = srcImgB(x, y - (blendedImg._height - srcImgB._height), 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x, y - (blendedImg._height - srcImgB._height), 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x, y - (blendedImg._height - srcImgB._height), 0, 2);
						}
						else {    //��ɫ����
							blendedImg(x, y, 0, 0) = 0;
							blendedImg(x, y, 0, 1) = 0;
							blendedImg(x, y, 0, 2) = 0;
						}

						
					}
				}
				//3-11���޸�
				else {
					//matchVec.dy > 0;���ͼƬ��Ҫ�������ƶ�
					//��ͼ����Ե�һ��ͼ�����ϱ�,��lastcalureHeightһ����������Ϊ����
					cimg_forXY(blendedImg, x, y) {
						if (x >= blendedImg._width - lastblendedImg._width
							&& x < blendedImg._width 
							&& y >= blendedImg._height - lastblendedImg._height
							&& y < blendedImg._height
							) {
							//�������ѡ��ʹ�ò�ֵ���߲���ֵ����ֵ�н��䣬���ǿ��ܻ�ͺ�ɫ�����ص���
							//��ֵ�Ļ���������Ļ�����ı�
							//���ǲ���ֵ���е�ͻأ�����Ը��������x������ʼ���
							//����ֵ����Ϊ
							if (x >= abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y>= abs(matchVecdy)
								&& y < srcImgB._height) {    //���
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
							{	//B����
								blendedImg(x, y, 0, 0) = srcImgB(x, y, 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x, y, 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x, y, 0, 2);
							}
							else {    //A���ڲ���
								blendedImg(x, y, 0, 0) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y - (blendedImg._height - lastblendedImg._height), 0, 0);
								blendedImg(x, y, 0, 1) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y - (blendedImg._height - lastblendedImg._height), 0, 1);
								blendedImg(x, y, 0, 2) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y - (blendedImg._height - lastblendedImg._height), 0, 2);
							}
						}
						else if (x >= (abs(MaxLeft) + (lastcalureWeight))
							&& x <= blendedImg._width - lastblendedImg._width
							&& y < srcImgB._height) {    //B���ڲ���
							blendedImg(x, y, 0, 0) = srcImgB(x, y, 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x, y, 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x, y, 0, 2);
						}
						else {    //��ɫ����
							blendedImg(x, y, 0, 0) = 0;
							blendedImg(x, y, 0, 1) = 0;
							blendedImg(x, y, 0, 2) = 0;
						}
					}
				}
			}
			else
			{
				//��������ͼ����Ҫ���Ӹ߶ȵĲ������������3-11���ع�����
				//���Ǹ�ͼ����������ӿռ�仯
				if (matchVecdy <= 0)
				{

					//��ƴ�ӵ�ͼƬ�����ƥ���ͼ�����������ƶ�
					//������ͼ����Ҫ��չ�߶�,����Ҫ��չ���
					if (lastcalureHeight > 0)
					{
						//��ͼƬ��Ե�һ��ͼ�����ϱߣ��ص�����Ϊ�϶����ȥ��ƫ�ƿ�ʼ����ƴ�����ͼ�����
						cimg_forXY(blendedImg, x, y)
						{
							if (x >= blendedImg._width - lastblendedImg._width
								&& x < blendedImg._width
								&& y < lastblendedImg._height)
							{
								//�������ѡ��ʹ�ò�ֵ���߲���ֵ����ֵ�н��䣬���ǿ��ܻ�ͺ�ɫ�����ص���
								//��ֵ�Ļ���������Ļ�����ı�
								//���ǲ���ֵ���е�ͻأ�����Ը��������x������ʼ���

								//������Ϊ�ص������ֵ,��ʱ������ԣ����ڽ��书�ܷ���ʱ����Զ����������ֵ
								//(srcImgB._width - leftmatchVec) Ϊͼ��b��δ�غϵ�����
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
								//��b�����ֵҲ���ӽ�ȥ���ⲿ�ֵ�ͼ����δ���в�ֵ����
								else if (x >= blendedImg._width - lastblendedImg._width
									&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
									&& y >= abs(MaxUp - lastcalureHeight)
									&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B���ڲ���
									blendedImg(x, y, 0, 0) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 2);
								}
								else {    //A�������ƴ�ӺõĶ��ڲ���
									blendedImg(x, y, 0, 0) = lastblendedImg(x - blendedImg._width - lastblendedImg._width, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x - blendedImg._width - lastblendedImg._width, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x - blendedImg._width - lastblendedImg._width, y, 0, 2);
								}
							}
							//y����Ӧ���������ͼ����±ߣ��ұ�,�������ﲻ�����У�������������ϱ�ѭ��
							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&& x <= (abs(MaxLeft) + (lastcalureWeight)) + blendedImg._width - lastblendedImg._width
								&& y >= abs(MaxUp - lastcalureHeight)
								&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B���ڲ���
								blendedImg(x, y, 0, 0) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 2);
							}
							else {    //��ɫ����
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}
					else
					{
						//��ͼƬ��Ե�һ��ͼ�����±ߣ��ص�����Ϊ�¶����ȥ��ƫ�ƿ�ʼ������ͼƬ�ص��������
						cimg_forXY(blendedImg, x, y)
						{
							if (x >= blendedImg._width - lastblendedImg._width
								&& x < blendedImg._width
								&& y < lastblendedImg._height)
							{
								//�������ѡ��ʹ�ò�ֵ���߲���ֵ����ֵ�н��䣬���ǿ��ܻ�ͺ�ɫ�����ص���
								//��ֵ�Ļ���������Ļ�����ı�
								//���ǲ���ֵ���е�ͻأ�����Ը��������x������ʼ���

								//������Ϊ�ص������ֵ,��ʱ������ԣ����ڽ��书�ܷ���ʱ����Զ����������ֵ
								//(srcImgB._width - leftmatchVec) Ϊͼ��b��δ�غϵ�����
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
								//��b�����ֵҲ���ӽ�ȥ���ⲿ�ֵ�ͼ����δ���в�ֵ����
								else if (x >= blendedImg._width - lastblendedImg._width
									&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
									&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
									&& y < blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B���ڲ���
									blendedImg(x, y, 0, 0) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x , y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x , y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
								}
								else {    //A�������ƴ�ӺõĶ��ڲ���
									blendedImg(x, y, 0, 0) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 2);
								}


							}
							//y����Ӧ���������ͼ����±ߣ��ұ�
							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&& x <= blendedImg._width - lastblendedImg._width
								&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
								&& y < blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B���ڲ���
								blendedImg(x, y, 0, 0) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
							}
							else {    //��ɫ����
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}

				}
				else
				{
					//��ƴ�ӵ�ͼƬ�����ƥ��ĵ�ͼ�����������ƶ�
					//������ͼ����Ҫ��չ�߶�,����Ҫ��չ���
					if (lastcalureHeight > 0)
					{
						//��ͼƬ��Ե�һ��ͼ�����ϱߣ��ص�����Ϊ�϶����ȥ��ƫ�ƿ�ʼ����ƴ�����ͼ�����
						cimg_forXY(blendedImg, x, y)
						{
							if (x > blendedImg._width - lastblendedImg._width
								&& x < blendedImg._width
								&& y < lastblendedImg._height)
							{
								//�������ѡ��ʹ�ò�ֵ���߲���ֵ����ֵ�н��䣬���ǿ��ܻ�ͺ�ɫ�����ص���
								//��ֵ�Ļ���������Ļ�����ı�
								//���ǲ���ֵ���е�ͻأ�����Ը��������x������ʼ���

								//������Ϊ�ص������ֵ,��ʱ������ԣ����ڽ��书�ܷ���ʱ����Զ����������ֵ
								//(srcImgB._width - leftmatchVec) Ϊͼ��b��δ�غϵ�����
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
								//��b�����ֵҲ���ӽ�ȥ���ⲿ�ֵ�ͼ����δ���в�ֵ����
								else if (x >= blendedImg._width - lastblendedImg._width
									&& x <= (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width)
									&& y >= abs(MaxUp - lastcalureHeight) 
									&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B���ڲ���
									blendedImg(x, y, 0, 0) = srcImgB(x , y - (abs(MaxUp - lastcalureHeight)), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x , y - (abs(MaxUp - lastcalureHeight)), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x , y - (abs(MaxUp - lastcalureHeight)), 0, 2);
								}
								else {    //A�������ƴ�ӺõĶ��ڲ���
									blendedImg(x, y, 0, 0) = lastblendedImg(x-(blendedImg._width - lastblendedImg._width), y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 2);
								}
							}
							//y����Ӧ���������ͼ����±ߣ��ұ�,�������ﲻ�����У�������������ϱ�ѭ��
							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&& x <= blendedImg._width - lastblendedImg._width
								&& y >= abs(MaxUp - lastcalureHeight) 
								&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B���ڲ���
								blendedImg(x, y, 0, 0) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 2);
							}
							else {    //��ɫ����
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}
					else
					{
						//��ͼƬ��Ե�һ��ͼ�����±ߣ��ص�����Ϊ�¶����ȥ��ƫ�ƿ�ʼ������ͼƬ�ص��������
						cimg_forXY(blendedImg, x, y)
						{
							if (x > blendedImg._width - lastblendedImg._width
								&& x < blendedImg._width
								&& y < lastblendedImg._height)
							{
								//�������ѡ��ʹ�ò�ֵ���߲���ֵ����ֵ�н��䣬���ǿ��ܻ�ͺ�ɫ�����ص���
								//��ֵ�Ļ���������Ļ�����ı�
								//���ǲ���ֵ���е�ͻأ�����Ը��������x������ʼ���

								//������Ϊ�ص������ֵ,��ʱ������ԣ����ڽ��书�ܷ���ʱ����Զ����������ֵ
								//(srcImgB._width - leftmatchVec) Ϊͼ��b��δ�غϵ�����
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
								//��b�����ֵҲ���ӽ�ȥ���ⲿ�ֵ�ͼ����δ���в�ֵ����
								else if (x >= blendedImg._width - lastblendedImg._width
									&& x <= (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width)
									&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
									&& y < blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B���ڲ���
									blendedImg(x, y, 0, 0) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
								}
								else {    //A�������ƴ�ӺõĶ��ڲ���
									blendedImg(x, y, 0, 0) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 2);
								}
							}
							
							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&& x <= blendedImg._width - lastblendedImg._width
								&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
								&& y < blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B���ڲ���
								blendedImg(x, y, 0, 0) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
							}
							else {    //��ɫ����
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
			//��ͼ��ƴ�ӵ�ʱ����Ҫ��������ӿռ�
			//��ƴ��ͼ����ԭ���Ŀ����
			//�жϸ�ͼ���Ƿ�����߶ȵı仯
			if (lastcalureHeight == MaxDown || lastcalureHeight == MaxUp) 
			{
				//��������ͼ����Ҫ���Ӹ߶ȵĲ������������3-9�Ŵ���
				if (matchVecdy <= 0) {
					//��ͼ����Ե�һ��ͼ�����±�,��lastcalureHeightһ����������Ϊ����
					//����bͼ�񣨴�ƴ��ͼ�񣩵��ص�����Ϊ�ܵ�ͼ��߶ȼ�ȥbͼ��ĸ߶ȣ��õ���ʼ�߶�
					cimg_forXY(blendedImg, x, y) {
						if (x < lastblendedImg._width && y < lastblendedImg._height) {
							//�������ѡ��ʹ�ò�ֵ���߲���ֵ����ֵ�н��䣬���ǿ��ܻ�ͺ�ɫ�����ص���
							//��ֵ�Ļ���������Ļ�����ı�
							//���ǲ���ֵ���е�ͻأ�����Ը��������x������ʼ���
							//����ֵ����Ϊif (x >= abs(MaxLeft) + lastcalureWeight + srcImgB - leftmatchVec�� && y >= blendedImg._height - srcImgB._height)
							//blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
							// * (float)(x - (abs(MaxLeft) + lastcalureWeight)) / (float)abs(matchVecdx)
							//	+ (float)srcImgB(x - (abs(MaxLeft) + lastcalureWeight), y - (blendedImg._height - srcImgB._height), 0, 0)
							//	* (float)(x - (abs(MaxLeft) + lastcalureWeight)) / (float)abs(matchVecdx);
							//������Ϊ�ص������ֵ,��ʱ������ԣ����ڽ��书�ܷ���ʱ����Զ����������ֵ
							//(srcImgB._width - leftmatchVec) Ϊͼ��b��δ�غϵ�����
							if (x >= abs(MaxLeft) + (lastcalureWeight) + (srcImgB._width - leftmatchVec) 
								&&x <= abs(MaxLeft) + (lastcalureWeight)+ srcImgB._width 
								&& y >= blendedImg._height - srcImgB._height) {    //���
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
							//��b�����ֵҲ���ӽ�ȥ���ⲿ�ֵ�ͼ����δ���в�ֵ����
							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&&x<= (abs(MaxLeft) + (lastcalureWeight) + srcImgB._width)
								&& y >= blendedImg._height - srcImgB._height 
								&& y < blendedImg._height) {    //B���ڲ���
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 2);
							}
							else {    //A�������ƴ�ӺõĶ��ڲ���
								blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
								blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
								blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
							}

						}
						//y����Ӧ���������ͼ����±ߣ��ұ�
						else if (x >= (abs(MaxLeft) + (lastcalureWeight))
							&& x <= (abs(MaxLeft) + (lastcalureWeight)) + srcImgB._width
							&& y >= blendedImg._height - srcImgB._height && y < blendedImg._height) {    //B���ڲ���
							blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 2);
						}
						else {    //��ɫ����
							blendedImg(x, y, 0, 0) = 0;
							blendedImg(x, y, 0, 1) = 0;
							blendedImg(x, y, 0, 2) = 0;
						}
					}
				}
				//3-11���޸�
				else {
					//matchVec.dy > 0; �Ҳ�ͼƬ��Ҫ�������ƶ�
					//��ͼ����Ե�һ��ͼ�����ϱ�,��lastcalureHeightһ����������Ϊ����
					cimg_forXY(blendedImg, x, y) {
						if (x < lastblendedImg._width && y >= blendedImg._height - lastblendedImg._height) {
							//�������ѡ��ʹ�ò�ֵ���߲���ֵ����ֵ�н��䣬���ǿ��ܻ�ͺ�ɫ�����ص���
							//��ֵ�Ļ���������Ļ�����ı�
							//���ǲ���ֵ���е�ͻأ�����Ը��������x������ʼ���
							//����ֵ����Ϊ
							if (x >= abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec) 
								&&x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y < srcImgB._height) {    //���
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
							else {    //A���ڲ���
								blendedImg(x, y, 0, 0) = lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 0);
								blendedImg(x, y, 0, 1) = lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 1);
								blendedImg(x, y, 0, 2) = lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 2);
							}
						}
						else if (x >= (abs(MaxLeft) + (lastcalureWeight)) 
							&& x <= (abs(MaxLeft) + (lastcalureWeight)) + srcImgB._width
							&& y < srcImgB._height) {    //B���ڲ���
							blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 2);
						}
						else {    //��ɫ����
							blendedImg(x, y, 0, 0) = 0;
							blendedImg(x, y, 0, 1) = 0;
							blendedImg(x, y, 0, 2) = 0;
						}
					}
				}
			}
			else 
			{
				//��������ͼ����Ҫ���Ӹ߶ȵĲ������������3-11���ع�����

				if (matchVecdy <= 0) 
				{
				
					//��ƴ�ӵ�ͼƬ�����ƥ���ͼ�����������ƶ�
					//������ͼ����Ҫ��չ�߶�,����Ҫ��չ���
					if (lastcalureHeight > 0)
					{
						//��ͼƬ��Ե�һ��ͼ�����ϱߣ��ص�����Ϊ�϶����ȥ��ƫ�ƿ�ʼ����ƴ�����ͼ�����
						cimg_forXY(blendedImg, x, y)
						{
							if (x < lastblendedImg._width && y < lastblendedImg._height) 
							{
								//�������ѡ��ʹ�ò�ֵ���߲���ֵ����ֵ�н��䣬���ǿ��ܻ�ͺ�ɫ�����ص���
								//��ֵ�Ļ���������Ļ�����ı�
								//���ǲ���ֵ���е�ͻأ�����Ը��������x������ʼ���

								//������Ϊ�ص������ֵ,��ʱ������ԣ����ڽ��书�ܷ���ʱ����Զ����������ֵ
								//(srcImgB._width - leftmatchVec) Ϊͼ��b��δ�غϵ�����
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
								//��b�����ֵҲ���ӽ�ȥ���ⲿ�ֵ�ͼ����δ���в�ֵ����
								else if (x >= (abs(MaxLeft) + (lastcalureWeight)) 
									&& x <= (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width )
									&& y >= abs(MaxUp - lastcalureHeight) 
									&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B���ڲ���
									blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight) ), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight) ), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight) ), 0, 2);
								}
								else {    //A�������ƴ�ӺõĶ��ڲ���
									blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
								}
							}
							//y����Ӧ���������ͼ����±ߣ��ұ�,�������ﲻ�����У�������������ϱ�ѭ��
							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&& x <= (abs(MaxLeft) + (lastcalureWeight)) + srcImgB._width
								&& y >= abs(MaxUp - lastcalureHeight) 
								&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B���ڲ���
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 2);
							}
							else {    //��ɫ����
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}
					else 
					{
						//��ͼƬ��Ե�һ��ͼ�����±ߣ��ص�����Ϊ�¶����ȥ��ƫ�ƿ�ʼ������ͼƬ�ص��������
						cimg_forXY(blendedImg, x, y)
						{
							if (x < lastblendedImg._width && y < lastblendedImg._height)
							{
								//�������ѡ��ʹ�ò�ֵ���߲���ֵ����ֵ�н��䣬���ǿ��ܻ�ͺ�ɫ�����ص���
								//��ֵ�Ļ���������Ļ�����ı�
								//���ǲ���ֵ���е�ͻأ�����Ը��������x������ʼ���

								//������Ϊ�ص������ֵ,��ʱ������ԣ����ڽ��书�ܷ���ʱ����Զ����������ֵ
								//(srcImgB._width - leftmatchVec) Ϊͼ��b��δ�غϵ�����
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
								//��b�����ֵҲ���ӽ�ȥ���ⲿ�ֵ�ͼ����δ���в�ֵ����
								else if (x >= (abs(MaxLeft) + (lastcalureWeight)) 
									&& x <= (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width )
									&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
									&& y < blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B���ڲ���
									blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
								}
								else {    //A�������ƴ�ӺõĶ��ڲ���
									blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
								}


							}
							//y����Ӧ���������ͼ����±ߣ��ұ�
							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&& x <= (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width)
								&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
								&& y < blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B���ڲ���
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
							}
							else {    //��ɫ����
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}

					
				}
				else 
				{
					//��ƴ�ӵ�ͼƬ�����ƥ��ĵ�ͼ�����������ƶ�
					//������ͼ����Ҫ��չ�߶�,����Ҫ��չ���
					if (lastcalureHeight > 0)
					{
						//��ͼƬ��Ե�һ��ͼ�����ϱߣ��ص�����Ϊ�϶����ȥ��ƫ�ƿ�ʼ����ƴ�����ͼ�����
						cimg_forXY(blendedImg, x, y)
						{
							if (x < lastblendedImg._width && y < lastblendedImg._height)
							{
								//�������ѡ��ʹ�ò�ֵ���߲���ֵ����ֵ�н��䣬���ǿ��ܻ�ͺ�ɫ�����ص���
								//��ֵ�Ļ���������Ļ�����ı�
								//���ǲ���ֵ���е�ͻأ�����Ը��������x������ʼ���

								//������Ϊ�ص������ֵ,��ʱ������ԣ����ڽ��书�ܷ���ʱ����Զ����������ֵ
								//(srcImgB._width - leftmatchVec) Ϊͼ��b��δ�غϵ�����
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
								//��b�����ֵҲ���ӽ�ȥ���ⲿ�ֵ�ͼ����δ���в�ֵ����
								else if (x >= (abs(MaxLeft) + (lastcalureWeight)) 
									&& x <= (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width )
									&& y >= abs(MaxUp - lastcalureHeight) 
									&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B���ڲ���
									blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 2);
								}
								else {    //A�������ƴ�ӺõĶ��ڲ���
									blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
								}
							}
							//y����Ӧ���������ͼ����±ߣ��ұ�,�������ﲻ�����У�������������ϱ�ѭ��
							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&& x <= (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width)
								&& y >= abs(MaxUp - lastcalureHeight)
								&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B���ڲ���
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 2);
							}
							else {    //��ɫ����
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}
					else
					{
						//��ͼƬ��Ե�һ��ͼ�����±ߣ��ص�����Ϊ�¶����ȥ��ƫ�ƿ�ʼ������ͼƬ�ص��������
						cimg_forXY(blendedImg, x, y)
						{
							if (x < lastblendedImg._width && y < lastblendedImg._height)
							{
								//�������ѡ��ʹ�ò�ֵ���߲���ֵ����ֵ�н��䣬���ǿ��ܻ�ͺ�ɫ�����ص���
								//��ֵ�Ļ���������Ļ�����ı�
								//���ǲ���ֵ���е�ͻأ�����Ը��������x������ʼ���

								//������Ϊ�ص������ֵ,��ʱ������ԣ����ڽ��书�ܷ���ʱ����Զ����������ֵ
								//(srcImgB._width - leftmatchVec) Ϊͼ��b��δ�غϵ�����
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
								//��b�����ֵҲ���ӽ�ȥ���ⲿ�ֵ�ͼ����δ���в�ֵ����
								else if (x >= (abs(MaxLeft) + (lastcalureWeight)) 
									&& x <= (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width )
									&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) -  srcImgB._height
									&& y < blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B���ڲ���
									blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
								}
								else {    //A�������ƴ�ӺõĶ��ڲ���
									blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
								}


							}
							//y����Ӧ���������ͼ����±ߣ��ұ�
							//һ�㲻����ʹ��
							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&& x <= (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width)
								&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
								&& y < blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B���ڲ���
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
							}
							else {    //��ɫ����
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
		//��Ҫƴ�ӵ�ͼ�����ұ�
		//�жϵ�ǰͼ���������һ��ͼ���������ƶ����������ƶ�
		//С��0�������ƶ�
		//����0�������ƶ�
		//��ʽ��dy1 = img2.y - img1.y
		//dy2 = img3.y - img2.y;
		//img3�����img1:
		//dy = dy1+dy2 = img3.y - img1.y;
		//�����ȼ���ƫ����������������һ��ͼ�������ϻ�������

		lastcalureHeight += matchVecdy;
		if (lastcalureHeight <= 0)
		{
			//�����Ƚ�˭��С
			//�ж���������µ�ƫ���������С�����������������У�������Ҫ����������
			//������ڵĻ����͸����µ�������ͬʱ�������µ�ֵ
			MaxDown = MaxDown < lastcalureHeight ? MaxDown : lastcalureHeight;

		}
		else//�����ƶ� 
		{
			//�����Ƚ�˭���
			//�ж���������ϵ�ƫ���������С�����������������У�������Ҫ����������
				//������ڵĻ����͸����ϵ�������ͬʱ�������ϵ�ֵ
			MaxUp = MaxUp > lastcalureHeight ? MaxUp : lastcalureHeight;
		}

		//�������ǰ��ͼ������ڵ�һ��ͼ�����ƫ���˶���
		lastcalureWeight = lastcalureWeight + srcImgB._width - matchVecdx;
		if (lastcalureWeight > 0)
			MaxRight = MaxRight > lastcalureWeight ? MaxRight : lastcalureWeight;
		else
			MaxLeft = MaxLeft < lastcalureWeight ? MaxLeft : lastcalureWeight;

		//��ƴ�ӵ�ͼƬ��ͼ����ұ�
		//-----------------------�ϲ��Ĵ��ڴ�Сλ��-----------------------------------------
		//dxΪ����ͼ���������֮��ĺ�Ĳ�ֵ
		//dyΪ����ͼ���������֮������Ĳ�ֵ
		//�ж��Ƿ���Ҫ���ӻ����ĳ���
		//�������ԭͼ�񣬼���������ֵͬʱ����������ֵ
		//������һ��ͼ���ж��Ƿ������еĸ߶�
		//���ӳ�ʼ��ͼ��Ĵ�С
		//�����ԭ��Ϊ���п�ȵ���ȡ��ԭ���Ĵ���ͼ��Ŀ�ȣ�
		//������ƴ����ߵ�ͼ��ʱ�������Ҫ������ͼ������
		//blendedImg = CImg<int>(lastblendedImg._width + srcImgB._width - matchVecdx,
		//	srcImgB._height + abs(MaxDown) + abs(MaxUp), 1, 3, 0);

		//
		blendedImg = CImg<int>(oriImageWidth + abs(MaxLeft) + abs(MaxRight),
			srcImgB._height + abs(MaxDown) + abs(MaxUp), 1, 3, 0);


		//�ж��¼����ͼ�����������һ��ͼ�����ƶ��������ƶ�
		//�ж�x�������ƴ�Ӻû����ǵ�һ��ͼ�������
		//������ͼ��ĵ�ǡ��������ͼ����ص�֮������Ҫ����ֵ����
		//�����ص�����Ĳ�ֵ��ʱ�򣬽��غ�����ı���ͬʱ�����ص�����Ŀ��
		//�˴���Ҫ��������
		
		if (lastcalureWeight == MaxRight)
		{
			//��ͼ��ƴ�ӵ�ʱ����Ҫ���ұ����ӿռ�
			//���Ҵ�ƴ��ͼ�����ұ�
			//�жϸ�ͼ���Ƿ�����߶ȵı仯
			if (lastcalureHeight == MaxDown || lastcalureHeight == MaxUp)
			{
				//��������ͼ����Ҫ���Ӹ߶ȵĲ������������3-9�Ŵ���
				if (matchVecdy <= 0) {
					//��ͼ����Ե�һ��ͼ�����±�,��lastcalureHeightһ����������Ϊ����
					//����bͼ�񣨴�ƴ��ͼ�񣩵��ص�����Ϊ�ܵ�ͼ��߶ȼ�ȥbͼ��ĸ߶ȣ��õ���ʼ�߶�
					cimg_forXY(blendedImg, x, y) {
					if (x < lastblendedImg._width && y < lastblendedImg._height) {
						if (x >= abs(MaxLeft) + (lastcalureWeight)
							&& x <= abs(MaxLeft) + (lastcalureWeight)+matchVecdx
							&& y >= blendedImg._height-srcImgB._height
							&& y <= blendedImg._height - srcImgB._height + (srcImgB._height -abs(matchVecdy))
							) {    //���
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
						//y����Ӧ���������ͼ����±ߣ��ұ�
						else if (x >= abs(MaxLeft) + (lastcalureWeight)
							&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
							&& y >= blendedImg._height - srcImgB._height && y < blendedImg._height) {    //B���ڲ���
							blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 2);
						}
						else {    //A�������ƴ�ӺõĶ��ڲ���
							blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
							blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
							blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
						}

					}
					//y����Ӧ���������ͼ����±ߣ��ұ�
					else if (x >= abs(MaxLeft) + (lastcalureWeight)
						&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
						&& y >= blendedImg._height - srcImgB._height && y < blendedImg._height) {    //B���ڲ���
						blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 0);
						blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 1);
						blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 2);
					}
					else {    //��ɫ����
						blendedImg(x, y, 0, 0) = 0;
						blendedImg(x, y, 0, 1) = 0;
						blendedImg(x, y, 0, 2) = 0;
					}
					}
				}
				//3-11���޸�
				else {    
					//matchVec.dy > 0; �Ҳ�ͼƬ��Ҫ�������ƶ�
					//��ͼ����Ե�һ��ͼ�����ϱ�,��lastcalureHeightһ����������Ϊ����
					cimg_forXY(blendedImg, x, y) {
						if (x < lastblendedImg._width 
							&& y >= blendedImg._height - lastblendedImg._height) {
							if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+matchVecdx
								&& y > srcImgB._height - abs(matchVecdy)
								&& y < srcImgB._height) {    //���
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
								&& y < srcImgB._height) {    //B���ڲ���
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 2);
							}
							else {    //A���ڲ���
								blendedImg(x, y, 0, 0) = lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 0);
								blendedImg(x, y, 0, 1) = lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 1);
								blendedImg(x, y, 0, 2) = lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 2);
							}
						}
						else if (x >= abs(MaxLeft) + (lastcalureWeight)
							&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
							&& y < srcImgB._height) {    //B���ڲ���
							blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 2);
						}
						else {    //��ɫ����
							blendedImg(x, y, 0, 0) = 0;
							blendedImg(x, y, 0, 1) = 0;
							blendedImg(x, y, 0, 2) = 0;
						}
					}
				}
			}
			else
			{
				//���ڲ���Ҫ���Ӹ߶ȵĲ��֣�����3-11�Ŵ������д���
				if (matchVecdy <= 0) 
				{
					//��ƴ�ӵ�ͼƬ�����ƥ���ͼ�����������ƶ�
					//������ͼ����Ҫ��չ�߶�
					if (lastcalureHeight > 0)
					{
						//��ͼƬ��Ե�һ��ͼ�����ϱߣ��ص�����Ϊ�϶����ȥ��ƫ�ƿ�ʼ����ƴ�����ͼ�����
						cimg_forXY(blendedImg, x, y) {
							if (x < lastblendedImg._width && y < lastblendedImg._height)
							{
								//Ҳ�ɸ���Ϊ�����ӣ�����ʣ����±߽�����ֵ�����ǿ��ܻ���ʧ��Ч������
								//if (x >= (lastblendedImg._width - matchVecdx) && y >= abs(MaxUp - lastcalureHeight)
								//	&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height - abs(matchVecdy))
								if (x >= abs(MaxLeft) + (lastcalureWeight)
									&& x <= abs(MaxLeft) + (lastcalureWeight)+matchVecdx
									&& y >= abs(MaxUp - lastcalureHeight)
									 && y <= abs(MaxUp - lastcalureHeight) + srcImgB._height - abs(matchVecdy)) {    //���
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
									&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B���ڲ���
									blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 2);
								}
								else {    //A�������ƴ�ӺõĶ��ڲ���
									blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
								}
							}
							else if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y >= abs(MaxUp - lastcalureHeight)
								&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B���ڲ���
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 2);
							}
							else {    //��ɫ����
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}

					else
					{
						//��ͼƬ��Ե�һ��ͼ�����±ߣ��ص�����Ϊ�¶��߼�ȥ��ƫ�ƣ��ó��ڶ���ͼ��Ľ�β���ؾ�������ͼ��ľ���
						//lastblendedImg._height - abs(MaxDown - lastcalureHeight)Ϊ�ڶ���ͼ�����ƴ��ʱ��ĸ߶�
						cimg_forXY(blendedImg, x, y) {
						if (x < lastblendedImg._width && y < lastblendedImg._height) 
						{
							//Ҳ�ɸ���Ϊ�����ӣ�����ʣ����±߽�����ֵ�����ǿ��ܻ���ʧ��Ч������
							//if (x >= (lastblendedImg._width - matchVecdx) && y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight)
							//-srcImgB._height && y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - matchVecdy)
							if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+matchVecdx
								&& y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight)- srcImgB._height 
								&& y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - abs(matchVecdy))
							{    //���
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
						{    //B���ڲ���
							blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
						}
						else {    //A�������ƴ�ӺõĶ��ڲ���
							blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
							blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
							blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
						}
						}
						else if (x >= abs(MaxLeft) + (lastcalureWeight)
							&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
							&& y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
							&& y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B���ڲ���
							blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
						}
						else {    //��ɫ����
							blendedImg(x, y, 0, 0) = 0;
							blendedImg(x, y, 0, 1) = 0;
							blendedImg(x, y, 0, 2) = 0;
						}
						}
					}
					
				}
				else 
				{
					//��ƴ�ӵ�ͼƬ�����ƥ���ͼ�����������ƶ�
					//������ͼ����Ҫ��չ�߶�

					if (lastcalureHeight > 0)
					{
						//��ͼƬ��Ե�һ��ͼ�����ϱߣ��ص�����Ϊ�϶����ȥ��ƫ�ƿ�ʼ����ƴ�����ͼ�����
						cimg_forXY(blendedImg, x, y) {
							if (x < lastblendedImg._width && y < lastblendedImg._height)
							{
								//Ҳ�ɸ���Ϊ�����ӣ�����ʣ����±߽�����ֵ�����ǿ��ܻ���ʧ��Ч������
								//if (x >= (lastblendedImg._width - matchVecdx) && y >= abs(MaxUp - lastcalureHeight)
								//	&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height - abs(matchVecdy))
								if (x >= abs(MaxLeft) + (lastcalureWeight)
									&& x <= abs(MaxLeft) + (lastcalureWeight)+matchVecdx
									&& y >= abs(MaxUp - lastcalureHeight) + abs(matchVecdy)
									&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //���
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
									&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B���ڲ���
									blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 2);
								}
								else {    //A�������ƴ�ӺõĶ��ڲ���
									blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
								}
							}
							else if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y >= abs(MaxUp - lastcalureHeight)
								&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B���ڲ���
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 2);
							}
							else {    //��ɫ����
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}

					else
					{
						//��ͼƬ��Ե�һ��ͼ�����±ߣ��ص�����Ϊ�¶��߼�ȥ��ƫ�ƣ��ó��ڶ���ͼ��Ľ�β���ؾ�������ͼ��ľ���
						//lastblendedImg._height - abs(MaxDown - lastcalureHeight)Ϊ�ڶ���ͼ�����ƴ��ʱ��ĸ߶�
						cimg_forXY(blendedImg, x, y) {
							if (x < lastblendedImg._width && y < lastblendedImg._height)
							{
								//Ҳ�ɸ���Ϊ�����ӣ�����ʣ����±߽�����ֵ�����ǿ��ܻ���ʧ��Ч������
								//if (x >= (lastblendedImg._width - matchVecdx) && y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight)
								//-srcImgB._height && y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - matchVecdy)
								if (x >= abs(MaxLeft) + (lastcalureWeight)
									&& x <= abs(MaxLeft) + (lastcalureWeight)+matchVecdx 
									&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight)- (srcImgB._height - matchVecdy)
									&& y <= blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //���
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
									&& y <= blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B���ڲ���
									blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
								}
								else {    //A�������ƴ�ӺõĶ��ڲ���
									blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
								}
							}
							else if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
								&& y <= blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B���ڲ���
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
							}
							else {    //��ɫ����
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
			//��ͼ��ƴ�ӵ�ʱ����Ҫ���ұ������ӿռ�
			//��ƴ��ͼ����ԭ���Ŀ����
			//�жϸ�ͼ���Ƿ�����߶ȵı仯
			if (lastcalureHeight == MaxDown || lastcalureHeight == MaxUp)
		{
			//��������ͼ����Ҫ���Ӹ߶ȵĲ������������3-14�Ŵ���
			if (matchVecdy <= 0) {
				//��ͼ����Ե�һ��ͼ�����±�,��lastcalureHeightһ����������Ϊ����
				//����bͼ�񣨴�ƴ��ͼ�񣩵��ص�����Ϊ�ܵ�ͼ��߶ȼ�ȥbͼ��ĸ߶ȣ��õ���ʼ�߶�
				cimg_forXY(blendedImg, x, y) {
					if (x < lastblendedImg._width && y < lastblendedImg._height) {
						if (x >= abs(MaxLeft) + (lastcalureWeight)
							&& x <= abs(MaxLeft) + (lastcalureWeight) + matchVecdx
							&& y >= blendedImg._height - srcImgB._height
							&& y <= blendedImg._height - srcImgB._height + abs(matchVecdy)
							) {    //���
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
							//Bͼ����Aͼ���еĲ��ֲ���Ҫ�����ֵ�Ĳ���
							blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 2);
						}
						else {    //A�������ƴ�ӺõĶ��ڲ���
							blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
							blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
							blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
						}

					}
					//y����Ӧ���������ͼ����±ߣ��ұ�
					else if (x >= abs(MaxLeft) + (lastcalureWeight)
						&& x<= abs(MaxLeft) + (lastcalureWeight) + srcImgB._width
						&& y >= blendedImg._height - srcImgB._height
						&& y < blendedImg._height) {    //B���ڲ���
						blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 0);
						blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 1);
						blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 2);
					}
					else {    //��ɫ����
						blendedImg(x, y, 0, 0) = 0;
						blendedImg(x, y, 0, 1) = 0;
						blendedImg(x, y, 0, 2) = 0;
					}
				}
			}
			//3-11���޸�
			else {
				//matchVec.dy > 0; �Ҳ�ͼƬ��Ҫ�������ƶ�
				//��ͼ����Ե�һ��ͼ�����ϱ�,��lastcalureHeightһ����������Ϊ����
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
						{    //���
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
							//Bͼ����Aͼ���еĲ��ֲ���Ҫ�����ֵ�Ĳ���
							blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y , 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y , 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y , 0, 2);
						}

						else {    //A���ڲ���
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
						) {    //B���ڲ���
						blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 0);
						blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 1);
						blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 2);
					}
					else {    //��ɫ����
						blendedImg(x, y, 0, 0) = 0;
						blendedImg(x, y, 0, 1) = 0;
						blendedImg(x, y, 0, 2) = 0;
					}
				}
			}
		}
		else
		{
			//���ڲ���Ҫ���Ӹ߶ȵĲ��֣�����3-14�Ŵ������д���
			if (matchVecdy <= 0)
			{
				//��ƴ�ӵ�ͼƬ�����ƥ���ͼ�����������ƶ�
				//������ͼ����Ҫ��չ�߶�
				if (lastcalureHeight > 0)
				{
					//��ͼƬ��Ե�һ��ͼ�����ϱߣ��ص�����Ϊ�϶����ȥ��ƫ�ƿ�ʼ����ƴ�����ͼ�����
					cimg_forXY(blendedImg, x, y) {
						if (x < lastblendedImg._width && y < lastblendedImg._height)
						{
							//Ҳ�ɸ���Ϊ�����ӣ�����ʣ����±߽�����ֵ�����ǿ��ܻ���ʧ��Ч������
							//if (x >= (lastblendedImg._width - matchVecdx) && y >= abs(MaxUp - lastcalureHeight)
							//	&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height - abs(matchVecdy))
							if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+matchVecdx
								&& y >= abs(MaxUp - lastcalureHeight)
								&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height - abs(matchVecdy)) {    //���
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
							else {    //A�������ƴ�ӺõĶ��ڲ���
								blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
								blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
								blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
							}
						}
						else if ((x >= abs(MaxLeft) + (lastcalureWeight)
							&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
							&& y >= abs(MaxUp - lastcalureHeight)
							&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height)) {    //B���ڲ���
							blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 2);
						}
						else {    //��ɫ����
							blendedImg(x, y, 0, 0) = 0;
							blendedImg(x, y, 0, 1) = 0;
							blendedImg(x, y, 0, 2) = 0;
						}
					}
				}

				else
				{
					//��ͼƬ��Ե�һ��ͼ�����±ߣ��ص�����Ϊ�¶��߼�ȥ��ƫ�ƣ��ó��ڶ���ͼ��Ľ�β���ؾ�������ͼ��ľ���
					//lastblendedImg._height - abs(MaxDown - lastcalureHeight)Ϊ�ڶ���ͼ�����ƴ��ʱ��ĸ߶�
					cimg_forXY(blendedImg, x, y) {
						if (x < lastblendedImg._width && y < lastblendedImg._height)
						{
							//Ҳ�ɸ���Ϊ�����ӣ�����ʣ����±߽�����ֵ�����ǿ��ܻ���ʧ��Ч������
							//if (x >= (lastblendedImg._width - matchVecdx) && y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight)
							//-srcImgB._height && y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - matchVecdy)
							if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+matchVecdx
								&& y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight)- srcImgB._height 
								&& y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - abs(matchVecdy))
							{    //���
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
							else {    //A�������ƴ�ӺõĶ��ڲ���
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
						{	//b�����ֵ
							blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
						}
						else {    //��ɫ����
							blendedImg(x, y, 0, 0) = 0;
							blendedImg(x, y, 0, 1) = 0;
							blendedImg(x, y, 0, 2) = 0;
						}
					}
				}

			}
			else
			{
				//��ƴ�ӵ�ͼƬ�����ƥ���ͼ�����������ƶ�
				//������ͼ����Ҫ��չ�߶�

				if (lastcalureHeight > 0)
				{
					//��ͼƬ��Ե�һ��ͼ�����ϱߣ��ص�����Ϊ�϶����ȥ��ƫ�ƿ�ʼ����ƴ�����ͼ�����
					cimg_forXY(blendedImg, x, y) {
						if (x < lastblendedImg._width && y < lastblendedImg._height)
						{
							
							if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+matchVecdx
								&& y >= abs(MaxUp - lastcalureHeight) + abs(matchVecdy)
								&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height
								) {    //���
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
							else {    //A�������ƴ�ӺõĶ��ڲ���
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
							//B���ڵĲ���
							blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 2);
						}
						else {    //��ɫ����
							blendedImg(x, y, 0, 0) = 0;
							blendedImg(x, y, 0, 1) = 0;
							blendedImg(x, y, 0, 2) = 0;
						}
					}
				}

				else
				{
					//��ͼƬ��Ե�һ��ͼ�����±ߣ��ص�����Ϊ�¶��߼�ȥ��ƫ�ƣ��ó��ڶ���ͼ��Ľ�β���ؾ�������ͼ��ľ���
					//lastblendedImg._height - abs(MaxDown - lastcalureHeight)Ϊ�ڶ���ͼ�����ƴ��ʱ��ĸ߶�
					cimg_forXY(blendedImg, x, y) {
						if (x < lastblendedImg._width && y < lastblendedImg._height)
						{
							//Ҳ�ɸ���Ϊ�����ӣ�����ʣ����±߽�����ֵ�����ǿ��ܻ���ʧ��Ч������
							//if (x >= (lastblendedImg._width - matchVecdx) && y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight)
							//-srcImgB._height && y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - matchVecdy)
							if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+matchVecdx
								&& y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - abs(matchVecdy)
								&& y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight)) 
							{    //���
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
							else {    //A�������ƴ�ӺõĶ��ڲ���
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
						{	//B������ֵ���ӽ���
							blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
						}
						else {    //��ɫ����
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

//��ѷ���߶�ֱ����ںϲ���
//2019-4-17��Ҫ����ֱ��ʴ�����ȡ��������Ҫ����Ϊ������ÿ�θ�ֵ��ʱ������ظ����������
//���벻��Ҫ��ѭ��
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
	//������ͼ��֮���x��y��ƫ�������д洢
	SaveSequentialImagesDifference(_filenameA, _filenameB, matchVecdx, matchVecdy);

	//��ȡͼ��
	srcImgA.load(_filenameA);
	srcImgB.load(_filenameB);

	//�жϴ������һ��ͼ����Ƿ�Ϊ��
	//��Ϊ������丳��ͼ��a����ֵ
	if (lastblendedImg == NULL)
	{
		lastblendedImg = srcImgA;
	}

	//�ж��������ʼ��ͼƬ
	if (StitchPictureNum == 0)
	{
		//���ǵĻ�����ʼͼƬ�Ŀ�Ⱥ͸߶ȶ���������
		oriImageWidth = lastblendedImg._width;
		oriImageHeight = lastblendedImg._height;
	}

	//�жϸ�ͼ���ƴ��ͼ���Ƿ������
	//�ǵĻ����ϼ�¼�����ҽ���ͼ����չ�ĵ���
	if (matchVecdx > srcImgB._width)
	{
		//2019-03-09�޸�����
		//��Ҫƴ�ӵ�ͼƬ�����
		//�µļ�¼ֵ���еǼ��滻
		//����ʵ�� = ��ƴ��ͼ�� - dx/2��*2��
		//���Է���ֽ2��ƴ��ȫ��ͼƬ
		//leftmatchVec�������غ�����
		//matchVecxΪ���ʣ��Ĵ�ƴ�����
		int leftmatchVec = (srcImgB._width * 2 - matchVecdx);

		//��ƴ���ұ�ͼ��ͬ���ǣ���Ҫ��dy��ȡ��ֵ
		//�жϵ�ǰͼ���������һ��ͼ���������ƶ����������ƶ�
		//С��0�������ƶ�
		//����0�������ƶ�
		//��ʽ��dy1 = img2.y - img1.y
		//dy2 = img3.y - img2.y;
		//img3�����img1:
		//dy = dy1+dy2 = img3.y - img1.y;
		//�����ȼ���ƫ����������������һ��ͼ�������ϻ�������
		//ȡ����
		//ͬʱ��ƫ����ȡ������ת��Ϊ��ȷ��ͼƬ����λ�����ƫ����
		//matchVecdy = 0 - matchVecdy;
		lastcalureHeight = lastcalureHeight + (matchVecdy);

		if (lastcalureHeight <= 0)
		{
			//�����Ƚ�˭��С
			//�ж���������µ�ƫ���������С�����������������У�������Ҫ����������
			//������ڵĻ����͸����µ�������ͬʱ�������µ�ֵ
			MaxDown = MaxDown < lastcalureHeight ? MaxDown : lastcalureHeight;

		}
		else//�����ƶ� 
		{
			//�����Ƚ�˭���
			//�ж���������ϵ�ƫ���������С�����������������У�������Ҫ����������
				//������ڵĻ����͸����ϵ�������ͬʱ�������ϵ�ֵ
			MaxUp = MaxUp > lastcalureHeight ? MaxUp : lastcalureHeight;
		}

		//�������ǰ��ͼ������ڵ�һ��ͼ�����ƫ���˶���
		lastcalureWeight = lastcalureWeight - (srcImgB._width - leftmatchVec);
		if (lastcalureWeight > 0)
			MaxRight = MaxRight > lastcalureWeight ? MaxRight : lastcalureWeight;
		else
			MaxLeft = MaxLeft < lastcalureWeight ? MaxLeft : lastcalureWeight;

		//��ƴ�ӵ�ͼƬ��ͼ������
		//��ߵ�ͼ����������ķ����ƫ��ֵ���෴��
		//dx�����µı�ʾ����
		//-----------------------�ϲ��Ĵ��ڴ�Сλ��-----------------------------------------
		//dxΪ����ͼ���������֮��ĺ�Ĳ�ֵ
		//dyΪ����ͼ���������֮������Ĳ�ֵ
		//�ж��Ƿ���Ҫ���ӻ����ĳ���
		//�������ԭͼ�񣬼���������ֵͬʱ����������ֵ
		//������һ��ͼ���ж��Ƿ������еĸ߶�
		//���ӳ�ʼ��ͼ��Ĵ�С
		//�������ԭͼ�񣬼���������ֵͬʱ����������ֵ
		//�����ԭ��Ϊ���п�ȵ���ȡ��ԭ���Ĵ���ͼ��Ŀ�ȣ�
		//������ƴ����ߵ�ͼ��ʱ�������Ҫ������ͼ������
		//blendedImg = CImg<int>(lastblendedImg._width + srcImgB._width - matchVecdx,
		//	srcImgB._height + abs(MaxDown) + abs(MaxUp), 1, 3, 0);
		blendedImg = CImg<int>(oriImageWidth + abs(MaxLeft) + abs(MaxRight),
			srcImgB._height + abs(MaxDown) + abs(MaxUp), 1, 3, 0);

		//�ж��¼����ͼ�����������һ��ͼ�����ƶ��������ƶ�
		//�ж�x�������ƴ�Ӻû����ǵ�һ��ͼ�������
		//������ͼ��ĵ�ǡ��������ͼ����ص�֮������Ҫ����ֵ����
		//�����ص�����Ĳ�ֵ��ʱ�򣬽��غ�����ı���ͬʱ�����ص�����Ŀ��
		//�˴���Ҫ��������
		//�ж��Ƿ����߿�ʼ��չͼ��
		if (lastcalureWeight == MaxLeft)
		{
			//��ͼ��ƴ�ӵ�ʱ����Ҫ��������ӿռ�
			//���Ҵ�ƴ��ͼ�������
			//�жϸ�ͼ���Ƿ�����߶ȵı仯
			if (lastcalureHeight == MaxDown || lastcalureHeight == MaxUp)
			{
				//��������ͼ����Ҫ���Ӹ߶ȵĲ��������

				if (matchVecdy <= 0) {
					//��ͼ����Ե�һ��ͼ�����±�,��lastcalureHeightһ����������Ϊ����
					//����bͼ�񣨴�ƴ��ͼ�񣩵��ص�����Ϊ�ܵ�ͼ��߶ȼ�ȥbͼ��ĸ߶ȣ��õ���ʼ�߶�
					//����Ҫע����ǣ�ͼ�����ʼλ�úʹ�ƴ��λ����ʼ��һ������ͬһ�������ϣ�����ʹA��ƴ��ͼƬ
					//������ͼ����ڲ������߽�ط�����ͼ
					CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameB, _filenameA, leftmatchVec,-matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
					cimg_forXY(blendedImg, x, y) {
						if (x >= blendedImg._width - lastblendedImg._width
							&& x < blendedImg._width
							&& y < lastblendedImg._height) {
							//�������ѡ��ʹ�ò�ֵ���߲���ֵ����ֵ�н��䣬���ǿ��ܻ�ͺ�ɫ�����ص���
							//��ֵ�Ļ���������Ļ�����ı�
							//���ǲ���ֵ���е�ͻأ�����Ը��������x������ʼ���
							//������Ϊ�ص������ֵ,��ʱ������ԣ����ڽ��书�ܷ���ʱ����Զ����������ֵ
							//Ϊͼ��b��δ�غϵ�����
							//abs(MaxLeft) + (lastcalureWeight)== 0�໥����
							if (x >= abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec)
								&& x < abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y >= blendedImg._height - srcImgB._height
								&& y < blendedImg._height - srcImgB._height + (srcImgB._height - abs(matchVecdy))
								)
							{    //���
								//3-12���·��֣�����ȫ���������˼·�޸�

								
								for (int k = 0;k < srcImgB._spectrum;k++)
								{
								blendedImg(x, y, 0, k) = rectImage(x - (abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec))
									, y - blendedImg._height - srcImgB._height, 0, k);
								}

							}
							else if (x >= blendedImg._width - lastblendedImg._width
								&& x < (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width - leftmatchVec)
								&& y >= blendedImg._height - srcImgB._height
								&& y < blendedImg._height) {    //B���ڲ���
								blendedImg(x, y, 0, 0) = srcImgB(x, y - (blendedImg._height - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x, y - (blendedImg._height - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x, y - (blendedImg._height - srcImgB._height), 0, 2);
							}
							else {    //A�������ƴ�ӺõĶ��ڲ���
								blendedImg(x, y, 0, 0) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 0);
								blendedImg(x, y, 0, 1) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 1);
								blendedImg(x, y, 0, 2) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 2);
							}

						}
						//y����Ӧ���������ͼ����±ߣ��ұ�
						else if (x >= (abs(MaxLeft) + (lastcalureWeight))
							&& x < blendedImg._width - lastblendedImg._width
							&& y >= blendedImg._height - srcImgB._height
							&& y < blendedImg._height) {    //B���ڲ���
							blendedImg(x, y, 0, 0) = srcImgB(x, y - (blendedImg._height - srcImgB._height), 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x, y - (blendedImg._height - srcImgB._height), 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x, y - (blendedImg._height - srcImgB._height), 0, 2);
						}
						else {    //��ɫ����
							blendedImg(x, y, 0, 0) = 0;
							blendedImg(x, y, 0, 1) = 0;
							blendedImg(x, y, 0, 2) = 0;
						}


					}
				}
				//3-11���޸�
				else {
					//matchVec.dy > 0;���ͼƬ��Ҫ�������ƶ�
					//��ͼ����Ե�һ��ͼ�����ϱ�,��lastcalureHeightһ����������Ϊ����
					CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameB, _filenameA, leftmatchVec, -matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
					cimg_forXY(blendedImg, x, y) {
						if (x >= blendedImg._width - lastblendedImg._width
							&& x < blendedImg._width
							&& y >= blendedImg._height - lastblendedImg._height
							&& y < blendedImg._height
							) {
							//�������ѡ��ʹ�ò�ֵ���߲���ֵ����ֵ�н��䣬���ǿ��ܻ�ͺ�ɫ�����ص���
							//��ֵ�Ļ���������Ļ�����ı�
							//���ǲ���ֵ���е�ͻأ�����Ը��������x������ʼ���
							//����ֵ����Ϊ
							if (x >= abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec)
								&& x < abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y >= abs(matchVecdy)
								&& y < srcImgB._height) 
							{    //���

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
							{	//B����
								blendedImg(x, y, 0, 0) = srcImgB(x, y, 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x, y, 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x, y, 0, 2);
							}
							else {    //A���ڲ���
								blendedImg(x, y, 0, 0) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y - (blendedImg._height - lastblendedImg._height), 0, 0);
								blendedImg(x, y, 0, 1) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y - (blendedImg._height - lastblendedImg._height), 0, 1);
								blendedImg(x, y, 0, 2) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y - (blendedImg._height - lastblendedImg._height), 0, 2);
							}
						}
						else if (x >= (abs(MaxLeft) + (lastcalureWeight))
							&& x < blendedImg._width - lastblendedImg._width
							&& y < srcImgB._height) {    //B���ڲ���
							blendedImg(x, y, 0, 0) = srcImgB(x, y, 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x, y, 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x, y, 0, 2);
						}
						else {    //��ɫ����
							blendedImg(x, y, 0, 0) = 0;
							blendedImg(x, y, 0, 1) = 0;
							blendedImg(x, y, 0, 2) = 0;
						}
					}
				}
			}
			else
			{
				//��������ͼ����Ҫ���Ӹ߶ȵĲ������������3-11���ع�����
				//���Ǹ�ͼ����������ӿռ�仯
				if (matchVecdy <= 0)
				{

					//��ƴ�ӵ�ͼƬ�����ƥ���ͼ�����������ƶ�
					//������ͼ����Ҫ��չ�߶�,����Ҫ��չ���
					CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameB, _filenameA, leftmatchVec, -matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
					if (lastcalureHeight > 0)
					{
						//��ͼƬ��Ե�һ��ͼ�����ϱߣ��ص�����Ϊ�϶����ȥ��ƫ�ƿ�ʼ����ƴ�����ͼ�����
						
						cimg_forXY(blendedImg, x, y)
						{
							if (x >= blendedImg._width - lastblendedImg._width
								&& x < blendedImg._width
								&& y < lastblendedImg._height)
							{
								//�������ѡ��ʹ�ò�ֵ���߲���ֵ����ֵ�н��䣬���ǿ��ܻ�ͺ�ɫ�����ص���
								//��ֵ�Ļ���������Ļ�����ı�
								//���ǲ���ֵ���е�ͻأ�����Ը��������x������ʼ���

								//������Ϊ�ص������ֵ,��ʱ������ԣ����ڽ��书�ܷ���ʱ����Զ����������ֵ
								//(srcImgB._width - leftmatchVec) Ϊͼ��b��δ�غϵ�����
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
								//��b�����ֵҲ���ӽ�ȥ���ⲿ�ֵ�ͼ����δ���в�ֵ����
								else if (x >= blendedImg._width - lastblendedImg._width
									&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
									&& y >= abs(MaxUp - lastcalureHeight)
									&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B���ڲ���
									blendedImg(x, y, 0, 0) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 2);
								}
								else {    //A�������ƴ�ӺõĶ��ڲ���
									blendedImg(x, y, 0, 0) = lastblendedImg(x - blendedImg._width - lastblendedImg._width, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x - blendedImg._width - lastblendedImg._width, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x - blendedImg._width - lastblendedImg._width, y, 0, 2);
								}
							}
							//y����Ӧ���������ͼ����±ߣ��ұ�,�������ﲻ�����У�������������ϱ�ѭ��
							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&& x <= (abs(MaxLeft) + (lastcalureWeight)) + blendedImg._width - lastblendedImg._width
								&& y >= abs(MaxUp - lastcalureHeight)
								&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B���ڲ���
								blendedImg(x, y, 0, 0) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 2);
							}
							else {    //��ɫ����
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}
					else
					{
						//��ͼƬ��Ե�һ��ͼ�����±ߣ��ص�����Ϊ�¶����ȥ��ƫ�ƿ�ʼ������ͼƬ�ص��������
						CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameB, _filenameA, leftmatchVec, -matchVecdy, isLaplacianBlending, isDrawbestSeamLine);

						cimg_forXY(blendedImg, x, y)
						{
							if (x >= blendedImg._width - lastblendedImg._width
								&& x < blendedImg._width
								&& y < lastblendedImg._height)
							{
								//�������ѡ��ʹ�ò�ֵ���߲���ֵ����ֵ�н��䣬���ǿ��ܻ�ͺ�ɫ�����ص���
								//��ֵ�Ļ���������Ļ�����ı�
								//���ǲ���ֵ���е�ͻأ�����Ը��������x������ʼ���

								//������Ϊ�ص������ֵ,��ʱ������ԣ����ڽ��书�ܷ���ʱ����Զ����������ֵ
								//(srcImgB._width - leftmatchVec) Ϊͼ��b��δ�غϵ�����
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
								//��b�����ֵҲ���ӽ�ȥ���ⲿ�ֵ�ͼ����δ���в�ֵ����
								else if (x >= blendedImg._width - lastblendedImg._width
									&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
									&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
									&& y < blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B���ڲ���
									blendedImg(x, y, 0, 0) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
								}
								else {    //A�������ƴ�ӺõĶ��ڲ���
									blendedImg(x, y, 0, 0) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 2);
								}


							}
							//y����Ӧ���������ͼ����±ߣ��ұ�
							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&& x <= blendedImg._width - lastblendedImg._width
								&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
								&& y < blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B���ڲ���
								blendedImg(x, y, 0, 0) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
							}
							else {    //��ɫ����
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}

				}
				else
				{
					//��ƴ�ӵ�ͼƬ�����ƥ��ĵ�ͼ�����������ƶ�
					//������ͼ����Ҫ��չ�߶�,����Ҫ��չ���
					if (lastcalureHeight > 0)
					{
						CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameB, _filenameA, leftmatchVec, -matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
						//��ͼƬ��Ե�һ��ͼ�����ϱߣ��ص�����Ϊ�϶����ȥ��ƫ�ƿ�ʼ����ƴ�����ͼ�����
						cimg_forXY(blendedImg, x, y)
						{
							if (x > blendedImg._width - lastblendedImg._width
								&& x < blendedImg._width
								&& y < lastblendedImg._height)
							{
								//�������ѡ��ʹ�ò�ֵ���߲���ֵ����ֵ�н��䣬���ǿ��ܻ�ͺ�ɫ�����ص���
								//��ֵ�Ļ���������Ļ�����ı�
								//���ǲ���ֵ���е�ͻأ�����Ը��������x������ʼ���

								//������Ϊ�ص������ֵ,��ʱ������ԣ����ڽ��书�ܷ���ʱ����Զ����������ֵ
								//(srcImgB._width - leftmatchVec) Ϊͼ��b��δ�غϵ�����
								if (x >= abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec)
									&& x < abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
									&& y >= abs(MaxUp - lastcalureHeight) + abs(matchVecdy)
									&& y < abs(MaxUp - lastcalureHeight) + srcImgB._height
									)
								{
									//������Ҹо��е����⣬ò��û�н����޸�
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
								//��b�����ֵҲ���ӽ�ȥ���ⲿ�ֵ�ͼ����δ���в�ֵ����
								else if (x >= blendedImg._width - lastblendedImg._width
									&& x <= (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width)
									&& y >= abs(MaxUp - lastcalureHeight)
									&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B���ڲ���
									blendedImg(x, y, 0, 0) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 2);
								}
								else {    //A�������ƴ�ӺõĶ��ڲ���
									blendedImg(x, y, 0, 0) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 2);
								}
							}
							//y����Ӧ���������ͼ����±ߣ��ұ�,�������ﲻ�����У�������������ϱ�ѭ��
							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&& x <= blendedImg._width - lastblendedImg._width
								&& y >= abs(MaxUp - lastcalureHeight)
								&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B���ڲ���
								blendedImg(x, y, 0, 0) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x, y - (abs(MaxUp - lastcalureHeight)), 0, 2);
							}
							else {    //��ɫ����
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}
					else
					{
						CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameB, _filenameA, leftmatchVec, -matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
						//��ͼƬ��Ե�һ��ͼ�����±ߣ��ص�����Ϊ�¶����ȥ��ƫ�ƿ�ʼ������ͼƬ�ص��������
						cimg_forXY(blendedImg, x, y)
						{
							if (x >= blendedImg._width - lastblendedImg._width
								&& x < blendedImg._width
								&& y < lastblendedImg._height)
							{
								//�������ѡ��ʹ�ò�ֵ���߲���ֵ����ֵ�н��䣬���ǿ��ܻ�ͺ�ɫ�����ص���
								//��ֵ�Ļ���������Ļ�����ı�
								//���ǲ���ֵ���е�ͻأ�����Ը��������x������ʼ���

								//������Ϊ�ص������ֵ,��ʱ������ԣ����ڽ��书�ܷ���ʱ����Զ����������ֵ
								//(srcImgB._width - leftmatchVec) Ϊͼ��b��δ�غϵ�����
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
								//��b�����ֵҲ���ӽ�ȥ���ⲿ�ֵ�ͼ����δ���в�ֵ����
								else if (x >= blendedImg._width - lastblendedImg._width
									&& x <= (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width)
									&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
									&& y < blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B���ڲ���
									blendedImg(x, y, 0, 0) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
								}
								else {    //A�������ƴ�ӺõĶ��ڲ���
									blendedImg(x, y, 0, 0) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x - (blendedImg._width - lastblendedImg._width), y, 0, 2);
								}
							}

							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&& x <= blendedImg._width - lastblendedImg._width
								&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
								&& y < blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B���ڲ���
								blendedImg(x, y, 0, 0) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x, y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
							}
							else {    //��ɫ����
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
			//��ͼ��ƴ�ӵ�ʱ����Ҫ��������ӿռ�
			//��ƴ��ͼ����ԭ���Ŀ����
			//�жϸ�ͼ���Ƿ�����߶ȵı仯
			if (lastcalureHeight == MaxDown || lastcalureHeight == MaxUp)
			{
				//��������ͼ����Ҫ���Ӹ߶ȵĲ������������3-9�Ŵ���
				if (matchVecdy <= 0) {
					//��ͼ����Ե�һ��ͼ�����±�,��lastcalureHeightһ����������Ϊ����
					//����bͼ�񣨴�ƴ��ͼ�񣩵��ص�����Ϊ�ܵ�ͼ��߶ȼ�ȥbͼ��ĸ߶ȣ��õ���ʼ�߶�
					CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameB, _filenameA, leftmatchVec, -matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
					cimg_forXY(blendedImg, x, y) {
						if (x < lastblendedImg._width && y < lastblendedImg._height) {
							//�������ѡ��ʹ�ò�ֵ���߲���ֵ����ֵ�н��䣬���ǿ��ܻ�ͺ�ɫ�����ص���
							//��ֵ�Ļ���������Ļ�����ı�
							//���ǲ���ֵ���е�ͻأ�����Ը��������x������ʼ���
							//����ֵ����Ϊif (x >= abs(MaxLeft) + lastcalureWeight + srcImgB - leftmatchVec�� && y >= blendedImg._height - srcImgB._height)
							//blendedImg(x, y, 0, 0) = (float)lastblendedImg(x, y, 0, 0)
							// * (float)(x - (abs(MaxLeft) + lastcalureWeight)) / (float)abs(matchVecdx)
							//	+ (float)srcImgB(x - (abs(MaxLeft) + lastcalureWeight), y - (blendedImg._height - srcImgB._height), 0, 0)
							//	* (float)(x - (abs(MaxLeft) + lastcalureWeight)) / (float)abs(matchVecdx);
							//������Ϊ�ص������ֵ,��ʱ������ԣ����ڽ��书�ܷ���ʱ����Զ����������ֵ
							//(srcImgB._width - leftmatchVec) Ϊͼ��b��δ�غϵ�����
							if (x >= abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec)
								&& x < abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y >= blendedImg._height - srcImgB._height) {    //���

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
							//��b�����ֵҲ���ӽ�ȥ���ⲿ�ֵ�ͼ����δ���в�ֵ����
							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&& x < (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width)
								&& y >= blendedImg._height - srcImgB._height
								&& y < blendedImg._height) {    //B���ڲ���
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 2);
							}
							else {    //A�������ƴ�ӺõĶ��ڲ���
								blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
								blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
								blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
							}

						}
						//y����Ӧ���������ͼ����±ߣ��ұ�
						else if (x >= (abs(MaxLeft) + (lastcalureWeight))
							&& x < (abs(MaxLeft) + (lastcalureWeight)) + srcImgB._width
							&& y >= blendedImg._height - srcImgB._height && y < blendedImg._height) {    //B���ڲ���
							blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 2);
						}
						else {    //��ɫ����
							blendedImg(x, y, 0, 0) = 0;
							blendedImg(x, y, 0, 1) = 0;
							blendedImg(x, y, 0, 2) = 0;
						}
					}
				}
				//3-11���޸�
				else {
					//matchVec.dy > 0; �Ҳ�ͼƬ��Ҫ�������ƶ�
					//��ͼ����Ե�һ��ͼ�����ϱ�,��lastcalureHeightһ����������Ϊ����
					CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameB, _filenameA, leftmatchVec, -matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
					cimg_forXY(blendedImg, x, y) {
						if (x < lastblendedImg._width && y >= blendedImg._height - lastblendedImg._height) {
							//�������ѡ��ʹ�ò�ֵ���߲���ֵ����ֵ�н��䣬���ǿ��ܻ�ͺ�ɫ�����ص���
							//��ֵ�Ļ���������Ļ�����ı�
							//���ǲ���ֵ���е�ͻأ�����Ը��������x������ʼ���
							//����ֵ����Ϊ
							if (x >= abs(MaxLeft) + (lastcalureWeight)+(srcImgB._width - leftmatchVec)
								&& x < abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y < srcImgB._height) {    //���


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
							else {    //A���ڲ���
								blendedImg(x, y, 0, 0) = lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 0);
								blendedImg(x, y, 0, 1) = lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 1);
								blendedImg(x, y, 0, 2) = lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 2);
							}
						}
						else if (x >= (abs(MaxLeft) + (lastcalureWeight))
							&& x <= (abs(MaxLeft) + (lastcalureWeight)) + srcImgB._width
							&& y < srcImgB._height) {    //B���ڲ���
							blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 2);
						}
						else {    //��ɫ����
							blendedImg(x, y, 0, 0) = 0;
							blendedImg(x, y, 0, 1) = 0;
							blendedImg(x, y, 0, 2) = 0;
						}
					}
				}
			}
			else
			{
				//��������ͼ����Ҫ���Ӹ߶ȵĲ������������3-11���ع�����

				if (matchVecdy <= 0)
				{

					//��ƴ�ӵ�ͼƬ�����ƥ���ͼ�����������ƶ�
					//������ͼ����Ҫ��չ�߶�,����Ҫ��չ���
					if (lastcalureHeight > 0)
					{
						//��ͼƬ��Ե�һ��ͼ�����ϱߣ��ص�����Ϊ�϶����ȥ��ƫ�ƿ�ʼ����ƴ�����ͼ�����
						CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameB, _filenameA, leftmatchVec, -matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
						cimg_forXY(blendedImg, x, y)
						{
							if (x < lastblendedImg._width && y < lastblendedImg._height)
							{
								//�������ѡ��ʹ�ò�ֵ���߲���ֵ����ֵ�н��䣬���ǿ��ܻ�ͺ�ɫ�����ص���
								//��ֵ�Ļ���������Ļ�����ı�
								//���ǲ���ֵ���е�ͻأ�����Ը��������x������ʼ���

								//������Ϊ�ص������ֵ,��ʱ������ԣ����ڽ��书�ܷ���ʱ����Զ����������ֵ
								//(srcImgB._width - leftmatchVec) Ϊͼ��b��δ�غϵ�����
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
								//��b�����ֵҲ���ӽ�ȥ���ⲿ�ֵ�ͼ����δ���в�ֵ����
								else if (x >= (abs(MaxLeft) + (lastcalureWeight))
									&& x < (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width)
									&& y >= abs(MaxUp - lastcalureHeight)
									&& y < abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B���ڲ���
									blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 2);
								}
								else {    //A�������ƴ�ӺõĶ��ڲ���
									blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
								}
							}
							//y����Ӧ���������ͼ����±ߣ��ұ�,�������ﲻ�����У�������������ϱ�ѭ��
							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&& x <= (abs(MaxLeft) + (lastcalureWeight)) + srcImgB._width
								&& y >= abs(MaxUp - lastcalureHeight)
								&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B���ڲ���
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 2);
							}
							else {    //��ɫ����
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}
					else
					{
						//��ͼƬ��Ե�һ��ͼ�����±ߣ��ص�����Ϊ�¶����ȥ��ƫ�ƿ�ʼ������ͼƬ�ص��������
						CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameB, _filenameA, leftmatchVec, -matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
						cimg_forXY(blendedImg, x, y)
						{
							if (x < lastblendedImg._width && y < lastblendedImg._height)
							{
								//�������ѡ��ʹ�ò�ֵ���߲���ֵ����ֵ�н��䣬���ǿ��ܻ�ͺ�ɫ�����ص���
								//��ֵ�Ļ���������Ļ�����ı�
								//���ǲ���ֵ���е�ͻأ�����Ը��������x������ʼ���

								//������Ϊ�ص������ֵ,��ʱ������ԣ����ڽ��书�ܷ���ʱ����Զ����������ֵ
								//(srcImgB._width - leftmatchVec) Ϊͼ��b��δ�غϵ�����
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
								//��b�����ֵҲ���ӽ�ȥ���ⲿ�ֵ�ͼ����δ���в�ֵ����
								else if (x >= (abs(MaxLeft) + (lastcalureWeight))
									&& x < (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width)
									&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
									&& y < blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B���ڲ���
									blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
								}
								else {    //A�������ƴ�ӺõĶ��ڲ���
									blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
								}


							}
							//y����Ӧ���������ͼ����±ߣ��ұ�
							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&& x <= (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width)
								&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
								&& y < blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B���ڲ���
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
							}
							else {    //��ɫ����
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}


				}
				else
				{
					//��ƴ�ӵ�ͼƬ�����ƥ��ĵ�ͼ�����������ƶ�
					//������ͼ����Ҫ��չ�߶�,����Ҫ��չ���
					if (lastcalureHeight > 0)
					{
						//��ͼƬ��Ե�һ��ͼ�����ϱߣ��ص�����Ϊ�϶����ȥ��ƫ�ƿ�ʼ����ƴ�����ͼ�����
						CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameB, _filenameA, leftmatchVec, -matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
						cimg_forXY(blendedImg, x, y)
						{
							if (x < lastblendedImg._width && y < lastblendedImg._height)
							{
								//�������ѡ��ʹ�ò�ֵ���߲���ֵ����ֵ�н��䣬���ǿ��ܻ�ͺ�ɫ�����ص���
								//��ֵ�Ļ���������Ļ�����ı�
								//���ǲ���ֵ���е�ͻأ�����Ը��������x������ʼ���

								//������Ϊ�ص������ֵ,��ʱ������ԣ����ڽ��书�ܷ���ʱ����Զ����������ֵ
								//(srcImgB._width - leftmatchVec) Ϊͼ��b��δ�غϵ�����
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
								//��b�����ֵҲ���ӽ�ȥ���ⲿ�ֵ�ͼ����δ���в�ֵ����
								else if (x >= (abs(MaxLeft) + (lastcalureWeight))
									&& x <= (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width)
									&& y >= abs(MaxUp - lastcalureHeight)
									&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B���ڲ���
									blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 2);
								}
								else {    //A�������ƴ�ӺõĶ��ڲ���
									blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
								}
							}
							//y����Ӧ���������ͼ����±ߣ��ұ�,�������ﲻ�����У�������������ϱ�ѭ��
							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&& x <= (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width)
								&& y >= abs(MaxUp - lastcalureHeight)
								&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B���ڲ���
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (abs(MaxUp - lastcalureHeight)), 0, 2);
							}
							else {    //��ɫ����
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}
					else
					{
						//��ͼƬ��Ե�һ��ͼ�����±ߣ��ص�����Ϊ�¶����ȥ��ƫ�ƿ�ʼ������ͼƬ�ص��������
						CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameB, _filenameA, leftmatchVec, -matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
						cimg_forXY(blendedImg, x, y)
						{
							if (x < lastblendedImg._width && y < lastblendedImg._height)
							{
								//�������ѡ��ʹ�ò�ֵ���߲���ֵ����ֵ�н��䣬���ǿ��ܻ�ͺ�ɫ�����ص���
								//��ֵ�Ļ���������Ļ�����ı�
								//���ǲ���ֵ���е�ͻأ�����Ը��������x������ʼ���

								//������Ϊ�ص������ֵ,��ʱ������ԣ����ڽ��书�ܷ���ʱ����Զ����������ֵ
								//(srcImgB._width - leftmatchVec) Ϊͼ��b��δ�غϵ�����
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
								//��b�����ֵҲ���ӽ�ȥ���ⲿ�ֵ�ͼ����δ���в�ֵ����
								else if (x >= (abs(MaxLeft) + (lastcalureWeight))
									&& x <= (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width)
									&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
									&& y < blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B���ڲ���
									blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
								}
								else {    //A�������ƴ�ӺõĶ��ڲ���
									blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
								}


							}
							//y����Ӧ���������ͼ����±ߣ��ұ�
							//һ�㲻����ʹ��
							else if (x >= (abs(MaxLeft) + (lastcalureWeight))
								&& x <= (abs(MaxLeft) + (lastcalureWeight)+srcImgB._width)
								&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
								&& y < blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B���ڲ���
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
							}
							else {    //��ɫ����
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
		//��Ҫƴ�ӵ�ͼ�����ұ�
		//�жϵ�ǰͼ���������һ��ͼ���������ƶ����������ƶ�
		//С��0�������ƶ�
		//����0�������ƶ�
		//��ʽ��dy1 = img2.y - img1.y
		//dy2 = img3.y - img2.y;
		//img3�����img1:
		//dy = dy1+dy2 = img3.y - img1.y;
		//�����ȼ���ƫ����������������һ��ͼ�������ϻ�������

		lastcalureHeight += matchVecdy;
		if (lastcalureHeight <= 0)
		{
			//�����Ƚ�˭��С
			//�ж���������µ�ƫ���������С�����������������У�������Ҫ����������
			//������ڵĻ����͸����µ�������ͬʱ�������µ�ֵ
			MaxDown = MaxDown < lastcalureHeight ? MaxDown : lastcalureHeight;

		}
		else//�����ƶ� 
		{
			//�����Ƚ�˭���
			//�ж���������ϵ�ƫ���������С�����������������У�������Ҫ����������
				//������ڵĻ����͸����ϵ�������ͬʱ�������ϵ�ֵ
			MaxUp = MaxUp > lastcalureHeight ? MaxUp : lastcalureHeight;
		}

		//�������ǰ��ͼ������ڵ�һ��ͼ�����ƫ���˶���
		lastcalureWeight = lastcalureWeight + srcImgB._width - matchVecdx;
		if (lastcalureWeight > 0)
			MaxRight = MaxRight > lastcalureWeight ? MaxRight : lastcalureWeight;
		else
			MaxLeft = MaxLeft < lastcalureWeight ? MaxLeft : lastcalureWeight;

		//��ƴ�ӵ�ͼƬ��ͼ����ұ�
		//-----------------------�ϲ��Ĵ��ڴ�Сλ��-----------------------------------------
		//dxΪ����ͼ���������֮��ĺ�Ĳ�ֵ
		//dyΪ����ͼ���������֮������Ĳ�ֵ
		//�ж��Ƿ���Ҫ���ӻ����ĳ���
		//�������ԭͼ�񣬼���������ֵͬʱ����������ֵ
		//������һ��ͼ���ж��Ƿ������еĸ߶�
		//���ӳ�ʼ��ͼ��Ĵ�С
		//�����ԭ��Ϊ���п�ȵ���ȡ��ԭ���Ĵ���ͼ��Ŀ�ȣ�
		//������ƴ����ߵ�ͼ��ʱ�������Ҫ������ͼ������
		//blendedImg = CImg<int>(lastblendedImg._width + srcImgB._width - matchVecdx,
		//	srcImgB._height + abs(MaxDown) + abs(MaxUp), 1, 3, 0);

		//
		blendedImg = CImg<int>(oriImageWidth + abs(MaxLeft) + abs(MaxRight),
			srcImgB._height + abs(MaxDown) + abs(MaxUp), 1, 3, 0);


		//�ж��¼����ͼ�����������һ��ͼ�����ƶ��������ƶ�
		//�ж�x�������ƴ�Ӻû����ǵ�һ��ͼ�������
		//������ͼ��ĵ�ǡ��������ͼ����ص�֮������Ҫ����ֵ����
		//�����ص�����Ĳ�ֵ��ʱ�򣬽��غ�����ı���ͬʱ�����ص�����Ŀ��
		//�˴���Ҫ��������

		if (lastcalureWeight == MaxRight)
		{
			//��ͼ��ƴ�ӵ�ʱ����Ҫ���ұ����ӿռ�
			//���Ҵ�ƴ��ͼ�����ұ�
			//�жϸ�ͼ���Ƿ�����߶ȵı仯
			if (lastcalureHeight == MaxDown || lastcalureHeight == MaxUp)
			{
				//��������ͼ����Ҫ���Ӹ߶ȵĲ������������3-9�Ŵ���
				if (matchVecdy <= 0) {
					//��ͼ����Ե�һ��ͼ�����±�,��lastcalureHeightһ����������Ϊ����
					//����bͼ�񣨴�ƴ��ͼ�񣩵��ص�����Ϊ�ܵ�ͼ��߶ȼ�ȥbͼ��ĸ߶ȣ��õ���ʼ�߶�
					CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameA, _filenameB, matchVecdx, matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
					cimg_forXY(blendedImg, x, y) {
						if (x < lastblendedImg._width && y < lastblendedImg._height) {
							if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x < abs(MaxLeft) + (lastcalureWeight)+matchVecdx
								&& y >= blendedImg._height - srcImgB._height
								&& y < blendedImg._height - srcImgB._height + (srcImgB._height - abs(matchVecdy))
								) {    //���

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
							//y����Ӧ���������ͼ����±ߣ��ұ�
							else if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y >= blendedImg._height - srcImgB._height && y < blendedImg._height) {    //B���ڲ���
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 2);
							}
							else {    //A�������ƴ�ӺõĶ��ڲ���
								blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
								blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
								blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
							}

						}
						//y����Ӧ���������ͼ����±ߣ��ұ�
						else if (x >= abs(MaxLeft) + (lastcalureWeight)
							&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
							&& y >= blendedImg._height - srcImgB._height && y < blendedImg._height) {    //B���ڲ���
							blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 2);
						}
						else {    //��ɫ����
							blendedImg(x, y, 0, 0) = 0;
							blendedImg(x, y, 0, 1) = 0;
							blendedImg(x, y, 0, 2) = 0;
						}
					}
				}
				//3-11���޸�
				else {
					//matchVec.dy > 0; �Ҳ�ͼƬ��Ҫ�������ƶ�
					//��ͼ����Ե�һ��ͼ�����ϱ�,��lastcalureHeightһ����������Ϊ����
					CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameA, _filenameB, matchVecdx, matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
					cimg_forXY(blendedImg, x, y) {
						if (x < lastblendedImg._width
							&& y >= blendedImg._height - lastblendedImg._height) {
							if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x < abs(MaxLeft) + (lastcalureWeight)+matchVecdx
								&& y > srcImgB._height - abs(matchVecdy)
								&& y < srcImgB._height) {    //���

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
								&& y < srcImgB._height) {    //B���ڲ���
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 2);
							}
							else {    //A���ڲ���
								blendedImg(x, y, 0, 0) = lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 0);
								blendedImg(x, y, 0, 1) = lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 1);
								blendedImg(x, y, 0, 2) = lastblendedImg(x, y - (blendedImg._height - lastblendedImg._height), 0, 2);
							}
						}
						else if (x >= abs(MaxLeft) + (lastcalureWeight)
							&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
							&& y < srcImgB._height) {    //B���ڲ���
							blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 2);
						}
						else {    //��ɫ����
							blendedImg(x, y, 0, 0) = 0;
							blendedImg(x, y, 0, 1) = 0;
							blendedImg(x, y, 0, 2) = 0;
						}
					}
				}
			}
			else
			{
				//���ڲ���Ҫ���Ӹ߶ȵĲ��֣�����3-11�Ŵ������д���
				if (matchVecdy <= 0)
				{
					//��ƴ�ӵ�ͼƬ�����ƥ���ͼ�����������ƶ�
					//������ͼ����Ҫ��չ�߶�
					if (lastcalureHeight > 0)
					{
						//��ͼƬ��Ե�һ��ͼ�����ϱߣ��ص�����Ϊ�϶����ȥ��ƫ�ƿ�ʼ����ƴ�����ͼ�����
						CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameA, _filenameB, matchVecdx, matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
						cimg_forXY(blendedImg, x, y) {
							if (x < lastblendedImg._width && y < lastblendedImg._height)
							{
								//Ҳ�ɸ���Ϊ�����ӣ�����ʣ����±߽�����ֵ�����ǿ��ܻ���ʧ��Ч������
								//if (x >= (lastblendedImg._width - matchVecdx) && y >= abs(MaxUp - lastcalureHeight)
								//	&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height - abs(matchVecdy))
								if (x >= abs(MaxLeft) + (lastcalureWeight)
									&& x < abs(MaxLeft) + (lastcalureWeight)+matchVecdx
									&& y >= abs(MaxUp - lastcalureHeight)
									&& y <abs(MaxUp - lastcalureHeight) + srcImgB._height - abs(matchVecdy)) {    //���
									
									
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
									&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B���ڲ���
									blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 2);
								}
								else {    //A�������ƴ�ӺõĶ��ڲ���
									blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
								}
							}
							else if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y >= abs(MaxUp - lastcalureHeight)
								&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B���ڲ���
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 2);
							}
							else {    //��ɫ����
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}

					else
					{
						//��ͼƬ��Ե�һ��ͼ�����±ߣ��ص�����Ϊ�¶��߼�ȥ��ƫ�ƣ��ó��ڶ���ͼ��Ľ�β���ؾ�������ͼ��ľ���
						//lastblendedImg._height - abs(MaxDown - lastcalureHeight)Ϊ�ڶ���ͼ�����ƴ��ʱ��ĸ߶�
						CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameA, _filenameB, matchVecdx, matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
						cimg_forXY(blendedImg, x, y) {
							if (x < lastblendedImg._width && y < lastblendedImg._height)
							{
								//Ҳ�ɸ���Ϊ�����ӣ�����ʣ����±߽�����ֵ�����ǿ��ܻ���ʧ��Ч������
								//if (x >= (lastblendedImg._width - matchVecdx) && y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight)
								//-srcImgB._height && y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - matchVecdy)
								if (x >= abs(MaxLeft) + (lastcalureWeight)
									&& x < abs(MaxLeft) + (lastcalureWeight)+matchVecdx
									&& y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
									&& y < lastblendedImg._height - abs(MaxDown - lastcalureHeight) - abs(matchVecdy))
								{    //���
									
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
								{    //B���ڲ���
									blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
								}
								else {    //A�������ƴ�ӺõĶ��ڲ���
									blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
								}
							}
							else if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
								&& y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B���ڲ���
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
							}
							else {    //��ɫ����
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}

				}
				else
				{
					//��ƴ�ӵ�ͼƬ�����ƥ���ͼ�����������ƶ�
					//������ͼ����Ҫ��չ�߶�

					if (lastcalureHeight > 0)
					{
						//��ͼƬ��Ե�һ��ͼ�����ϱߣ��ص�����Ϊ�϶����ȥ��ƫ�ƿ�ʼ����ƴ�����ͼ�����
						CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameA, _filenameB, matchVecdx, matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
						cimg_forXY(blendedImg, x, y) {
							if (x < lastblendedImg._width && y < lastblendedImg._height)
							{
								//Ҳ�ɸ���Ϊ�����ӣ�����ʣ����±߽�����ֵ�����ǿ��ܻ���ʧ��Ч������
								//if (x >= (lastblendedImg._width - matchVecdx) && y >= abs(MaxUp - lastcalureHeight)
								//	&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height - abs(matchVecdy))
								if (x >= abs(MaxLeft) + (lastcalureWeight)
									&& x < abs(MaxLeft) + (lastcalureWeight)+matchVecdx
									&& y >= abs(MaxUp - lastcalureHeight) + abs(matchVecdy)
									&& y < abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //���
									
									
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
									&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B���ڲ���
									blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 2);
								}
								else {    //A�������ƴ�ӺõĶ��ڲ���
									blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
								}
							}
							else if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y >= abs(MaxUp - lastcalureHeight)
								&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height) {    //B���ڲ���
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 2);
							}
							else {    //��ɫ����
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}

					else
					{
						//��ͼƬ��Ե�һ��ͼ�����±ߣ��ص�����Ϊ�¶��߼�ȥ��ƫ�ƣ��ó��ڶ���ͼ��Ľ�β���ؾ�������ͼ��ľ���
						//lastblendedImg._height - abs(MaxDown - lastcalureHeight)Ϊ�ڶ���ͼ�����ƴ��ʱ��ĸ߶�
						CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameA, _filenameB, matchVecdx, matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
						cimg_forXY(blendedImg, x, y) {
							if (x < lastblendedImg._width && y < lastblendedImg._height)
							{
								//Ҳ�ɸ���Ϊ�����ӣ�����ʣ����±߽�����ֵ�����ǿ��ܻ���ʧ��Ч������
								//if (x >= (lastblendedImg._width - matchVecdx) && y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight)
								//-srcImgB._height && y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - matchVecdy)
								if (x >= abs(MaxLeft) + (lastcalureWeight)
									&& x < abs(MaxLeft) + (lastcalureWeight)+matchVecdx
									&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - (srcImgB._height - matchVecdy)
									&& y < blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //���
									
									
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
									&& y <= blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B���ڲ���
									blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
									blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
									blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
								}
								else {    //A�������ƴ�ӺõĶ��ڲ���
									blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
								}
							}
							else if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y >= blendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
								&& y <= blendedImg._height - abs(MaxDown - lastcalureHeight)) {    //B���ڲ���
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
							}
							else {    //��ɫ����
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
			//��ͼ��ƴ�ӵ�ʱ����Ҫ���ұ������ӿռ�
			//��ƴ��ͼ����ԭ���Ŀ����
			//�жϸ�ͼ���Ƿ�����߶ȵı仯
			if (lastcalureHeight == MaxDown || lastcalureHeight == MaxUp)
			{
				//��������ͼ����Ҫ���Ӹ߶ȵĲ������������3-14�Ŵ���
				if (matchVecdy <= 0) {
					//��ͼ����Ե�һ��ͼ�����±�,��lastcalureHeightһ����������Ϊ����
					//����bͼ�񣨴�ƴ��ͼ�񣩵��ص�����Ϊ�ܵ�ͼ��߶ȼ�ȥbͼ��ĸ߶ȣ��õ���ʼ�߶�
					CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameA, _filenameB, matchVecdx, matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
					cimg_forXY(blendedImg, x, y) {
						if (x < lastblendedImg._width && y < lastblendedImg._height) {
							if (x >= abs(MaxLeft) + (lastcalureWeight)
								&& x < abs(MaxLeft) + (lastcalureWeight)+matchVecdx
								&& y >= blendedImg._height - srcImgB._height
								&& y < blendedImg._height - srcImgB._height + abs(matchVecdy)
								) {    //���
								
								
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
								//Bͼ����Aͼ���еĲ��ֲ���Ҫ�����ֵ�Ĳ���
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 2);
							}
							else {    //A�������ƴ�ӺõĶ��ڲ���
								blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
								blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
								blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
							}

						}
						//y����Ӧ���������ͼ����±ߣ��ұ�
						else if (x >= abs(MaxLeft) + (lastcalureWeight)
							&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
							&& y >= blendedImg._height - srcImgB._height
							&& y < blendedImg._height) {    //B���ڲ���
							blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (blendedImg._height - srcImgB._height), 0, 2);
						}
						else {    //��ɫ����
							blendedImg(x, y, 0, 0) = 0;
							blendedImg(x, y, 0, 1) = 0;
							blendedImg(x, y, 0, 2) = 0;
						}
					}
				}
				//3-11���޸�
				else {
					//matchVec.dy > 0; �Ҳ�ͼƬ��Ҫ�������ƶ�
					//��ͼ����Ե�һ��ͼ�����ϱ�,��lastcalureHeightһ����������Ϊ����
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
							{    //���
								
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
								//Bͼ����Aͼ���еĲ��ֲ���Ҫ�����ֵ�Ĳ���
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 2);
							}

							else {    //A���ڲ���
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
							) {    //B���ڲ���
							blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 0);
							blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 1);
							blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y, 0, 2);
						}
						else {    //��ɫ����
							blendedImg(x, y, 0, 0) = 0;
							blendedImg(x, y, 0, 1) = 0;
							blendedImg(x, y, 0, 2) = 0;
						}
					}
				}
			}
			else
			{
				//���ڲ���Ҫ���Ӹ߶ȵĲ��֣�����3-14�Ŵ������д���
				if (matchVecdy <= 0)
				{
					//��ƴ�ӵ�ͼƬ�����ƥ���ͼ�����������ƶ�
					//������ͼ����Ҫ��չ�߶�
					if (lastcalureHeight > 0)
					{
						//��ͼƬ��Ե�һ��ͼ�����ϱߣ��ص�����Ϊ�϶����ȥ��ƫ�ƿ�ʼ����ƴ�����ͼ�����
						CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameA, _filenameB, matchVecdx, matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
						cimg_forXY(blendedImg, x, y) {
							if (x < lastblendedImg._width && y < lastblendedImg._height)
							{
								//Ҳ�ɸ���Ϊ�����ӣ�����ʣ����±߽�����ֵ�����ǿ��ܻ���ʧ��Ч������
								//if (x >= (lastblendedImg._width - matchVecdx) && y >= abs(MaxUp - lastcalureHeight)
								//	&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height - abs(matchVecdy))
								if (x >= abs(MaxLeft) + (lastcalureWeight)
									&& x < abs(MaxLeft) + (lastcalureWeight)+matchVecdx
									&& y >= abs(MaxUp - lastcalureHeight)
									&& y < abs(MaxUp - lastcalureHeight) + srcImgB._height - abs(matchVecdy)) {    //���
									
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
								else {    //A�������ƴ�ӺõĶ��ڲ���
									blendedImg(x, y, 0, 0) = lastblendedImg(x, y, 0, 0);
									blendedImg(x, y, 0, 1) = lastblendedImg(x, y, 0, 1);
									blendedImg(x, y, 0, 2) = lastblendedImg(x, y, 0, 2);
								}
							}
							else if ((x >= abs(MaxLeft) + (lastcalureWeight)
								&& x <= abs(MaxLeft) + (lastcalureWeight)+srcImgB._width
								&& y >= abs(MaxUp - lastcalureHeight)
								&& y <= abs(MaxUp - lastcalureHeight) + srcImgB._height)) {    //B���ڲ���
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 2);
							}
							else {    //��ɫ����
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}

					else
					{
						//��ͼƬ��Ե�һ��ͼ�����±ߣ��ص�����Ϊ�¶��߼�ȥ��ƫ�ƣ��ó��ڶ���ͼ��Ľ�β���ؾ�������ͼ��ľ���
						//lastblendedImg._height - abs(MaxDown - lastcalureHeight)Ϊ�ڶ���ͼ�����ƴ��ʱ��ĸ߶�
						CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameA, _filenameB, matchVecdx, matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
						cimg_forXY(blendedImg, x, y) {
							if (x < lastblendedImg._width && y < lastblendedImg._height)
							{
								//Ҳ�ɸ���Ϊ�����ӣ�����ʣ����±߽�����ֵ�����ǿ��ܻ���ʧ��Ч������
								//if (x >= (lastblendedImg._width - matchVecdx) && y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight)
								//-srcImgB._height && y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - matchVecdy)
								if (x >= abs(MaxLeft) + (lastcalureWeight)
									&& x < abs(MaxLeft) + (lastcalureWeight)+matchVecdx
									&& y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height
									&& y < lastblendedImg._height - abs(MaxDown - lastcalureHeight) - abs(matchVecdy))
								{    //���
									
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
								else {    //A�������ƴ�ӺõĶ��ڲ���
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
							{	//b�����ֵ
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
							}
							else {    //��ɫ����
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}

				}
				else
				{
					//��ƴ�ӵ�ͼƬ�����ƥ���ͼ�����������ƶ�
					//������ͼ����Ҫ��չ�߶�

					if (lastcalureHeight > 0)
					{
						//��ͼƬ��Ե�һ��ͼ�����ϱߣ��ص�����Ϊ�϶����ȥ��ƫ�ƿ�ʼ����ƴ�����ͼ�����
						CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameA, _filenameB, matchVecdx, matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
						cimg_forXY(blendedImg, x, y) {
							if (x < lastblendedImg._width && y < lastblendedImg._height)
							{

								if (x >= abs(MaxLeft) + (lastcalureWeight)
									&& x < abs(MaxLeft) + (lastcalureWeight)+matchVecdx
									&& y >= abs(MaxUp - lastcalureHeight) + abs(matchVecdy)
									&& y < abs(MaxUp - lastcalureHeight) + srcImgB._height
									) {    //���

									
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
								else {    //A�������ƴ�ӺõĶ��ڲ���
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
								//B���ڵĲ���
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - abs(MaxUp - lastcalureHeight), 0, 2);
							}
							else {    //��ɫ����
								blendedImg(x, y, 0, 0) = 0;
								blendedImg(x, y, 0, 1) = 0;
								blendedImg(x, y, 0, 2) = 0;
							}
						}
					}

					else
					{
						//��ͼƬ��Ե�һ��ͼ�����±ߣ��ص�����Ϊ�¶��߼�ȥ��ƫ�ƣ��ó��ڶ���ͼ��Ľ�β���ؾ�������ͼ��ľ���
						//lastblendedImg._height - abs(MaxDown - lastcalureHeight)Ϊ�ڶ���ͼ�����ƴ��ʱ��ĸ߶�
						CImg<int> rectImage = myImageFusio.bestSeamLineGetLinePoint1(_filenameA, _filenameB, matchVecdx, matchVecdy, isLaplacianBlending, isDrawbestSeamLine);
						cimg_forXY(blendedImg, x, y) {
							if (x < lastblendedImg._width && y < lastblendedImg._height)
							{
								//Ҳ�ɸ���Ϊ�����ӣ�����ʣ����±߽�����ֵ�����ǿ��ܻ���ʧ��Ч������
								//if (x >= (lastblendedImg._width - matchVecdx) && y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight)
								//-srcImgB._height && y <= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - matchVecdy)
								if (x >= abs(MaxLeft) + (lastcalureWeight)
									&& x < abs(MaxLeft) + (lastcalureWeight)+matchVecdx
									&& y >= lastblendedImg._height - abs(MaxDown - lastcalureHeight) - abs(matchVecdy)
									&& y < lastblendedImg._height - abs(MaxDown - lastcalureHeight))
								{    //���
									
									
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
								else {    //A�������ƴ�ӺõĶ��ڲ���
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
							{	//B������ֵ���ӽ���
								blendedImg(x, y, 0, 0) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 0);
								blendedImg(x, y, 0, 1) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 1);
								blendedImg(x, y, 0, 2) = srcImgB(x - (abs(MaxLeft) + (lastcalureWeight)), y - (lastblendedImg._height - abs(MaxDown - lastcalureHeight) - srcImgB._height), 0, 2);
							}
							else {    //��ɫ����
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
	//-------��һ���ִ�����Խ�����д�޸�ƴ�ӹ���
	//�����޸�,���ݴ����ͼƬ�����Զ�ѡ��ͼƬ�ļ���

	CImg<int> srcImgA, srcImgB;
	//�޸�Ϊ����ͼƬ
	//srcImgA.load_bmp(_filenameA);
	//srcImgB.load_bmp(_filenameB);
	srcImgA.load(_filenameA);
	srcImgB.load(_filenameB);

	//�жϸ�ͼ���ƴ��ͼ���Ƿ����ұߣ��ǵĻ����ϼ�¼�����ҽ���ͼ����չ�ĵ���
	if (matchVec.dx > srcImgB._width) 
	{
		//ϴ�ļ�¼ֵ���еǼ��滻
		//����ʵ�� = ��ƴ��ͼ�� - dx/2��*2��
		//���Է���ֽ2
		int leftmatchVec = (srcImgB._width - matchVec.dx / 2) * 2;
	}
	else
	{



		//-----------------------�ϲ��Ĵ��ڴ�Сλ��-----------------------------------------
		//dxΪ����ͼ���������֮��ĺ�Ĳ�ֵ
		//dyΪ����ͼ���������֮������Ĳ�ֵ
		blendedImg = CImg<int>(srcImgA._width + srcImgB._width - matchVec.dx,
			srcImgA._height + abs(matchVec.dy), 1, 3, 0);
	}
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

	//��¼ƴ�ӵ���������
	if (StitchPictureNum == 0)
		StitchPictureNum = 2;
	else
		StitchPictureNum++;

}
*/
//----------2019-4-17����-----------
//1.�Ƚ������������أ��ж���ѡ����ֱ�ӽ��뽥���ںϻ��ǽ�����ѷ���߶�ֱ����ں�
//Ĭ�����ã����뽥���ں�

//2.�޸Ķ�ֱ����ںϲ��ִ��룬��ߴ���ɶ���
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

//�Ż�CImg�ĸ�ʽ��ƴ�Ӻ�ͼ��
CImg<int> MyMorePictureStitch::getBlendedImg() {
	return blendedImg;
}