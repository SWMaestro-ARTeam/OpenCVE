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

#ifndef _CheckInChessboard_hpp_
#define _CheckInChessboard_hpp_

#include "CVESDependent.hpp"

using namespace std;

class CheckInChessboard {
private:
	// p,q,r로 이루어진 삼각형의 넓이 return
	float area_tri(CvPoint p, CvPoint q, CvPoint r);
	// 말이 어느 체스판에 있는지를 체크.
	CvPoint	Get_Chessidx(CvPoint point, vector<ChessPoint> cross_point);
	// width, height가 가리키는 픽셀이 어느 체스보드 인덱스를 가지는지를 계산하여 반환.
	CvPoint Get_ChessboxPos(int width, int height, vector<ChessPoint> cross_point);

public:
	CheckInChessboard();
	~CheckInChessboard();

	// binary image가 체스보드 안에 픽셀을 가지는지 검사.
	bool Check_InChessboard(IplImage *img, vector<ChessPoint> point);
	// img가 픽셀값을 아무것도 가지지 않는지 체크.
	bool Check_imgZero(IplImage *img);
	// 차영상의 결과로 나온 이진 이미지를 계산하여 체스말의 좌표이동을 반환.
	void Calculate_Movement(IplImage *bin, vector<ChessPoint> cross_point, CvPoint *out1, CvPoint *out2);
};

#endif