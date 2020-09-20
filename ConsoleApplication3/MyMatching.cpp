#include "MyMatching.h"

MyMatching::MyMatching() {
}

MyMatching::~MyMatching() {
}

MyMatching::MyMatching(int _kp_count_A, Keypoint _firstKeyDesc_A, int _kp_count_B, Keypoint _firstKeyDesc_B) {
	keypoint_count_A = _kp_count_A;
	keypoint_count_B = _kp_count_B;
	firstKeyDescriptor_A = _firstKeyDesc_A;
	firstKeyDescriptor_B = _firstKeyDesc_B;
}

void MyMatching::featureMatchMainProcess() {
	Keypoint tempDescA = firstKeyDescriptor_A;
	//����Aͼ���������������B����������бȽ�ȡ��ֵ
	while (tempDescA) {
		float colA = tempDescA->col;
		float rowA = tempDescA->row;
		float* kp_desc_A = tempDescA->descrip;

		Keypoint tempDescB = firstKeyDescriptor_B;

		float minSSD = 100.0;
		int minIndex = -1;
		int colB = -1;
		int rowB = -1;
		while (tempDescB) {    //��Aͼÿ���㣬��Bͼ�����㣬�������
			float ssd = 0;
			for (int i = 0; i < LEN; i++) {
				float descA = *(kp_desc_A + i);
				float descB = *(tempDescB->descrip + i);
				ssd += abs(descA - descB);
			}
			if (ssd < minSSD) {
				minSSD = ssd;
				colB = tempDescB->col;
				rowB = tempDescB->row;
			}
			tempDescB = tempDescB->next;
		}

		if (minSSD < FeatureDescGap) {    //������С����ֵ��������һ��ƥ���
			Point pa(tempDescA->col, tempDescA->row);
			Point pb(colB, rowB);

			MatchedPair mpair(pa, pb, minSSD);
			matchedPairSet.push_back(mpair);
		}
		tempDescA = tempDescA->next;
	}

	//���������ͼ����������������
	//δ����ɸѡ��ĵ�matchedPairSet[]
	for (int i = 0; i < matchedPairSet.size(); i++) {
		cout << "A col: " << matchedPairSet[i].keyPointA.col << ", row: " << matchedPairSet[i].keyPointA.row << endl;
		cout << " with B col: " << matchedPairSet[i].keyPointB.col << ", row: " << matchedPairSet[i].keyPointB.row << " , minSSD: " << matchedPairSet[i].minDis << endl;
	}
	cout << ">>> matchedPairSet.size: " << matchedPairSet.size() << endl;
}

//�滭�������Ƶ�
//��ȡԭ����ͼ��
//��ƥ�����������ϼ��ϻƵ�

void MyMatching::drawOriKeypointOnImg(char* _filenameA, char* _filenameB, char* _saveAddrA, char* _saveAddrB) {
	
	//�鿴�Ƿ��иı�ͼƬ��ȡ��ʽ 2019-3-6
	srcImgA.load(_filenameA);
	//srcImgA.load_bmp(_filenameA);
	srcImgWithKpA = CImg<int>(srcImgA._width, srcImgA._height, 1, 3, 0);
	cimg_forXY(srcImgWithKpA, x, y) {
		srcImgWithKpA(x, y, 0, 0) = srcImgA(x, y, 0, 0);
		srcImgWithKpA(x, y, 0, 1) = srcImgA(x, y, 0, 1);
		srcImgWithKpA(x, y, 0, 2) = srcImgA(x, y, 0, 2);
	}

	srcImgB.load(_filenameB);
	srcImgWithKpB = CImg<int>(srcImgB._width, srcImgB._height, 1, 3, 0);
	cimg_forXY(srcImgWithKpB, x, y) {
		srcImgWithKpB(x, y, 0, 0) = srcImgB(x, y, 0, 0);
		srcImgWithKpB(x, y, 0, 1) = srcImgB(x, y, 0, 1);
		srcImgWithKpB(x, y, 0, 2) = srcImgB(x, y, 0, 2);
	}

	const double yellow[] = { 255, 255, 0 };
	for (int i = 0; i < matchedPairSet.size(); i++) {
		cout << "A col: " << matchedPairSet[i].keyPointA.col << ", row: " << matchedPairSet[i].keyPointA.row << endl;
		cout << " with B col: " << matchedPairSet[i].keyPointB.col << ", row: " << matchedPairSet[i].keyPointB.row << " , minSSD: " << matchedPairSet[i].minDis << endl;
		//�˴�����ƥ�䵽��������Сƥ���ĵ���λ��
		srcImgWithKpA.draw_circle(matchedPairSet[i].keyPointA.col, matchedPairSet[i].keyPointA.row, 3, yellow, 1.0f);
		srcImgWithKpB.draw_circle(matchedPairSet[i].keyPointB.col, matchedPairSet[i].keyPointB.row, 3, yellow, 1.0f);
	}
	//չʾ����ͼ�񲢱��֣��Һ��ڿ��Բ�����������ӣ�����������ͼչʾ
	srcImgWithKpA.display("srcImgWithKpA");
	srcImgWithKpA.save(_saveAddrA);
	srcImgWithKpB.display("srcImgWithKpB");
	srcImgWithKpB.save(_saveAddrB);

}


//�滭����ɫ��������ƥ����
void MyMatching::mixImageAndDrawPairLine(char* mixImgAddr, char* mixImgWithLineAddr) {
	//�滭������ͼ��Ŀ�Ⱥ�ѡȡͼ��������Ϊͼ�����ߵ�
	mixImg = CImg<int>(srcImgA._width + srcImgB._width, MAX(srcImgA._height, srcImgB._height), 1, 3, 0);
	cimg_forXY(mixImg, x, y) {
		if (x < srcImgA._width) {
			if (y < srcImgA._height) {
				mixImg(x, y, 0, 0) = srcImgWithKpA(x, y, 0, 0);
				mixImg(x, y, 0, 1) = srcImgWithKpA(x, y, 0, 1);
				mixImg(x, y, 0, 2) = srcImgWithKpA(x, y, 0, 2);
			}
			else {
				mixImg(x, y, 0, 0) = 0;
				mixImg(x, y, 0, 1) = 0;
				mixImg(x, y, 0, 2) = 0;
			}
		}
		else {
			//���ұ�ƫ�Ʋ���ͼƬ����
			if (y < srcImgB._height) {
				mixImg(x, y, 0, 0) = srcImgWithKpB(x - srcImgA._width, y, 0, 0);
				mixImg(x, y, 0, 1) = srcImgWithKpB(x - srcImgA._width, y, 0, 1);
				mixImg(x, y, 0, 2) = srcImgWithKpB(x - srcImgA._width, y, 0, 2);
			}
			else {
				mixImg(x, y, 0, 0) = 0;
				mixImg(x, y, 0, 1) = 0;
				mixImg(x, y, 0, 2) = 0;
			}
		}
	}
	mixImg.display("mixImg");
	mixImg.save(mixImgAddr);

	const double blue[] = { 0, 255, 255 };
	for (int i = 0; i < matchedPairSet.size(); i++) {
		int xa = matchedPairSet[i].keyPointA.col;
		int ya = matchedPairSet[i].keyPointA.row;

		int xb = matchedPairSet[i].keyPointB.col + srcImgA._width;
		int yb = matchedPairSet[i].keyPointB.row;

		mixImg.draw_line(xa, ya, xb, yb, blue);
	}
	mixImg.display("mixImgWithLine");
	mixImg.save(mixImgWithLineAddr);
}

void MyMatching::myRANSACtoFindKpTransAndDrawOut(char* _filename) {
	int maxInliers = 0;
	int maxIndex = -1;
	int inliersCount;

	//��ÿһ��ƥ��㣬��ƥ������v
	//�����ǵ�ǰͼ������ƥ���
	for (int i = 0; i < matchedPairSet.size(); i++) {
		inliersCount = 0;
		int xa = matchedPairSet[i].keyPointA.col;
		int ya = matchedPairSet[i].keyPointA.row;

		int xb = matchedPairSet[i].keyPointB.col + srcImgA._width;
		int yb = matchedPairSet[i].keyPointB.row;

		int deltaX = xb - xa;
		int deltaY = yb - ya;

		//��ÿһ��v������������ƥ������������롣��С����ֵ������Ϊinlier
		for (int j = 0; j < matchedPairSet.size(); j++) {
			if (j != i) {
				int txa = matchedPairSet[j].keyPointA.col;
				int tya = matchedPairSet[j].keyPointA.row;

				int txb = matchedPairSet[j].keyPointB.col + srcImgA._width;
				int tyb = matchedPairSet[j].keyPointB.row;

				int tdeltaX = txb - txa;
				int tdeltaY = tyb - tya;
				//��ƽ������ʽ��ͬʱ���õ���vectorGap��¼����
				int vectorGap = (tdeltaX - deltaX) * (tdeltaX - deltaX) + (tdeltaY - deltaY) * (tdeltaY - deltaY);
				//cout << "i: " << i << ", j: " << j << "  vectorGap: " << vectorGap << endl;

				if (vectorGap < InliersGap) {
					inliersCount++;
				}
			}
		}
		//�������inliers��ƥ������v����ƥ����������Ϊ���йؼ����ƥ������
		if (inliersCount > maxInliers) {
			maxInliers = inliersCount;
			maxIndex = i;
		}
	}

	//�˴���maxIndexΪ��¼������������ϱ��
	//ͬʱ�ڴ˴�����õ����Ϣ
	cout << "maxIndex: " << maxIndex << ", maxInliers: " << maxInliers << endl;

	drawRealKeypointOnImg(_filename, maxIndex);
}


void MyMatching::drawRealKeypointOnImg(char* _filename, int maxIndex) {
	//���µĺϲ�ͼ�ϣ��������ڸ�ƥ���ϵ��ƥ���pair
	fixedMatchedImg = CImg<int>(srcImgA._width + srcImgB._width, srcImgA._height, 1, 3, 0);
	cimg_forXY(fixedMatchedImg, x, y) {
		if (x < srcImgA._width) {
			if (y < srcImgA._height) {
				fixedMatchedImg(x, y, 0, 0) = srcImgWithKpA(x, y, 0, 0);
				fixedMatchedImg(x, y, 0, 1) = srcImgWithKpA(x, y, 0, 1);
				fixedMatchedImg(x, y, 0, 2) = srcImgWithKpA(x, y, 0, 2);
			}
			else {
				fixedMatchedImg(x, y, 0, 0) = 0;
				fixedMatchedImg(x, y, 0, 1) = 0;
				fixedMatchedImg(x, y, 0, 2) = 0;
			}
		}
		else {
			if (y < srcImgB._height) {
				fixedMatchedImg(x, y, 0, 0) = srcImgWithKpB(x - srcImgA._width, y, 0, 0);
				fixedMatchedImg(x, y, 0, 1) = srcImgWithKpB(x - srcImgA._width, y, 0, 1);
				fixedMatchedImg(x, y, 0, 2) = srcImgWithKpB(x - srcImgA._width, y, 0, 2);
			}
			else {
				fixedMatchedImg(x, y, 0, 0) = 0;
				fixedMatchedImg(x, y, 0, 1) = 0;
				fixedMatchedImg(x, y, 0, 2) = 0;
			}
		}
	}
	//����������ͼ���ƥ��
	int mxa = matchedPairSet[maxIndex].keyPointA.col;
	int mya = matchedPairSet[maxIndex].keyPointA.row;

	int mxb = matchedPairSet[maxIndex].keyPointB.col + srcImgA._width;
	int myb = matchedPairSet[maxIndex].keyPointB.row;

	int mdeltaX = mxb - mxa;
	int mdeltaY = myb - mya;    //�õ���ʵƥ���ϵ��ƥ������v

	matchVec = Point(mdeltaX, mdeltaY);
	cout << "Real match vector: (" << mdeltaX << ", " << mdeltaY << ")" << endl;

	const double blue[] = { 0, 255, 255 };
	for (int j = 0; j < matchedPairSet.size(); j++) {    //��������ƥ��������v�ľ���d
		int txa = matchedPairSet[j].keyPointA.col;
		int tya = matchedPairSet[j].keyPointA.row;

		int txb = matchedPairSet[j].keyPointB.col + srcImgA._width;
		int tyb = matchedPairSet[j].keyPointB.row;

		int tdeltaX = txb - txa;
		int tdeltaY = tyb - tya;

		int vectorGap = (tdeltaX - mdeltaX) * (tdeltaX - mdeltaX) + (tdeltaY - mdeltaY) * (tdeltaY - mdeltaY);

		if (vectorGap < InliersGap) {    //����dС����ֵ������Ϊ��ȷ��ƥ���

			//��ƥ��ĵ��ڼ�¼��ͼ�е�λ�ã������޸�
			fixedMatchedImg.draw_line(txa, tya, txb, tyb, blue);
		}
	}

	fixedMatchedImg.display("mixImgWithLine_fixed");
	fixedMatchedImg.save(_filename);
}

//�Ż�ƥ��ĵ�������еĵ�
Point MyMatching::getMatchVec() {
	return matchVec;
}