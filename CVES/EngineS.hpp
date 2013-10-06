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

#ifndef _EngineS_hpp_
#define _EngineS_hpp_

#include "Common.hpp"

#include "GlobalVariables.hpp"

#include "InternalProtocolSeeker.hpp"

#include "Telepathy.hpp"

#include "Hand_recognition.hpp"
#include "Chess_recognition.hpp"
#include "BlobLabeling.hpp"
#include "chess_game.hpp"
//#include "Img_Process.hpp"

#include <stdio.h>
#include <cv.h>
#include <highgui.h>

#if WINDOWS_SYS
// It's for windows dependent Functions.
#include <Windows.h>
#elif POSIX_SYS

#endif
#include <time.h>

#define WIDTH 640
#define HEIGHT 480
#define RECOGNITION_MODE 2 // 1 : hough Line, 2 : 규홍 recognition

class EngineS {
private:
	Hand_recognition Find_Hand;
	Chess_recognition Find_Chess;
	CBlobLabeling CBlob;
	chess_game CHESS_GAME;

	int ImgProcess_Mode;				//모드 설정
	bool Sub_check;
	bool InHand_Check;
	bool BeforeHand_first;

	CvCapture *Cam;										//캠
	IplImage *img_Cam;								//원본 이미지
	IplImage *img_Chess;							//처리할 관심영역 속 RGB 이미지
	IplImage *img_Skin;								//차영상 결과 이미지
	IplImage *prev_img;								//차영상을 위해 필요한 이미지
	IplImage *img_sub;								//차영상 촬영이후
	IplImage *temp_prev;							//임시 이전 영상 저장 이미지
	IplImage *temp_prev2;							//임시 이전 영상 저장 이미지
	IplImage *other;									//손을 제외한 나머지 이진 영상
	CvRect ImgProcess_ROI;						//관심영역 크기

	vector<Chess_point> cross_point;
	vector<int> piece_idx;

	void Engine_Initializing();
	void Engine_DeInitializing();

	static void MouseCallback_SetROI(int event, int x, int y, int flags, void *param);

	void Inter_imageCraete(int roi_width, int roi_height);													//연산에 필요한 이미지 할당
	void Sub_image(IplImage *src1, IplImage *src2, IplImage *dst);									//차영상 진행
	void Compose_diffImage(IplImage *rgb, IplImage *bin, CvScalar RGB);							//차영상 결과 이미지에 RGB 색 씌우기
	bool Check_InChessboard(IplImage *img, vector<Chess_point> point);							//binary image가 체스보드 안에 픽셀을 가지는지 검사
	bool Check_imgZero(IplImage *img);																							//img가 픽셀값을 아무것도 가지지 않는지 체크
	CvPoint	Get_Chessidx(CvPoint point, vector<Chess_point> cross_point);										//말이 어느 체스판에 있는지를 체크
	float area_tri(CvPoint p, CvPoint q, CvPoint r);
public:
	EngineS();
	~EngineS();

	bool EngineEnable;

	void Init_process();			//image process 초기화
	void Get_History();				//여태 이동한 경로 큐를 가져옴 -- 미구현
	void Exit_imgProcess();			//이미지 프로세스를 종료함
	void Do_imgprocess();			//매 루프에서 호출되는 image process 함수
	bool Check_Exit();

	void EngineS_Start();
};

#endif