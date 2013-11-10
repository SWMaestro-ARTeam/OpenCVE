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


#include "ChessObjDetection.hpp"

ChessObjDetection::ChessObjDetection(void) {
	_Canny_HighThreshold = 250;
	_Canny_LowThreshold = 240;
}


ChessObjDetection::~ChessObjDetection(void) {
}

void ChessObjDetection::SetCannyThreshold(int Low, int High) {
	_Canny_LowThreshold = Low;
	_Canny_HighThreshold = High;
}

void ChessObjDetection::ConvertHplane(IplImage *src) {
	IplImage *temp_src = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);

	cvCvtColor(src, temp_src, CV_BGR2HSV);
	cvSetImageCOI(temp_src, 1);
	cvCopy(temp_src, _H_Plane);

	cvReleaseImage(&temp_src);
}

void ChessObjDetection::ConvertSplane(IplImage *src) {
	IplImage *temp_src = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);

	cvCvtColor(src, temp_src, CV_BGR2HSV);
	cvSetImageCOI(temp_src, 2);
	cvCopy(temp_src, _S_Plane);

	cvReleaseImage(&temp_src);
}

void ChessObjDetection::add_CannyImg(IplImage *H_canny, IplImage *S_canny, IplImage *dst) {
	cvZero(dst);

	for (int i = 0; i < dst->width; i++) {
		for (int j = 0; j < dst->height; j++) {
			unsigned char H_Value = (unsigned char)H_canny->imageData[i + j * H_canny->widthStep];
			unsigned char S_Value = (unsigned char)S_canny->imageData[i + j * S_canny->widthStep];

			if (H_Value != 0 || S_Value != 0)
				dst->imageData[i + j * dst->widthStep] = 255;
		}
	}
}

void ChessObjDetection::DetectObj(IplImage *src, vector<_ChessPoint> _cross_point, bool *board[]) {
	IplImage *H_Canny = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	IplImage *S_Canny = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1); 
	IplImage *Add_Canny = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);

	ConvertHplane(src);
	ConvertSplane(src);

	cvCanny(_H_Plane, H_Canny, _Canny_LowThreshold, _Canny_HighThreshold);
	cvCanny(_S_Plane, S_Canny, _Canny_LowThreshold, _Canny_HighThreshold);

	add_CannyImg(H_Canny, S_Canny, Add_Canny);

	cvShowImage("Canny", Add_Canny);

	// 체스보드의 라인영향을 최소화시킴
	Delete_ChessLine(Add_Canny, _cross_point);

	// 오브젝트 유무를 확인하기 위해서
	// 각 체스보드 그리드 안에 엣지가 존재하는 면적비를 연산
	float score_board[8][8]; // 엣지 / 체스그리드 면적 => 스코어
	_CheckChessboard->Cal_BoardScore(Add_Canny, _cross_point, score_board);


	cvReleaseImage(&Add_Canny);
	cvReleaseImage(&H_Canny);
	cvReleaseImage(&S_Canny);
}

void ChessObjDetection::Delete_ChessLine(IplImage *edge, vector<_ChessPoint> _cross_point) {

}
