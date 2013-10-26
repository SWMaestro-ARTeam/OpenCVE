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

#ifndef _ChessRecognition_hpp_
#define _ChessRecognition_hpp_

#include <cv.h>
#include <highgui.h>
#include <Windows.h>
#include <process.h>
#include <thread>

#define H_THREAD 12

#define MAX_CORNER 5000

using namespace std;

typedef struct {
	CvPoint Cordinate; // 좌표 위치
	CvPoint index; // 좌표 인덱스
} Chess_point;

class ChessRecognition {
private:
	int _Width, _Height;
	int MODE;

	vector<Chess_point> CP; // 교점
	vector<std::pair<float, float>> vec_LineX, vec_LineY; // 라인 : <rho, theta>
	IplImage *img_process;

	HANDLE hThread;

	void NMS2(IplImage* image, IplImage* image2, int kernel);	//
	void cast_seq(CvSeq* linesX, CvSeq* linesY); // 벡터로 변환
	//bool sort_first(pair<float, float> a, pair<float, float> b);	//벡터 정렬 규칙
	void mergeLine( vector<std::pair<float, float>> *Lines);

	static UINT WINAPI thread_hough(void *arg); //쓰레드 함수
	static UINT WINAPI thread_GH(void *arg);
	CRITICAL_SECTION cs, vec_cs; //thread 동기화를 위한 cs
	bool thread_exit;

public:
	ChessRecognition();
	~ChessRecognition();

	//wrapper method
	void Chess_recog_wrapper(IplImage *src, vector<Chess_point> *point);
};
#endif