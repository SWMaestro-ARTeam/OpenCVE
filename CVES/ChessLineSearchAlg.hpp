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

#ifndef _ChessLineSearchAlg_hpp_
#define _ChessLineSearchAlg_hpp_

#include "SystemDependency.hpp"
#include "CVESDependent.hpp"

#ifdef WINDOWS_SYS
#include <windows.h>
#include <process.h>
#elif POSIX_SYS

#endif

using namespace std;

class ChessLineSearchAlg {
private:
	typedef struct _GraySideLinesPointStruct {
		ChessLineSearchAlg *T_ChessLineSearchAlg;
		IplImage *chess_image;
		vector<MyGrayPoint> *Lines;
		vector<MyPoint> *InLinePoints;
		MyLinePoint *LinePoint;
		bool XYflag;

		_GraySideLinesPointStruct(ChessLineSearchAlg *_ChessLineSearchAlg, IplImage *image, vector<MyGrayPoint> *line, MyLinePoint *line_point, vector<MyPoint> *in_line_point, bool xyFlag) {
			T_ChessLineSearchAlg = _ChessLineSearchAlg;
			chess_image = image;
			Lines = line;
			LinePoint = line_point;
			InLinePoints = in_line_point;
			XYflag = xyFlag;
		}
	} GraySideLinesPointStruct;
public:
	ChessLineSearchAlg();
	~ChessLineSearchAlg();

	// 모든 체스판 라인이 검출되기 위한 위치는 찾는데 사용되는 변수.
	int Linefindcount_x1, Linefindcount_y1, Linefindcount_x2, Linefindcount_y2;

	// Linefindcount 함수들의 증감을 결정하는 변수.
	bool flag_x1, flag_y1, flag_x2, flag_y2;

	vector<MyPoint> in_line_point_x1, in_line_point_x2, in_line_point_y1, in_line_point_y2;
	vector<MyGrayPoint> line_x1, line_x2, line_x_mid, line_y1, line_y2, line_y_mid;

	MyLinePoint line_point_x1, line_point_x2, line_point_x_mid, line_point_y1, line_point_y2, line_point_y_mid;

	// MySquarePoint main_square;

	// 영상에서 해당 부분의 grayscale 추출.
	void GetLinegrayScale(IplImage *gray_image, int linefindcount_x1, int linefindcount_y1, int linefindcount_x2, int linefindcount_y2);
	// 체스판에서 각 체스판의 경계를 추출
	void GetgraySidelinesPoint(IplImage *chess_image);
	// 추출된 모든 수직이 되는 모든 선들의 교차점을 구함.
	void GetInCrossPoint(IplImage *chess_image, vector<ChessPoint> *point);
	// 해당 위치의 grayscale을 반환.
	int Getgrayscale(IplImage *gray_image, int x, int y);
	// 두 라인의 교차점을 반환.
	bool GetCrossPoint(MyLinePoint line1, MyLinePoint line2, MyPoint *out);
	// 정보들을 MyLinePoint 형식 반환.
	void SetMyLinePoint(int x1, int y1, int x2, int y2, MyLinePoint *setLinePoint);
	// 영상 이진화 함수.
	void GrayImageBinarization(IplImage *gray_image);
	// 체스판 경계 탐색 함수.
	void GetgraySidelines(IplImage *image, vector<MyGrayPoint> *line, MyLinePoint *line_point, vector<MyPoint> *in_line_point, bool XYFlag);
	// 체스판 보정 함수
	void ChessBoardCorrection(vector<ChessPoint> *Point);

	void MemoryClear();

	MyGrayPoint setMyGrayPoint(int grayscale, int x, int y);
	MyPoint setMyPoint(int x, int y);

	static
#if WINDOWS_SYS
		UINT WINAPI
		//DWORD WINAPI
#elif POSIX_SYS
		// using pthread
		void *
#endif
		GraySideLinesPointThread(
#if WINDOWS_SYS
		LPVOID
#elif POSIX_SYS
		void *
#endif
		Param);
};
#endif