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

#include "ChessObjectDetection.hpp"

ChessObjectDetection::ChessObjectDetection() {
	_Canny_HighThreshold = CANNY_EDGE_THRESHOLD_HIGH;
	_Canny_LowThreshold = CANNY_EDGE_THRESHOLD_LOW;
}

ChessObjectDetection::~ChessObjectDetection() {
}

// 이상 없음.
void ChessObjectDetection::ConvertHplane(IplImage *Source) {
	IplImage *_TTemp_Source = cvCreateImage(cvGetSize(Source), IPL_DEPTH_8U, 3);

	cvCvtColor(Source, _TTemp_Source, CV_BGR2HSV);
	cvSetImageCOI(_TTemp_Source, 1);
	cvCopy(_TTemp_Source, _H_Plane);

	cvReleaseImage(&_TTemp_Source);
}

// 이상 없음.
void ChessObjectDetection::ConvertSplane(IplImage *Source) {
	IplImage *_TTemp_Source = cvCreateImage(cvGetSize(Source), IPL_DEPTH_8U, 3);

	cvCvtColor(Source, _TTemp_Source, CV_BGR2HSV);
	cvSetImageCOI(_TTemp_Source, 2);
	cvCopy(_TTemp_Source, _S_Plane);

	cvReleaseImage(&_TTemp_Source);
}

// 이상 없음.
void ChessObjectDetection::Add_CannyImage(IplImage *H_Canny, IplImage *S_Canny, IplImage *Destination) {
	cvZero(Destination);

	for (int i = 0; i < Destination->width; i++) {
		for (int j = 0; j < Destination->height; j++) {
			unsigned char H_Value = (unsigned char)H_Canny->imageData[i + j * H_Canny->widthStep];
			unsigned char S_Value = (unsigned char)S_Canny->imageData[i + j * S_Canny->widthStep];

			if (H_Value != 0 || S_Value != 0)
				Destination->imageData[i + j * Destination->widthStep] = 255;
		}
	}
}

// 이상 없음.
void ChessObjectDetection::Delete_ChessLine(IplImage *Edge, vector<_ChessPoint> CrossPoint) {

}

// 이상 없음.
void ChessObjectDetection::SetCannyThreshold(int Low, int High) {
	_Canny_LowThreshold = Low;
	_Canny_HighThreshold = High;
}

// 이상 없음.
void ChessObjectDetection::DetectObject(IplImage *Source, vector<_ChessPoint> CrossPoint, bool *Board[]) {
	IplImage *_T_H_Canny = cvCreateImage(cvGetSize(Source), IPL_DEPTH_8U, 1);
	IplImage *_T_S_Canny = cvCreateImage(cvGetSize(Source), IPL_DEPTH_8U, 1); 
	IplImage *_T_Add_Canny = cvCreateImage(cvGetSize(Source), IPL_DEPTH_8U, 1);

	ConvertHplane(Source);
	ConvertSplane(Source);

	cvCanny(_H_Plane, _T_H_Canny, _Canny_LowThreshold, _Canny_HighThreshold);
	cvCanny(_S_Plane, _T_S_Canny, _Canny_LowThreshold, _Canny_HighThreshold);

	Add_CannyImage(_T_H_Canny, _T_S_Canny, _T_Add_Canny);

	cvShowImage("Canny", _T_Add_Canny);

	// 체스보드의 라인영향을 최소화시킴.
	Delete_ChessLine(_T_Add_Canny, CrossPoint);

	// 오브젝트 유무를 확인하기 위해서 각 체스보드 그리드 안에 엣지가 존재하는 면적비를 연산.
	float ScoreBoard[8][8]; // 엣지 / 체스그리드 면적 => 스코어.
	_CheckChessboard.Calculate_BoardScore(_T_Add_Canny, CrossPoint, ScoreBoard);

	cvReleaseImage(&_T_Add_Canny);
	cvReleaseImage(&_T_H_Canny);
	cvReleaseImage(&_T_S_Canny);
}
