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

#include <stdio.h>
#include <time.h>

#include <queue>
#include <mutex>

// 공통 상수 정의
#include "Common.hpp"
// 공용 변수 정의
#include "GlobalVariables.hpp"
// Internal Protocol Seeker
#include "InternalProtocolSeeker.hpp"
// String Tokenizer
#include "StringTokenizer.hpp"
// Telepathy Module
#include "Telepathy.hpp"
// Time Module
#include "Time.hpp"

// 손 인식
#include "HandRecognition.hpp"
// Chess Board 인식
#include "ChessRecognition.hpp"
// 레이블링
#include "BlobLabeling.hpp"
// Chess Game을 위한 좌표
#include "ChessGame.hpp"

#include "StringTools.hpp"

#if WINDOWS_SYS
// It's for windows dependent Functions.
#include <Windows.h>
#elif POSIX_SYS

#endif

typedef struct _ServerGetInformation {
	SOCKET AnySocket;
	char *Infomations;

	_ServerGetInformation() {
		Infomations = new char[BUFFER_MAX_32767];
	}
} ServerGetInformation;

class EngineS {
private:
	HandRecognition _HandRecognition;
	ChessRecognition _ChessRecognition;
	BlobLabeling _BlobLabeling;
	ChessGame _ChessGame;

	Telepathy::Server *_TelepathyServer;
	InternalProtocolSeeker _InternalProtocolSeeker;

	int _ImageProcessMode;				//모드 설정
	bool _SubCheck;
	bool _InHandCheck;
	bool _BeforeHandFirst;

	CvCapture *_Cam; // 캠.
	IplImage *_CamOriginalImage; // 원본 이미지.
	IplImage *_ImageChess; // 처리할 관심영역 속 RGB 이미지.
	IplImage *_ImageSkin; // 차영상 결과 이미지.
	IplImage *_PrevImage; // 차영상을 위해 필요한 이미지.
	IplImage *_ImageSub; // 차영상 촬영이후.
	IplImage *_TempPrev; // 임시 이전 영상 저장 이미지.
	IplImage *_TempPrev2; // 임시 이전 영상 저장 이미지.
	IplImage *_OtherBinaryImage; // 손을 제외한 나머지 이진 영상.
	IplImage *_PureImage;	//원본 ROI 셋팅 영상을 저장하기 위한 이미지.
	CvRect _ROIRect; // 관심영역 크기.
	CvScalar _RGB; // 관심영역을 그릴 RGB color 저장변수

	vector<ChessPoint> _CrossPoint;
	vector<int> _PieceIndex;

	mutex _QueueProtectMutex;
	mutex _VarProtectMutex;

	// image process 초기화.
	void Initialize_ImageProcessing();
	// 이미지 프로세스를 종료함.
	void Deinitialize_ImageProcessing();

	bool Initialize_TServer();
	void Deinitialize_TServer();

	void Engine_Initializing();
	void Engine_DeInitializing();

	void Set_ClientData(SOCKET Socket, int Type);
	void Process_Info(CommandString *IPCS, SOCKET Socket);
	void Process_Info_Go(CommandString *IPCS);
	void Process_Info_Position(CommandString *IPCS);

	// 여태 이동한 경로 큐를 가져옴. -- 미구현
	void Get_History();
	// 차영상의 결과로 나온 이진 이미지를 계산하여 체스말의 좌표이동을 반환.
	void Calculate_Movement(IplImage *bin, vector<ChessPoint> cross_point, CvPoint *out1, CvPoint *out2);
	bool Check_Exit();

	// 매 루프에서 호출되는 image process 함수.
	void Go_ImageProcessing();
	// chess UI 만들기
	void DrawWindowS(IplImage *src, float fps, CvScalar RGB);

	static void MouseCallback_SetROI(int event, int x, int y, int flags, void *param);

	// 연산에 필요한 이미지 할당.
	void Inter_imageCraete(int roi_width, int roi_height);
	// 차영상 진행.
	void Sub_image(IplImage *src1, IplImage *src2, IplImage *dst);
	// 차영상 결과 이미지에 RGB 색 씌우기.
	void Compose_diffImage(IplImage *rgb, IplImage *bin, CvScalar RGB);
	// p,q,r로 이루어진 삼각형의 넓이 return
	float area_tri(CvPoint p, CvPoint q, CvPoint r);
	//입력 영상으로 내부 모드에 따라 이미지 연산. (case문 대체)
	void imgproc_mode();

	// binary image가 체스보드 안에 픽셀을 가지는지 검사.
	bool Check_InChessboard(IplImage *img, vector<ChessPoint> point);
	// img가 픽셀값을 아무것도 가지지 않는지 체크.
	bool Check_imgZero(IplImage *img);

	// 말이 어느 체스판에 있는지를 체크.
	CvPoint	Get_Chessidx(CvPoint point, vector<ChessPoint> cross_point);
	CvPoint Get_ChessboxPos(int width, int height, vector<ChessPoint> cross_point);

	static void ServerReceivedCallback(char *Buffer, SOCKET ClientSocket);
	static void AnyConnentionNotifier(SOCKET ClientSocket);

	// ClientCommandQueueProcessingThread
	static
#if WINDOWS_SYS
		UINT WINAPI
		//DWORD WINAPI
#elif POSIX_SYS
		// using pthread
		void *
#endif
		ServerCommandQueueProcessingThread(
#if WINDOWS_SYS
		LPVOID
#elif POSIX_SYS
		void *
#endif
		Param);

public:
	EngineS();
	~EngineS();

	bool EngineEnable;
	bool IsStarted;
	bool IsTictokEnable;

	queue<ServerGetInformation *> *CommandQueue;

	bool Start_Server();
	void Stop_Server();

	Telepathy::Server *Get_Telepathy_Server();

	void EngineS_Start();
};
#endif
