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

#include "Thread.hpp"

// CVESDependent into ChessLineSearchAlg.
#include "LineSearchBased.hpp"
#include "HoughLineBased.hpp"

#include <mutex>

#if defined(WINDOWS_SYS)
#include <Windows.h>
#include <process.h>
//#include <thread>
#elif defined(POSIX_SYS)

#endif

using namespace std;

class ChessRecognition {
private:
	HoughLineBased *_HoughLineBased;
	LineSearchBased *_LineSearchBased;
	Thread _Thread;
	
	int _Width, _Height;
	int _MODE;
	bool _EnableThread;
	bool _IsInitialized;

	vector<ChessPoint> _CP; // 교점
	//vector<std::pair<float, float> > vec_LineX, vec_LineY; // 라인 : <rho, theta>
	IplImage *_ChessBoardDetectionInternalImage; // 체스보드 인식에 사용되는 내부 연산 이미지.
	//HANDLE hThread;
	mutex _ChessBoardDetectionInternalImageProtectMutex;
	mutex _Vec_ProtectionMutex;

	// 점 그리기.
	void DrawPoints(IplImage *Source, vector<ChessPoint> Point);
	// 라인 return.
	void Get_Line(vector<pair<float, float> > *XLines, vector<pair<float, float> > *YLines);
	// 교차점 보정함수.
	void Refine_CrossPoint(vector<ChessPoint> *CrossPoint);

	// Thread 함수에 대해서는 나중에 수정 하기로 함.
	static
#if defined(WINDOWS_SYS)
		UINT WINAPI
		//DWORD WINAPI
#elif defined(POSIX_SYS)
		// using pthread
		void *
#endif
		HoughLineThread(
#if defined(WINDOWS_SYS)
		LPVOID
#elif defined(POSIX_SYS)
		void *
#endif
		arg); // 쓰레드 함수.
	
	static
#if defined(WINDOWS_SYS)
		UINT WINAPI
		//DWORD WINAPI
#elif defined(POSIX_SYS)
		// using pthread
		void *
#endif
		LineSearchThread(
#if defined(WINDOWS_SYS)
		LPVOID
#elif defined(POSIX_SYS)
		void *
#endif
		arg);
	//CRITICAL_SECTION cs, vec_cs; // thread 동기화를 위한 cs
public:
	ChessRecognition();
	~ChessRecognition();

	// 초기화.
	void Initialize_ChessRecognition(int Width, int Height, int Mode);
	void Deinitialize_ChessRecognition();

	// 처리용 이미지 복사.
	void Copy_Img(IplImage *Source);
	void Find_ChessPoint(IplImage *Source, vector<ChessPoint> *Point);
};
#endif
