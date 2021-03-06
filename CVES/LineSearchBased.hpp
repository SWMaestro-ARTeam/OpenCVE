﻿//////////////////////////////////////////////////////////////////////////////////////////////
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

#ifndef _LineSearchAlg_hpp_
#define _LineSearchAlg_hpp_

#include "SystemDependency.hpp"
#include "CVESDependent.hpp"

#if defined(WINDOWS_SYS)
#include <windows.h>
#include <process.h>
#elif defined(POSIX_SYS)

#endif

using namespace std;

class LineSearchBased {
private:
	typedef struct _GraySideLinesPointStruct {
		LineSearchBased *T_ChessLineSearchAlg;
		IplImage *ChessImage;
		vector<MyGrayPoint> *Lines;
		vector<MyPoint> *InLinePoints;
		MyLinePoint *LinePoint;
		bool XYflag;

		_GraySideLinesPointStruct(LineSearchBased *_ChessLineSearchAlg, IplImage *_ChessImage, vector<MyGrayPoint> *_Line, MyLinePoint *_LinePoint, vector<MyPoint> *_InLinePoint, bool _XYFlag) {
			T_ChessLineSearchAlg = _ChessLineSearchAlg;
			ChessImage = _ChessImage;
			Lines = _Line;
			LinePoint = _LinePoint;
			InLinePoints = _InLinePoint;
			XYflag = _XYFlag;
		}
	} GraySideLinesPointStruct;

	// 모든 체스판 라인이 검출되기 위한 위치는 찾는데 사용되는 변수.
	int Linefindcount_x1, Linefindcount_x11, Linefindcount_y1, Linefindcount_y11, Linefindcount_x2, Linefindcount_x22, Linefindcount_y2, Linefindcount_y22;

	// Linefindcount 함수들의 증감을 결정하는 변수.
	bool flag_x1, flag_y1, flag_x2, flag_y2, flag_x11, flag_y11, flag_x22, flag_y22;

	bool refine;

	vector<MyPoint> in_line_point_x1, in_line_point_x2, in_line_point_y1, in_line_point_y2, in_line_point_x11, in_line_point_x22, in_line_point_y11, in_line_point_y22;
	vector<MyPoint> true_line_point_x1, true_line_point_x2, true_line_point_y1, true_line_point_y2;

	vector<MyGrayPoint> line_x1, line_x2, line_y1, line_y2, line_x11, line_x22, line_y11, line_y22;

	MyLinePoint line_point_x1, line_point_x2, line_point_y1, line_point_y2, line_point_x11, line_point_x22, line_point_y11, line_point_y22;
	// MySquarePoint main_square;

	// 영상에서 해당 부분의 grayscale 추출.
	void Get_LineAtGrayScale(IplImage *gray_image, int linefindcount_x1, int linefindcount_y1, int linefindcount_x2, int linefindcount_y2, int linefindcount_x11, int linefindcount_y11, int linefindcount_x22, int linefindcount_y22);
	// 체스판에서 각 체스판의 경계를 추출
	void Get_SideLinesPointAtGrayScale(IplImage *chess_image);
	// 추출된 모든 수직이 되는 모든 선들의 교차점을 구함.
	void Get_InCrossPoint(IplImage *chess_image, vector<ChessPoint> *point);
	// 해당 위치의 grayscale을 반환.
	int Get_GrayScale(IplImage *gray_image, int x, int y);
	// 두 라인의 교차점을 반환.
	bool Get_CrossPoint(MyLinePoint line1, MyLinePoint line2, MyPoint *out);
	// 체스판 경계 탐색 함수.
	void Get_SideLinesAtGrayScale(IplImage *image, vector<MyGrayPoint> *line, MyLinePoint *line_point, vector<MyPoint> *in_line_point, bool XYFlag);
	// 체스 코너를 결정할 라인 선택 함수
	void Get_TrueLines(vector<MyPoint> in_line_point1, vector<MyPoint> in_line_point2, vector<MyPoint> *Ture_in_line_point);
	// 정보들을 MyLinePoint 형식 반환.
	void Set_MyLinePoint(int x1, int y1, int x2, int y2, MyLinePoint *setLinePoint);
	//
	MyGrayPoint Set_MyPointAtGrayScale(int grayscale, int x, int y);
	//
	MyPoint Set_MyPoint(int x, int y);
	// 영상 이진화 함수.
	void GrayImageBinarization(IplImage *gray_image);

	void MemoryClear();

	static
#if defined(WINDOWS_SYS)
		UINT WINAPI
		//DWORD WINAPI
#elif defined(POSIX_SYS)
		// using pthread
		void *
#endif
		GraySideLinesPointThread(
#if defined(WINDOWS_SYS)
		LPVOID
#elif defined(POSIX_SYS)
		void *
#endif
		Param);

public:
	LineSearchBased();
	~LineSearchBased();

	void ChessLineSearchProcess(IplImage *Source, vector<ChessPoint> *Point);
};
#endif