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

#ifndef _ChessObjectDetection_hpp_
#define _ChessObjectDetection_hpp_

#include "CVESDependent.hpp"
#include "CheckInChessboard.hpp"

using namespace std;

class ChessObjectDetection {
private:
	CheckInChessboard _CheckChessboard;

	int _Canny_LowThreshold, _Canny_HighThreshold;	// Canny Edge Detection에 사용되는 Threshold

	IplImage *_H_Plane;	// HSV에 H 평면
	IplImage *_S_Plane; // HSV에 S 평면

	void ConvertHplane(IplImage *Source);	// src - RGB, RGB 색상계에서 HSV 색상계의  H평면 분리
	void ConvertSplane(IplImage *Source);  // src - RGB, RGB 색상계에서 HSV 색상계의  S평면 분리

	void Add_CannyImage(IplImage *H_Canny, IplImage *S_Canny, IplImage *Destination); // 두 가지 CannyEdge Detection OR 연산 이미지 생성
	void Delete_ChessLine(IplImage *Edge, vector<ChessPoint> CrossPoint); // 디텍션된 엣지 영상에서 교점들을 사용하여 엣지들을 최소화함

public:
	ChessObjectDetection();
	~ChessObjectDetection();

	void SetCannyThreshold(int Low, int High); // Canny Edge Detection Threshold 재설정
	void DetectObject(IplImage *Source, vector<ChessPoint> CrossPoint, bool *Board[]); // RGB영상과 좌표를 출력을 이용하여 오브젝트의 좌표를 디텍션
};
#endif