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

	// if (present_ground != NULL)의 옆쪽 ';'은 무슨 의미? Release 안시킨다는 의미?
	// 수정 바람.
	if (present_ground != NULL);
	cvReleaseImage(&present_ground);
}

void Hand_recognition::Init(int width, int height) {
	img_width = width;
	img_height = height;

	img_YCrCb = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
	img_HSV = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);

	prev_ground = NULL;
	present_ground = NULL;
}

bool Hand_recognition::R1(int R, int G, int B) {
	// 연산 Error의 소지가 있음.
	// 수정 바람.
	bool e1 = (R > 95) && (G > 40) && (B > 20) && ((max(R, max(G, B)) - min(R, min(G , B))) > 15) && (abs(R - G) > 15) && (R > G) && (R > B);
	bool e2 = (R > 220) && (G > 210) && (B > 170) && (abs(R - G) <= 15) && (R > B) && (G > B);
	return (e1||e2);
}

bool Hand_recognition::R2(float Y, float Cr, float Cb) {
	// 연산 Error의 소지가 있음.
	// 수정 바람.
	bool e3 = Cr <= 1.5862 * Cb + 20;
	bool e4 = Cr >= 0.3448 * Cb + 76.2069;
	bool e5 = Cr >= -4.5652 * Cb + 234.5652;
	bool e6 = Cr <= -1.15 * Cb + 301.75;
	bool e7 = Cr <= -2.2857 * Cb + 432.85;
	return e3 && e4 && e5 && e6 && e7;
}

bool Hand_recognition::R3(float H, float S, float V) {
	return (H < 25) || (H > 230);
}

void Hand_recognition::Detect_Skin(IplImage *src, IplImage *dst) {
	cvCvtColor(src, img_YCrCb, CV_BGR2YCrCb);
	cvCvtColor(src, img_HSV, CV_BGR2HSV);

	cvZero(dst);

	for (register int i = 0; i < dst->height; i++) {
		for (register int j = 0; j < dst->width; j++) {
			// 연산 Error의 소지가 있음.
			// 수정 바람.
			B = (unsigned char)src->imageData[(j * 3) + i * src->widthStep];
			G = (unsigned char)src->imageData[(j * 3) + i * src->widthStep + 1];
			R = (unsigned char)src->imageData[(j * 3) + i * src->widthStep + 2];

			bool a = R1(R, G, B);

			if (a) {
				// 연산 Error의 소지가 있음.
				// 수정 바람.
				H = (unsigned char)img_HSV->imageData[(j * 3) + i * img_HSV->widthStep];
				S = (unsigned char)img_HSV->imageData[(j * 3) + i * img_HSV->widthStep + 1];
				V = (unsigned char)img_HSV->imageData[(j * 3) + i * img_HSV->widthStep + 2];

				bool c = R3(H, S, V);

				if (c) {
					// 연산 Error의 소지가 있음.
					// 수정 바람.
					Y = (unsigned char)img_YCrCb->imageData[(j * 3) + i * img_YCrCb->widthStep];
					Cr = (unsigned char)img_YCrCb->imageData[(j * 3) + i * img_YCrCb->widthStep + 1];
					Cb = (unsigned char)img_YCrCb->imageData[(j * 3) + i * img_YCrCb->widthStep + 2];

					bool b = R2(Y, Cr, Cb);

					if (b) {
						// 연산 Error의 소지가 있음.
						// 수정 바람.
						dst->imageData[j + i * dst->widthStep] = (unsigned char) 255;
					}
				}
			}
		}
	}

	cvErode(dst, dst, 0, MOP_NUM);
	cvDilate(dst, dst, 0, MOP_NUM);
}

bool Hand_recognition::is_Hand(IplImage *src) {
	int count = 0;

	for (register int i = 0; i < src->width; i++) {
		for (register int j = 0; j < src->height; j++) {
			// 연산 Error의 소지가 있음.
			// 수정 바람.
			if ((unsigned char)src->imageData[i + j * src->widthStep] == 255)
				count++;
		}
	}

	if (count > T_SKIN_NUM)
		return true;

	return false;
}

void Hand_recognition::Sub_prevFrame(IplImage *src, IplImage *dst, bool first) {
	static int frame_count = 0;

	if (first) {
		if (prev_ground != NULL)
			cvReleaseImage(&prev_ground);
		// 여기도 Error의 소지가 있음.
		// 고려 바람.
		prev_ground = NULL;
	}

	if (prev_ground == NULL) {
		prev_ground = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);
		present_ground = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);
		/*cvCvtColor(src, prev_ground, CV_BGR2GRAY);*/
		cvCopy(src, prev_ground);
	}

	cvZero(dst);

	if (frame_count == 1) {
		//cvCvtColor(src, present_ground, CV_BGR2GRAY);
		cvCopy(src, present_ground);

		for (int i = 0; i < src->width; i++) {
			for (int j = 0; j < src->height; j++) {
				// 연산 Error의 소지가 있음.
				// 수정 바람.
				unsigned char SUB_B = abs((unsigned char)src->imageData[(i * 3) + j * src->widthStep] - (unsigned char)prev_ground->imageData[(i * 3) + j * prev_ground->widthStep]);
				unsigned char SUB_G = abs((unsigned char)src->imageData[(i * 3) + j * src->widthStep + 1] - (unsigned char)prev_ground->imageData[(i * 3) + j * prev_ground->widthStep + 1]);
				unsigned char SUB_R = abs((unsigned char)src->imageData[(i * 3) + j * src->widthStep + 2] - (unsigned char)prev_ground->imageData[(i * 3) + j * prev_ground->widthStep + 2]);

				if(SUB_B > SUB_THRESHOLD || SUB_G > SUB_THRESHOLD || SUB_R > SUB_THRESHOLD) {
					// 연산 Error의 소지가 있음.
					// 수정 바람.
					dst->imageData[i + j * dst->widthStep] = (unsigned char)255;
				}
			}
		}
		//cvCopy(present_ground, prev_ground);

		cvSmooth(dst, dst, CV_MEDIAN,3,3);
		cvErode(dst, dst, 0, 2);
		cvDilate(dst, dst, 0, 2);

		frame_count = 0;
	}

	frame_count++;
}

void Hand_recognition::Init_diff() {
	cvReleaseImage(&prev_ground);
	cvReleaseImage(&present_ground);
}