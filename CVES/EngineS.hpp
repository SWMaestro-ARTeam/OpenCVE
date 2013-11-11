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
//#include <mutex>

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

#include "CodeConverter.hpp"
// String Tools
#include "StringTools.hpp"

// 손 인식
#include "HandRecognition.hpp"
// Chess Board 인식
#include "ChessRecognition.hpp"
// 레이블링
#include "BlobLabeling.hpp"
// Chess Game을 위한 좌표
#include "ChessGame.hpp"
// Chessboard 면적 계산 및 체스보드 인접 체크
#include "CheckInChessboard.hpp"
// 캐니 엣지를 사용한 Obj Detection
#include "ChessObjectDetection.hpp"

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
	// Recognition Modules.
	BlobLabeling _BlobLabeling;

	HandRecognition _HandRecognition;
	ChessRecognition _ChessRecognition;
	ChessGame _ChessGame;
	CheckInChessboard _CheckInChessboard;
	ChessObjectDetection _ChessObjectDetection;

	// Common Modules.
	Telepathy::Server *_TelepathyServer;
	InternalProtocolSeeker _InternalProtocolSeeker;
	CodeConverter _CodeConverter;
	StringTools _StringTools;

	int _Resolution_Width;
	int _Resolution_Heigth;
	int _ROI_Width;
	int _ROI_Heigth;
	int _ROI_Thickness;

	int _Camera_Number;

	int _ImageProcessMode; //모드 설정/
	bool _SubCheck;
	bool _InHandCheck;
	bool _BeforeHandFirst;
	bool _IsRestorePossible;
	bool _IsTrun;

	CvCapture *_Cam; // 캠.
	// 공유 자원 문제를 피하기 위해, 약 3가지의 Part에서 쓰는 Image를 Queue를 줌.
	// 자원은 무조건 Class 내에서만 할당 되어야 하며, 처리의 결과만 Class 내에서 처리되어
	// Return 되게 하여야 한다.

	queue<IplImage *> _OriginToChessBoardDetection; // Chess Board Detection.
	queue<IplImage *> _OriginToHandDetection; // Hand Detection.

	IplImage *_CamOriginalImage; // 원본 이미지.
	IplImage *_ImageChess; // 처리할 관심영역 속 RGB 이미지.
	IplImage *_ImageSkin; // 차영상 결과 이미지.
	IplImage *_PrevImage; // 차영상을 위해 필요한 이미지.
	IplImage *_ImageSub; // 차영상 촬영이후.
	IplImage *_TempPrev; // 임시 이전 영상 저장 이미지.
	IplImage *_TempPrev2; // 임시 이전 영상 저장 이미지.
	IplImage *_OtherBinaryImage; // 손을 제외한 나머지 이진 영상.
	IplImage *_PureImage;	//원본 ROI 셋팅 영상을 저장하기 위한 이미지.
	IplImage *_CamHSV;

	
	CvSize _Resolution; // 전체 해상도.
	CvSize _ROI_Resolution; // ROI상의 해상도.

	// CVES에서의 ROI 영역이 가지는 요구사항.
	// 1. 해상도 설정값에 따라 항상 변화할 수 있어야 한다.
	// 2. ROI 영역은 구정이 아니라 유동적이어야 한다.
	CvRect _ROIRect; // 관심영역 크기.
	CvScalar _ROIRectColour; // 관심영역을 그릴 RGB color 저장변수.
	CvScalar _FontColour;

	vector<ChessPoint> _CrossPoint;
	vector<int> _PieceIndex;

	//mutex _QueueProtectMutex;
	//mutex _VarProtectMutex;

private:
	bool Initialize_Camera();
	void Deinitialize_Camera();
	// image process 초기화.
	void Initialize_ImageProcessing();
	// 이미지 프로세스를 종료함.
	void Deinitialize_ImageProcessing();

	bool Initialize_TServer();
	void Deinitialize_TServer();

	void Engine_Initializing();
	void Engine_DeInitializing();

	bool Start_Server();
	void Stop_Server();

	//void Set_ClientData(SOCKET Socket, int Type);
	void Process_Info(CommandString *IPCS, SOCKET Socket);

	bool Check_Exit();

	// 매 루프에서 호출되는 image process 함수.
	void Go_ImageProcessing();
	// chess UI 만들기
	void DrawROI(IplImage *Source, float FramePerSecond, CvScalar RGB);
	// 연산에 필요한 이미지 할당.
	void Inter_imageCraete(int roi_width, int roi_height);
	// 차영상 진행.
	void Sub_image(IplImage *Source1, IplImage *Source2, IplImage *Destination);
	// 차영상 결과 이미지에 RGB 색 씌우기.
	void Compose_diffImage(IplImage *rgb, IplImage *bin, CvScalar RGB);
	//입력 영상으로 내부 모드에 따라 이미지 연산. (case문 대체)
	void imgproc_mode();

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

	void EngineS_Start();
};
#endif
