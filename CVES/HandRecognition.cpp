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

#include "HandRecognition.hpp"

// 무조건 순차적으로 처리한다.

HandRecognition::HandRecognition() {
	_IsInitialize = false;
}

HandRecognition::~HandRecognition() {
	_IsInitialize = false;
}

void HandRecognition::Initialize_HandRecognition(int Width, int Height) {
	// 관심영역의 size를 구함.
	_Image_Width = Width;
	_Image_Height = Height;

	// skin detection에 사용되는 내부 연산 이미지.
	// skin color를 통한 detecion에 문제가 있어 현재 사용하지 않고 있음.
	_Image_YCrCb = cvCreateImage(cvSize(Width, Height), IPL_DEPTH_8U, 3);
	_Image_HSV = cvCreateImage(cvSize(Width, Height), IPL_DEPTH_8U, 3);

	_Image_Previous = NULL;
	_Image_Now = NULL;
	_IsInitialize = true;
}

void HandRecognition::Deinitialize_HandRecognition() {
	if (_Image_YCrCb != NULL)
		cvReleaseImage(&_Image_YCrCb);

	if (_Image_HSV != NULL)
		cvReleaseImage(&_Image_HSV);

	if (_Image_Previous != NULL)
		cvReleaseImage(&_Image_Previous);

	if (_Image_Now != NULL)
		cvReleaseImage(&_Image_Now);

	_IsInitialize = false;
}

void HandRecognition::Initialize_Differential_Image() {
	if (_IsInitialize != false) {
		// 내부 차영상 연산에 사용되는 이미지 할당 해제.
		cvReleaseImage(&_Image_Previous);
		cvReleaseImage(&_Image_Now);
	}
	else {

	}
}

bool HandRecognition::RangeDetectionOfRGBSkinColour(int Value_R, int Value_G, int Value_B) {
	// RGB image의 각 픽셀값을 입력받아서 skin의 범위 해당하는지 return
	bool e1 = (Value_R > 95) && (Value_G > 40) && (Value_B > 20) && ((_V_MAX(Value_R, _V_MAX(Value_G, Value_B)) - _V_MIN(Value_R, _V_MIN(Value_G , Value_B))) > 15) && (abs(Value_R - Value_G) > 15) && (Value_R > Value_G) && (Value_R > Value_B);
	bool e2 = (Value_R > 220) && (Value_G > 210) && (Value_B > 170) && (abs(Value_R - Value_G) <= 15) && (Value_R > Value_B) && (Value_G > Value_B);
	return (e1||e2);
}

bool HandRecognition::RangeDetectionOfYCrCbSkinColour(float Value_Y, float Value_Cr, float Value_Cb) {
	// Y, Cr, Cb image를 통하여 skin 영역의 색상을 구분.
	bool e3 = Value_Cr <= (1.5862 * Value_Cb) + 20;
	bool e4 = Value_Cr >= (0.3448 * Value_Cb) + 76.2069;
	bool e5 = Value_Cr >= (-4.5652 * Value_Cb) + 234.5652;
	bool e6 = Value_Cr <= (-1.15 * Value_Cb) + 301.75;
	bool e7 = Value_Cr <= (-2.2857 * Value_Cb) + 432.85;
	return e3 && e4 && e5 && e6 && e7;
}


bool HandRecognition::RangeDetectionOfHSVSkinColour(float Value_H, float Value_S, float Value_V) {
	// HSV image의 H를 통하여 SKin color 범주에 있는지 확인.
	return (Value_H < 25) || (Value_H > 230);
}

void HandRecognition::Detect_SkinColour(IplImage *Source, IplImage *Destination) {
	if (_IsInitialize != false) {
		// skin color detection을 진행.
		// RGB-H-CbCr Skin Colour Model for Human Face Detection 논문 참조.
		// http://pesona.mmu.edu.my/~johnsee/research/papers/files/rgbhcbcr_m2usic06.pdf 
		cvCvtColor(Source, _Image_YCrCb, CV_BGR2YCrCb);
		cvCvtColor(Source, _Image_HSV, CV_BGR2HSV);

		cvZero(Destination);

		for (register int i = 0; i < Destination->height; i++) {
			for (register int j = 0; j < Destination->width; j++) {
				// RGB 색상계를 이용하여 skin이 아닌 픽셀들을 걸러냄.
				_Plane_B_Value = (unsigned char)Source->imageData[(j * 3) + (i * Source->widthStep)];
				_Plane_G_Value = (unsigned char)Source->imageData[(j * 3) + (i * Source->widthStep) + 1];
				_Plane_R_Value = (unsigned char)Source->imageData[(j * 3) + (i * Source->widthStep) + 2];

				if (RangeDetectionOfRGBSkinColour(_Plane_R_Value, _Plane_G_Value, _Plane_B_Value)) {
					// HSV 색상계에 H를 이용하여 피부 영역 검출
					_Plane_H_Value = (unsigned char)_Image_HSV->imageData[(j * 3) + (i * _Image_HSV->widthStep)];
					_Plane_S_Value = (unsigned char)_Image_HSV->imageData[(j * 3) + (i * _Image_HSV->widthStep) + 1];
					_Plane_V_Value = (unsigned char)_Image_HSV->imageData[(j * 3) + (i * _Image_HSV->widthStep) + 2];

					if (RangeDetectionOfHSVSkinColour(_Plane_H_Value, _Plane_S_Value, _Plane_V_Value)) {
						// YCbCr 색상계를 사용하여 최종 피부색 판정.
						_Plane_Y_Value = (unsigned char)_Image_YCrCb->imageData[(j * 3) + (i * _Image_YCrCb->widthStep)];
						_Plane_Cr_Value = (unsigned char)_Image_YCrCb->imageData[(j * 3) + (i * _Image_YCrCb->widthStep) + 1];
						_Plane_Cb_Value = (unsigned char)_Image_YCrCb->imageData[(j * 3) + (i * _Image_YCrCb->widthStep) + 2];

						if (RangeDetectionOfYCrCbSkinColour(_Plane_Y_Value, _Plane_Cr_Value, _Plane_Cb_Value)) {
							Destination->imageData[j + (i * Destination->widthStep)] = (unsigned char) 255;
						}
					}
				}
			}
		}

		// noise에 의한 영향을 줄이기 위해 모폴로지 연산 적용.
		cvErode(Destination, Destination, 0, MOP_NUM);
		cvDilate(Destination, Destination, 0, MOP_NUM);
		cvDilate(Destination, Destination, 0, 5);
	}
	else {

	}
}

bool HandRecognition::IsHand(IplImage *Source) {
	if (_IsInitialize != false) {
		// binary image를 이용하여 피부색 영역 픽셀이 일정 갯수 이상이면 손이라고 판정.
		int count = 0;

		for (register int i = 0; i < Source->width; i++) {
			for (register int j = 0; j < Source->height; j++) {
				if ((unsigned char)Source->imageData[i + (j * Source->widthStep)] == 255)
					count++;
			}
		}

		if (count > T_SKIN_NUM)
			return true;

		return false;
	}
	else {
	}
}

// rebuild 대상.
void HandRecognition::Subtraction_PreviousFrame(IplImage *Source, IplImage *Destination, bool First) {
	if (_IsInitialize != false) {
		// 손을 검출하기 위한 차영상 적용.
		// RGB image src를 입력받아 binary image dst를 반환.
		static int _TFrameCount = 0;

		// Lab 색상계 실험
		IplImage *Lab_src = cvCreateImage(cvGetSize(Source), IPL_DEPTH_8U, 3);
		IplImage *Lab_prev = cvCreateImage(cvGetSize(Source), IPL_DEPTH_8U, 3);

		// first가 true일 경우 이미지 초기화.
		if (First) {
			if (_Image_Previous != NULL) {
				//Hand Recognition 내부 연산에 사용되는 이미지 할당해제 및 포인터 초기화.
				cvReleaseImage(&_Image_Previous);
				cvReleaseImage(&_Image_Now); 
				_Image_Previous = NULL;
				_Image_Now = NULL;
			}
		}

		// 이전 프레임의 이미지가 존재하지 않을 때, 이전 이미지에 현재 입력받은 이미지를 입력.
		if (_Image_Previous == NULL) {
			_Image_Previous = cvCreateImage(cvGetSize(Source), IPL_DEPTH_8U, 3);
			_Image_Now = cvCreateImage(cvGetSize(Source), IPL_DEPTH_8U, 3);
			cvCopy(Source, _Image_Previous);
		}

		cvZero(Destination);

		// 프로그램의 성능을 위해 몇 프레임마다 차영상을 적용할 것인가를 if문을 통하여 조절할 수 있음.
		if (_TFrameCount == 1) {
			//cvCopy(Source, _Image_Now);
			cvCvtColor(Source, Lab_src, CV_BGR2Lab);
			cvCvtColor(_Image_Previous, Lab_prev, CV_BGR2Lab);

			// image 내부의 모든 픽셀을 대상으로 rgb 차영상 적용.
			for (register int i = 0; i < Source->width; i++) {
				for (register int j = 0; j < Source->height; j++) {
					/*unsigned char SUB_B = abs((unsigned char)Source->imageData[(i * 3) + (j * Source->widthStep)] - (unsigned char)_Image_Previous->imageData[(i * 3) + (j * _Image_Previous->widthStep)]);
					unsigned char SUB_G = abs((unsigned char)Source->imageData[(i * 3) + (j * Source->widthStep) + 1] - (unsigned char)_Image_Previous->imageData[(i * 3) + (j * _Image_Previous->widthStep) + 1]);
					unsigned char SUB_R = abs((unsigned char)Source->imageData[(i * 3) + (j * Source->widthStep) + 2] - (unsigned char)_Image_Previous->imageData[(i * 3) + (j * _Image_Previous->widthStep) + 2]);*/

					unsigned char SUB_L = abs((unsigned char)Lab_src->imageData[(i * 3) + (j * Lab_src->widthStep)] - (unsigned char)Lab_prev->imageData[(i * 3) + (j * Lab_prev->widthStep)]);
					unsigned char SUB_a = abs((unsigned char)Lab_src->imageData[(i * 3) + (j * Lab_src->widthStep) + 1] - (unsigned char)Lab_prev->imageData[(i * 3) + (j * Lab_prev->widthStep) + 1]);
					unsigned char SUB_b = abs((unsigned char)Lab_src->imageData[(i * 3) + (j * Lab_src->widthStep) + 2] - (unsigned char)Lab_prev->imageData[(i * 3) + (j * Lab_prev->widthStep) + 2]);

					/*if(SUB_B > SUB_THRESHOLD || SUB_G > SUB_THRESHOLD || SUB_R > SUB_THRESHOLD) {
						Destination->imageData[i + (j * Destination->widthStep)] = (unsigned char)255;
					}*/

					if(SUB_L > SUB_LabTHRESHOLD*5 && (SUB_a > SUB_LabTHRESHOLD || SUB_b > SUB_LabTHRESHOLD)) {
						Destination->imageData[i + (j * Destination->widthStep)] = (unsigned char)255;
					}
				}
			}

			cvErode(Destination, Destination, 0, 2);
			cvDilate(Destination, Destination, 0, 8);

			_TFrameCount = 0;
		}

		_TFrameCount++;

		cvReleaseImage(&Lab_prev);
		cvReleaseImage(&Lab_src);
	}
	else {
	}
}