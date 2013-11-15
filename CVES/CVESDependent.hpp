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

#ifndef _CVESDependent_hpp_
#define _CVESDependent_hpp_

#include <opencv/cv.h>
#include <opencv/highgui.h>

#define CANNY_EDGE_THRESHOLD_LOW 240
#define CANNY_EDGE_THRESHOLD_HIGH 250

#define ROI_DEFAULT_WIDTH 440
#define ROI_DEFAULT_HEIGHT 440
#define ROI_FRAME_THICKNESS 4

#define _V_MAX(a, b) (((a) > (b)) ? (a) : (b))
#define _V_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define _V_SWAP(x, y) { int t; t = x; x = y; y = t; }

// using EngineS, ChessLineSearchAlg, ChessRecognition.
#pragma region struct_ChessPoint
typedef struct _ChessPoint {
	CvPoint Cordinate; // 좌표 위치.
	CvPoint Index; // 좌표 인덱스.
} ChessPoint;
#pragma endregion struct_ChessPoint

// using BlobLabeling.
typedef struct _Visited {
	bool	bVisitedFlag;
	CvPoint ptReturnPoint;
} Visited;

// using ChessLineSearchAlg.
// 점의 위치를 저장하기 위한 구조체.
typedef struct _MyPoint {
	int x, y;
} MyPoint;

// 라인의 양끝 점을 저장하기 위한 구조체.
typedef struct _MyLinePoint {
	int x1, y1, x2, y2;
} MyLinePoint;

// 해당 위치와 함께 grayscale 의 값을 저장하기 위한 구조체.
typedef struct _MyGrayPoint {
	int grayscale, x, y;
} MyGrayPoint;

// 사각형의 네 꼭지점을 저장하기 위한 구조체.
typedef struct _MySquarePoint {
	MyPoint LeftTop, LeftBottom, RightTop, RightBottom;
} MySquarePoint;
#endif