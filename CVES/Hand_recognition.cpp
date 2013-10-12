//////////////////////////////////////////////////////////////////////////////////////////////
//	The OpenCVE Project.
//
//	The MIT License (MIT)
//	Copyright © 2013 {Doohoon Kim, Sungpil Moon, Kyuhong Choi} at AR Team of SW Maestro 4th
//	{invi.dh.kim, munsp9103, aiaipming} at gmail.com
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy of
//	this software and associated documentation files (the “Software”), to deal
//	in the Software without restriction, including without limitation the rights to
//	use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
//	the Software, and to permit persons to whom the Software is furnished to do so,
//	subject to the following conditions:
//
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
//	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
//	PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
//	LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
//	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
//	OR OTHER DEALINGS IN THE SOFTWARE.
//////////////////////////////////////////////////////////////////////////////////////////////

#include "Hand_recognition.hpp"

Hand_recognition::Hand_recognition(void) {
}

Hand_recognition::~Hand_recognition(void) {
	cvReleaseImage(&img_YCrCb);
	cvReleaseImage(&img_HSV);

	if (prev_ground != NULL)
		cvReleaseImage(&prev_ground);

	if (present_ground != NULL)
		cvReleaseImage(&present_ground);
}

void Hand_recognition::Init(int width, int height) {
	//관심영역의 size를 구함.
	img_width = width;
	img_height = height;

	//skin detection에 사용되는 내부 연산 이미지.
	//skin color를 통한 detecion에 문제가 있어 현재 사용하지 않고 있음.
	img_YCrCb = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
	img_HSV = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);

	prev_ground = NULL;
	present_ground = NULL;
}

bool Hand_recognition::R1(int R, int G, int B) {
	//RGB image의 각 픽셀값을 입력받아서 skin의 범위 해당하는지 return
	bool e1 = (R > 95) && (G > 40) && (B > 20) && ((max(R, max(G, B)) - min(R, min(G , B))) > 15) && (abs(R - G) > 15) && (R > G) && (R > B);
	bool e2 = (R > 220) && (G > 210) && (B > 170) && (abs(R - G) <= 15) && (R > B) && (G > B);
	return (e1||e2);
}

bool Hand_recognition::R2(float Y, float Cr, float Cb) {
	//Y, Cr, Cb image를 통하여 skin 영역의 색상을 구분.
	bool e3 = Cr <= (1.5862 * Cb) + 20;
	bool e4 = Cr >= (0.3448 * Cb) + 76.2069;
	bool e5 = Cr >= (-4.5652 * Cb) + 234.5652;
	bool e6 = Cr <= (-1.15 * Cb) + 301.75;
	bool e7 = Cr <= (-2.2857 * Cb) + 432.85;
	return e3 && e4 && e5 && e6 && e7;
}

bool Hand_recognition::R3(float H, float S, float V) {
	//HSV image의 H를 통하여 SKin color 범주에 있는지 확인.
	return (H < 25) || (H > 230);
}

void Hand_recognition::Detect_Skin(IplImage *src, IplImage *dst) {
	//skin color detection을 진행.
	//RGB-H-CbCr Skin Colour Model for Human Face Detection 논문 참조.
	//http://pesona.mmu.edu.my/~johnsee/research/papers/files/rgbhcbcr_m2usic06.pdf 
	cvCvtColor(src, img_YCrCb, CV_BGR2YCrCb);
	cvCvtColor(src, img_HSV, CV_BGR2HSV);

	cvZero(dst);

	for (register int i = 0; i < dst->height; i++) {
		for (register int j = 0; j < dst->width; j++) {
			//RGB 색상계를 이용하여 skin이 아닌 픽셀들을 걸러냄.
			B = (unsigned char)src->imageData[(j * 3) + (i * src->widthStep)];
			G = (unsigned char)src->imageData[(j * 3) + (i * src->widthStep) + 1];
			R = (unsigned char)src->imageData[(j * 3) + (i * src->widthStep) + 2];

			bool a = R1(R, G, B);

			if (a) {
				//HSV 색상계에 H를 이용하여 피부 영역 검출
				H = (unsigned char)img_HSV->imageData[(j * 3) + (i * img_HSV->widthStep)];
				S = (unsigned char)img_HSV->imageData[(j * 3) + (i * img_HSV->widthStep) + 1];
				V = (unsigned char)img_HSV->imageData[(j * 3) + (i * img_HSV->widthStep) + 2];

				bool c = R3(H, S, V);

				if (c) {
					//YCbCr 색상계를 사용하여 최종 피부색 판정.
					Y = (unsigned char)img_YCrCb->imageData[(j * 3) + (i * img_YCrCb->widthStep)];
					Cr = (unsigned char)img_YCrCb->imageData[(j * 3) + (i * img_YCrCb->widthStep) + 1];
					Cb = (unsigned char)img_YCrCb->imageData[(j * 3) + (i * img_YCrCb->widthStep) + 2];

					bool b = R2(Y, Cr, Cb);

					if (b) {
						dst->imageData[j + (i * dst->widthStep)] = (unsigned char) 255;
					}
				}
			}
		}
	}

	//noise에 의한 영향을 줄이기 위해 모폴로지 연산 적용.
	cvErode(dst, dst, 0, MOP_NUM);
	cvDilate(dst, dst, 0, MOP_NUM);
}

bool Hand_recognition::is_Hand(IplImage *src) {
	//binary image를 이용하여 피부색 영역 픽셀이 일정 갯수 이상이면 손이라고 판정.
	int count = 0;

	for (register int i = 0; i < src->width; i++) {
		for (register int j = 0; j < src->height; j++) {
			if ((unsigned char)src->imageData[i + (j * src->widthStep)] == 255)
				count++;
		}
	}

	if (count > T_SKIN_NUM)
		return true;

	return false;
}

void Hand_recognition::Sub_prevFrame(IplImage *src, IplImage *dst, bool first) {
	//손을 검출하기 위한 차영상 적용.
	//RGB image src를 입력받아 binary image dst를 반환.
	static int frame_count = 0;

	//first가 true일 경우 이미지 초기화.
	if (first) {
		if (prev_ground != NULL){
			cvReleaseImage(&prev_ground);
			prev_ground = NULL;
		}
	}

	//이전 프레임의 이미지가 존재하지 않을때, 이전 이미지에 현재 입력받은 이미지를 입력.
	if (prev_ground == NULL) {
		prev_ground = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);
		present_ground = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);
		/*cvCvtColor(src, prev_ground, CV_BGR2GRAY);*/
		cvCopy(src, prev_ground);
	}

	cvZero(dst);

	//프로그램의 성능을 위해 몇 프레임마다 차영상을 적용할 것인가를 if문을 통하여 조절할 수 있음.
	if (frame_count == 1) {
		//cvCvtColor(src, present_ground, CV_BGR2GRAY);
		cvCopy(src, present_ground);

		//image 내부의 모든 픽셀을 대상으로 rgb 차영상 적용.
		for (int i = 0; i < src->width; i++) {
			for (int j = 0; j < src->height; j++) {
				unsigned char SUB_B = abs((unsigned char)src->imageData[(i * 3) + (j * src->widthStep)] - (unsigned char)prev_ground->imageData[(i * 3) + (j * prev_ground->widthStep)]);
				unsigned char SUB_G = abs((unsigned char)src->imageData[(i * 3) + (j * src->widthStep) + 1] - (unsigned char)prev_ground->imageData[(i * 3) + (j * prev_ground->widthStep) + 1]);
				unsigned char SUB_R = abs((unsigned char)src->imageData[(i * 3) + (j * src->widthStep) + 2] - (unsigned char)prev_ground->imageData[(i * 3) + (j * prev_ground->widthStep) + 2]);

				if(SUB_B > SUB_THRESHOLD || SUB_G > SUB_THRESHOLD || SUB_R > SUB_THRESHOLD) {
					dst->imageData[i + (j * dst->widthStep)] = (unsigned char)255;
				}
			}
		}
		//cvCopy(present_ground, prev_ground);

		//차영상 연산의 결과로 생성된 binary image에 필터 적용.
		cvSmooth(dst, dst, CV_MEDIAN,3,3);
		cvErode(dst, dst, 0, 2);
		cvDilate(dst, dst, 0, 2);

		frame_count = 0;
	}

	frame_count++;
}

void Hand_recognition::Init_diff() {
	//내부 차영상 연산에 사용되는 이미지 할당 해제.
	cvReleaseImage(&prev_ground);
	cvReleaseImage(&present_ground);
}