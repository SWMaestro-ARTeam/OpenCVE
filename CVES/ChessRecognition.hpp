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

#include "Common.hpp"
// CVESDependent into ChessLineSearchAlg.
#include "ChessLineSearchAlg.hpp"

#include <mutex>

#if WINDOWS_SYS
#include <Windows.h>
#include <process.h>
//#include <thread>
#elif POSIX_SYS

#endif

using namespace std;

class ChessRecognition {
private:
	int _Width, _Height;
	int _MODE;

	vector<ChessPoint> _CP; // 교점
	vector<std::pair<float, float> > vec_LineX, vec_LineY; // 라인 : <rho, theta>
	IplImage *img_process; // 체스보드 인식에 사용되는 내부 연산 이미지

	HANDLE hThread;

	void Non_Maximum_Suppression(IplImage *image, IplImage *image2, int kernel);
	void cast_seq(CvSeq *linesX, CvSeq *linesY); // 벡터로 변환
	//bool sort_first(pair<float, float> a, pair<float, float> b); //벡터 정렬 규칙
	void mergeLine(vector<std::pair<float, float> > *Lines); //Hough Line Trasform 결과를 체스판에 맞추어 병합.

	// Thread 함수에 대해서는 나중에 수정 하기로 함.
	static UINT WINAPI thread_hough(void *arg); // 쓰레드 함수.
	static UINT WINAPI thread_ChessLineSearchAlg(void *arg);
	//CRITICAL_SECTION cs, vec_cs; // thread 동기화를 위한 cs
	mutex _CSProtectionMutex;
	mutex _Vec_CSProtectionMutex;

	bool thread_exit;

	ChessLineSearchAlg *_CLSA;
public:
	ChessRecognition();
	~ChessRecognition();

	//초기화
	void Initialize_ChessRecognition(int width, int height, int mode);
	//라인 그리기
	void drawLines(vector<pair<float, float> > lines, IplImage *image);
	void drawPoint(IplImage *src, vector<ChessPoint> point);
	//교차점 구하기
	void findIntersections(vector<pair<float, float> > linesX, vector<pair<float, float> > linesY, vector<ChessPoint> *point);

	//라인 return
	void Get_Line(vector<pair<float, float> > *linesX, vector<pair<float, float> > *linesY);
	//처리용 이미지 복사
	void Copy_Img(IplImage *src);
	//교차점 보정함수
	void Refine_CrossPoint(vector<ChessPoint> *point);
	//자원반환
	void exit();

	//wrapper method
	void Chess_recog_wrapper(IplImage *src, vector<ChessPoint> *point);

	void Chess_recognition_process(IplImage *src, vector<ChessPoint> *point);
};
#endif
