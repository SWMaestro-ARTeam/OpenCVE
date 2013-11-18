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

#include "EngineS.hpp"

EngineS *G_EngineS;

#pragma region Constructor & Destructor
EngineS::EngineS() {
	_IsRestorePossible = false;
	
	// Engine을 시작을 위한 Switch 설정.
	EngineEnable = false;
	EngineEnd = false;
	//IsStarted = false;
	IsStarted = true;
	IsTictokEnable = false;

	// Camera에 대한 해상도 및 ROI 변수 크기 설정.
	_Resolution_Width = SERVER_VIEW_DEFAULT_WIDTH;
	_Resolution_Height = SERVER_VIEW_DEFAULT_HEIGHT;
	_ROI_Width = ROI_DEFAULT_WIDTH;
	_ROI_Heigth = ROI_DEFAULT_HEIGHT;
	_ROI_Thickness = ROI_FRAME_THICKNESS;
	
	_Resolution = cvSize(_Resolution_Width, _Resolution_Height);
	_ROI_Resolution = cvSize(_ROI_Width, _ROI_Heigth);

	_Camera_Number = CAM_DEFAULT_NUMBER;

	// 정적 함수에서 불러 올 수 있도록 EngineS를 Global 변수에 할당.
	G_EngineS = this;
	TEngineSFrameCallback = NULL;
	
	// Camera.
	_Cam = NULL;

	// 내부 연산에 사용되는 이미지 포인터 초기화
	_TempPrev = NULL;
	_TempPrev2 = NULL;
	_ImageChess = NULL;
	_ImageSkin = NULL;
	_PrevImage = NULL;
	_ImageSub = NULL;
}

EngineS::~EngineS() {
	_IsRestorePossible = false;
	
	EngineEnable = false;
	EngineEnd = false;
	IsStarted = false;
	IsTictokEnable = false;
	
	G_EngineS = NULL;
}
#pragma endregion Constructor & Destructor

#pragma region Private Functions
#pragma region Initialize & Deinitialize Functions
bool EngineS::Initialize_Camera() {
	bool _TIsInitialize = false;
	// Cam Initialize
	// Engine의 Cam을 가져온다. 이제는 선택 할 수 있다.
	_Cam = cvCaptureFromCAM(_Camera_Number);
	if (_Cam != NULL) {
		cvSetCaptureProperty(_Cam, CV_CAP_PROP_FRAME_WIDTH, _Resolution_Width);
		cvSetCaptureProperty(_Cam, CV_CAP_PROP_FRAME_HEIGHT, _Resolution_Height);
		_TIsInitialize = true;
	}
	else {
		MessageBox(NULL, _CodeConverter.CharToWChar("Camera not Found."), _CodeConverter.CharToWChar("Error"), 0);
		EngineEnable = false;
	}

	return _TIsInitialize;
}

void EngineS::Deinitialize_Camera() {
	cvReleaseCapture(&_Cam);
}

bool EngineS::Initialize_TServer() {
	_TelepathyServer = new Telepathy::Server();

	if (_TelepathyServer->ServerInitialize() != true)
		return false;

	return true;
}

void EngineS::Deinitialize_TServer() {
	if (_TelepathyServer != NULL)
		delete _TelepathyServer;
}

void EngineS::Initialize_ImageProcessing() {
	if (Initialize_Camera() != true)
		EngineEnable = false;

	//
	//_BlobLabeling = new BlobLabeling();
	//_HandRecognition = new HandRecognition();
	//_ChessRecognition = new ChessRecognition();
	//_ChessGame = new ChessGame();
	//
	//_CheckInChessboard = new CheckInChessboard();
	//_ChessObjectDetection = new ChessObjectDetection();

	_ChessRecognitionProcessingPause = false;
	ChessRecognitionInitialize = false;
	HandRecognitionInitialize = false;
	// 모드 초기화.
	//_ImageProcessMode = 0;

	// window 생성 & 마우스 콜백 붙이기.
#if !defined(USING_QT)
	cvNamedWindow("CVES");
#endif
	// 관심영역 설정 사각형 좌표, 크기 초기화
	_ROIRect = cvRect(0, 0, 0, 0);

	_SubCheck	= false;
	_InHandCheck = false;
	_BeforeHandFirst = false;

	_OriginForChessBoardDetection = new queue<IplImage *>();
	_OriginForHandDetection = new queue<IplImage *>();
	// ROI Rect Colour 설정.
	_ROIRectColour = cvScalar(0, 0, 255);
	// ROI Rect의 크기를 정함.
	_ROIRect = Set_ROIRect(_Resolution_Width, _Resolution_Height, _ROI_Width, _ROI_Heigth);
	//_ROIRect = cvRect((SERVER_VIEW_DEFAULT_WIDTH - ROI_DEFAULT_WIDTH) / 2, (SERVER_VIEW_DEFAULT_HEIGHT - ROI_DEFAULT_HEIGHT) / 2, ROI_DEFAULT_WIDTH, ROI_DEFAULT_HEIGHT);
	Alloc_CVESImages();
}

void EngineS::Deinitialize_ImageProcessing(){
	//이미지 처리를 보여주기 위한 모든 window 삭제
	//연결된 Cam Disconnect
	//연산에 사용된 모든 이미지 할당해제
#if !defined(USING_QT)
	cvDestroyAllWindows();
#endif
	Deinitialize_Camera();

	cvReleaseImage(&_DetectionResultOnlyImage);

	//delete _BlobLabeling;
	//delete _HandRecognition;
	//delete _ChessRecognition;
	//delete _ChessGame;
	//delete _CheckInChessboard;
	//delete _ChessObjectDetection;
	/*
	if (temp_prev->imageData != NULL)
		cvReleaseImage(&temp_prev);
	if (temp_prev2->imageData != NULL)
		cvReleaseImage(&temp_prev2);

	if (img_Chess->imageData != NULL)
		cvReleaseImage(&img_Chess);
	if (img_Skin->imageData != NULL)
		cvReleaseImage(&img_Skin);
	if (prev_img->imageData != NULL)
		cvReleaseImage(&prev_img);
	if (img_sub->imageData != NULL)
		cvReleaseImage(&img_sub);
	*/
}

void EngineS::Engine_Initializing() {
	// 1. Server Start.
	Initialize_TServer();
}

void EngineS::Engine_DeInitializing() {
	delete CommandQueue;
	Deinitialize_TServer();
}
#pragma endregion Initialize & Deinitialize Functions

bool EngineS::Start_Server() {
	bool _TIsStarted = false;

	if (_TelepathyServer->IsInitializeServer == true) {
		_TelepathyServer->TAnyConnentionNotifier = AnyConnentionNotifier;
		_TelepathyServer->TServerReceivedCallback = ServerReceivedCallback;
		if (_TelepathyServer->ServerStart()) {
			// Command Queue 생성.
			CommandQueue = new queue<ServerGetInformation *>();
			// Command 처리용 Thread를 생성.
			_Thread.StartThread(ServerCommandQueueProcessingThread, this);
			/*
#if defined(WINDOWS_SYS)
			HANDLE _TThreadHandle = (HANDLE)_beginthreadex(NULL, 0, ServerCommandQueueProcessingThread, this, 0, NULL);
#elif defined(POSIX_SYS)
			pthread_t _TThread;
			pthread_attr_t _TThreadAttr;
			// pthread attribute initialize.
			pthread_attr_init(&_TThreadAttr);
			// Detached thread.
			pthread_attr_setdetachstate(&_TThreadAttr, PTHREAD_CREATE_DETACHED);
			// User space thread.
			pthread_attr_setscope(&_TThreadAttr, PTHREAD_SCOPE_SYSTEM);
			// Create thread.
			pthread_create(&_TThread, NULL, ServerCommandQueueProcessingThread, (void *)this);
#endif
			*/
			_TIsStarted = true;
		}
		else {
			_TelepathyServer->TAnyConnentionNotifier = NULL;
			_TelepathyServer->TServerReceivedCallback = NULL;
		}
	}
	return _TIsStarted;
}

void EngineS::Stop_Server() {
	// 모든게 확실히 된 다음에 다시 재구현.
	_TelepathyServer->TAnyConnentionNotifier = NULL;
	_TelepathyServer->TServerReceivedCallback = NULL;
}

bool EngineS::Start_ImageProcessing() {
	_Thread.StartThread(ChessRecognitionProcessingThread, this);
	_Thread.StartThread(HandRecognitionProcessingThread, this);
	/*
#if defined(WINDOWS_SYS)
	HANDLE _TThreadHandle[2];
	_TThreadHandle[0] = (HANDLE)_beginthreadex(NULL, 0, ChessRecognitionProcessingThread, this, 0, NULL);
	_TThreadHandle[1] = (HANDLE)_beginthreadex(NULL, 0, HandRecognitionProcessingThread, this, 0, NULL);
#elif defined(POSIX_SYS)
	pthread_t _TThread[2];
	pthread_attr_t _TThreadAttr[2];
	for (register int i = 0; i < 2; i++) {
		// pthread attribute initialize.
		pthread_attr_init(&_TThreadAttr[i]);
		// Detached thread.
		pthread_attr_setdetachstate(&_TThreadAttr[i], PTHREAD_CREATE_DETACHED);
		// User space thread.
		pthread_attr_setscope(&_TThreadAttr[i], PTHREAD_SCOPE_SYSTEM);
	}
	
	// Create thread.
	pthread_create(&_TThread[0], NULL, ChessRecognitionProcessingThread, (void *)this);
	pthread_create(&_TThread[1], NULL, HandRecognitionProcessingThread, (void *)this);
#endif
	*/
	return true;
}

void EngineS::Stop_ImageProcessing() {

}

void EngineS::Process_Info(CommandString *IPCS, SOCKET Socket)	{
	bool _TIsInfoGo = false;
	bool _TIsBlackTime = false;
	bool _TIsWhiteTime = false;
	bool _TIsTurn = false;
	bool _TIsInfoPosition = false;
	bool _TIsInfoEnemyMove = false;
	bool _TIsInfoType = false;
	//bool _TIsInfoGo = false;

	// Fetch the next at while.
	while (IPCS->NextCharArrayIter()) {
		int _NSeek_GUIToEngine = _InternalProtocolSeeker.InternalProtocolString_Seeker((const char *)*IPCS->CharArrayListIter);
		switch (_NSeek_GUIToEngine) {
			// Go 뒤로 부터 오는 것들.
			// e.g> "Info Go BlackTime(WhiteTime) xxxxx Turn xxxx"
		case VALUE_I_INFO_GO :
			// "Info Go"
			_TIsInfoGo = true;
			break;
		case VALUE_I_INFO_BLACKTIME :
			// "Info Go BlackTime xxxxx"
			// 현재 들어오는 Socket이외의 다른 Client(상대편)의 표시창에 현재 멈춤 시각을 알려준다.
			// 여기에 오면, 상대편(Black)의 시각이 들어오는 것이다.
			_TIsBlackTime = true;
			break;
		case VALUE_I_INFO_WHITETIME :
			// "Info Go White Time xxxxx"
			// 현재 들어오는 Socket이외의 다른 Client(상대편)의 표시창에 현재 멈춤 시각을 알려준다.
			// 여기에 오면, 상대편(White)의 시각이 들어오는 것이다.
			_TIsWhiteTime = true;
			break;
		case VALUE_I_INFO_TURN :
			// "Info Go Turn xxxx"
			// 여기에 현재 자신의 Turn 수가 적힌다.
			_TIsTurn = true;
			break;

			// Position 뒤로부터 오는 것들.
			// e.g> "Info Position MoveNULL"
			// "Info Position EnemyMove xxxx"
		case VALUE_I_INFO_POSITION :
			// "Info Position"
			_TIsInfoPosition = true;
			break;
		case VALUE_I_INFO_ENEMYMOVE :
			// "Info Position EnemyMove xxxx"
			// 적의 Move이므로.
			// 틀리면 비교한다.
			_TIsInfoEnemyMove = true;
			break;
			// "MoveNULL"이 있다면 불필요한 존재?!
			// 추후 없에기로 한다.
		case VALUE_I_INFO_MOVENULL :
			// "Info Position NoveNULL"
			// 이걸 게임의 시작으로 간주 해야 할 것 같다.
			// Socket을 넘겨 해당하는 Client List에 White 라는 식별을 한다.
			// 그외의 나머지(해봤자 1개 뿐..) Client는 Black으로 처리한다.

			break;
		case VALUE_I_INFO_WHITE :
			for_IterToEnd(list, ClientsList, _TelepathyServer->ClientList) {
				if (_TVal->ClientSocket == Socket && strcmp(_TVal->ClientType, "Client") == 0) {
					// White.
					_TVal->ClientName = _StringTools.ConstCharToChar("White");
				}
			}
			break;
		case VALUE_I_INFO_BLACK :
			for_IterToEnd(list, ClientsList, _TelepathyServer->ClientList) {
				if (_TVal->ClientSocket == Socket && strcmp(_TVal->ClientType, "Client") == 0) {
					// Null Move가 오면 일단 White.
					_TVal->ClientName = _StringTools.ConstCharToChar("Black");
				}
			}
			break;

			// Type 뒤로부터 오는 것들.
			// e.g> "Info Type Client(Observer)"
		case VALUE_I_INFO_TYPE :
			// "Info Type"
			_TIsInfoType = true;
			break;
		case VALUE_I_INFO_TYPE_CLIENT :
			// "Info Type Client"
			if (_TIsInfoType == true) {
				for_IterToEnd(list, ClientsList, _TelepathyServer->ClientList) {
					if (_TVal->ClientSocket == Socket) {
						_TVal->ClientType = _StringTools.ConstCharToChar(STR_I_INFO_TYPE_CLIENT);
						break;
					}
				}
			}
			break;			
		case VALUE_I_INFO_TYPE_OBSERVER :
			// "Info Type Observer"
			if (_TIsInfoType == true) {
				for_IterToEnd(list, ClientsList, _TelepathyServer->ClientList) {
					if (_TVal->ClientSocket == Socket) {
						_TVal->ClientType = _StringTools.ConstCharToChar(STR_I_INFO_TYPE_OBSERVER);
						break;
					}
				}
			}
			break;

		case VALUE_I_ANYVALUES :
			if (_TIsInfoGo) {
				if (_TIsBlackTime) {
					_TIsBlackTime = false;
					// Black Time을 처리할 구문.
					// Chess Game에 넘길 Data.
				}
				else if (_TIsWhiteTime) {
					_TIsWhiteTime = false;
					// White Time을 처리할 구문.
					// Chess Game에 넘길 Data.
				}
				else if (_TIsTurn) {
					_TIsTurn = false;
					// Turn을 처리할 구문.
					// Chess Game에 넘길 Data.
				}
			}
			else if (_TIsInfoPosition) {
				if (_TIsInfoEnemyMove) {

				}
				else if (_TIsInfoEnemyMove) {

				}
			}
			break;
		}
	}
}

//bool EngineS::Check_Exit() {
//	if (_ImageProcessMode == 3)
//		return true;
//	else
//		return false;
//}

CvRect EngineS::Set_ROIRect(int ResolutionWidth, int ResolutionHeight, int ROIWidth, int ROIHeight) {
	int _TCalculateROIWidth;
	int _TCalculateROIHeight;

	// 관심영역 크기 고정.
	// 640 * 480이므로, x: 0, y : 0 라고 한다면, 시작 영역은..
	// 관심영역 x크키 : 440 , 관심영역 y의 크기 : 440.
	// if, (x resolution > ROI) x and (y resolution > ROI y)
	// 해상도로부터의 관심영역 영역 Point :
	// x 축 : ((x축 해상도) / 2) - ((관심영역 크키 x) / 2), 줄여서 (x축 해상도 - 관심영역 크키 x) / 2
	// y 축 : ((y축 해상도) / 2) - ((관심영역 크키 y) / 2), 줄여서 (y축 해상도 - 관심영역 크키 y) / 2

	// 해상도보다 ROI Setting 값이 크면 해상도 값으로 강제 고정.
	if (ResolutionWidth < ROIWidth)
		_TCalculateROIWidth = ResolutionWidth;
	else
		_TCalculateROIWidth = ROIWidth;

	if (ResolutionHeight < ROIHeight)
		_TCalculateROIHeight = ResolutionHeight;
	else
		_TCalculateROIHeight = ROIHeight;

	return cvRect((ResolutionWidth - _TCalculateROIWidth) / 2, (ResolutionHeight - _TCalculateROIHeight) / 2, _TCalculateROIWidth, _TCalculateROIHeight);
}

void EngineS::Alloc_CVESImages() {
	// 내부 연산에 사용되는 이미지 할당.
	_DetectionResultOnlyImage = cvCreateImage(cvSize(_Resolution_Width, _Resolution_Height), IPL_DEPTH_8U, 3);

	_ImageChess = cvCreateImage(cvSize(_ROIRect.width, _ROIRect.height), IPL_DEPTH_8U, 3);
	_TempPrev = cvCreateImage(cvSize(_ROIRect.width, _ROIRect.height), IPL_DEPTH_8U, 3);
	_TempPrev2 = cvCreateImage(cvSize(_ROIRect.width, _ROIRect.height), IPL_DEPTH_8U, 3);
	_PureImage = cvCreateImage(cvSize(_ROIRect.width, _ROIRect.height), IPL_DEPTH_8U, 3);
	_OtherBinaryImage = cvCreateImage(cvSize(_ROIRect.width, _ROIRect.height), IPL_DEPTH_8U, 1);

	_ImageSkin = cvCreateImage(cvSize(_ROIRect.width, _ROIRect.height), IPL_DEPTH_8U, 1);
	_PrevImage = cvCreateImage(cvSize(_ROIRect.width, _ROIRect.height), IPL_DEPTH_8U, 3);
	_ImageSub = cvCreateImage(cvSize(_ROIRect.width, _ROIRect.height), IPL_DEPTH_8U, 1);
}

void EngineS::Draw_ROI(IplImage *Source, float FramePerSecond, CvScalar RGB) {
	// CVES 메인 윈도우에 이미지 UI 구성 및 Frame Per Second 표기
	const int _TLineLength = 30;	// 관심영역을 그릴 라인.
	const int _TROI_Length = 440; // 정사각형 관심영역 크기.
	char _TBuffer[32];

	//CvPoint window_center = cvPoint(SERVER_VIEW_DEFAULT_WIDTH/2, SERVER_VIEW_DEFAULT_HEIGHT/2);

	CvPoint _TROI_LeftTop = cvPoint(_ROIRect.x, _ROIRect.y); // 왼쪽 위.
	CvPoint _TROI_LeftBottom = cvPoint(_ROIRect.x, _ROIRect.y + _ROIRect.width); // 왼쪽 아래.
	CvPoint _TROI_RightTop = cvPoint(_ROIRect.x + _ROIRect.width, _ROIRect.y); // 오른쪽 위.
	CvPoint _TROI_RightBottom = cvPoint(_ROIRect.x + _ROIRect.width, _ROIRect.y + _ROIRect.width); // 오른쪽 아래.

	// 관심영역 격자를 그림.
	// 왼쪽 위. 오른쪽 아래.
	cvDrawLine(Source, cvPoint(_TROI_LeftTop.x + _TLineLength, _TROI_LeftTop.y), _TROI_LeftTop, RGB, _ROI_Thickness);
	cvDrawLine(Source, cvPoint(_TROI_LeftTop.x, _TROI_LeftTop.y + _TLineLength), _TROI_LeftTop, RGB, _ROI_Thickness);
	cvDrawLine(Source, cvPoint(_TROI_RightBottom.x - _TLineLength, _TROI_RightBottom.y), _TROI_RightBottom, RGB, _ROI_Thickness);
	cvDrawLine(Source, cvPoint(_TROI_RightBottom.x, _TROI_RightBottom.y - _TLineLength), _TROI_RightBottom, RGB, _ROI_Thickness);
	// 오른쪽 위, 왼쪽 아래.
	cvDrawLine(Source, cvPoint(_TROI_LeftBottom.x + _TLineLength, _TROI_LeftBottom.y), _TROI_LeftBottom, RGB, _ROI_Thickness);
	cvDrawLine(Source, cvPoint(_TROI_LeftBottom.x, _TROI_LeftBottom.y - _TLineLength), _TROI_LeftBottom, RGB, _ROI_Thickness);
	cvDrawLine(Source, cvPoint(_TROI_RightTop.x - _TLineLength, _TROI_RightTop.y), _TROI_RightTop, RGB, _ROI_Thickness);
	cvDrawLine(Source, cvPoint(_TROI_RightTop.x, _TROI_RightTop.y + _TLineLength), _TROI_RightTop, RGB, _ROI_Thickness);

	// Write Frame Per Sec.
	sprintf(_TBuffer, "%5.2f fps", FramePerSecond);
	CvFont _TCvFont = cvFont(1.0);
	cvPutText(Source, _TBuffer, cvPoint(30, 30), &_TCvFont, cvScalar(0, 0, 255));
}

void EngineS::InternalImageCraete(int roi_width, int roi_height){
	// 내부 연산에 사용되는 이미지 할당.
	_ImageSkin = cvCreateImage(cvSize(roi_width, roi_height), IPL_DEPTH_8U, 1);
	_PrevImage = cvCreateImage(cvSize(roi_width, roi_height), IPL_DEPTH_8U, 3);
	_ImageSub = cvCreateImage(cvSize(roi_width, roi_height), IPL_DEPTH_8U, 1);
}

void EngineS::Sub_image(IplImage *Source1, IplImage *Source2, IplImage *Destination) {
	// src1, src2에 차영상 연산하여 dst의 저장.
	// dst zero이미지로 초기화.
	cvZero(Destination);

	// 그림자 보정을 위한 Lab 색상계 변환.
	IplImage *Lab_src1 = cvCreateImage(cvGetSize(Source1), IPL_DEPTH_8U, 3);
	IplImage *Lab_src2 = cvCreateImage(cvGetSize(Source1), IPL_DEPTH_8U, 3);
	cvCvtColor(Source1, Lab_src1, CV_BGR2Lab);
	cvCvtColor(Source2, Lab_src2, CV_BGR2Lab);
	//cvCopy(Source1, Lab_src1); // 기존 RGB로 테스트.
	//cvCopy(Source2, Lab_src2);

	// 차영상 연산. 각 R,G,B값에 SUB_THRESHOLD를 적용하여 binary image 생성.
	for (register int i = 0; i < Source1->width; i++) {
		for (register int j = 0; j < Source1->height; j++) {
			//3채널에 대하여 각각 픽셀 값의 차이 절댓값을 연산
			unsigned char SUB_L = abs((unsigned char)Lab_src1->imageData[(i * 3) + (j * Lab_src1->widthStep)] - (unsigned char)Lab_src2->imageData[(i * 3) + (j * Lab_src2->widthStep)]);
			unsigned char SUB_a = abs((unsigned char)Lab_src1->imageData[(i * 3) + (j * Lab_src1->widthStep) + 1] - (unsigned char)Lab_src2->imageData[(i * 3) + (j * Lab_src2->widthStep) + 1]);
			unsigned char SUB_b = abs((unsigned char)Lab_src1->imageData[(i * 3) + (j * Lab_src1->widthStep) + 2] - (unsigned char)Lab_src2->imageData[(i * 3) + (j * Lab_src2->widthStep) + 2]);

			if ((SUB_L > SUB_LabTHRESHOLD*5) && (SUB_a > SUB_LabTHRESHOLD || SUB_b > SUB_LabTHRESHOLD)) {
				Destination->imageData[i + (j * Destination->widthStep)] = (unsigned char)255;
			}
			/*if (SUB_L > SUB_THRESHOLD || SUB_a > SUB_THRESHOLD || SUB_b > SUB_THRESHOLD) {
				Destination->imageData[i + (j * Destination->widthStep)] = (unsigned char)255;
			}*/
		}
	}

	// 차영상 연산 결과에 median filter 적용 & 후추 소금 노이즈를 제거하기 위한 mopology 연산 적용.
	cvErode(Destination, Destination, 0, 2);
	cvDilate(Destination, Destination, 0, 2);

	cvReleaseImage(&Lab_src1);
	cvReleaseImage(&Lab_src2);
}

void EngineS::Compose_diffImage(IplImage *rgb, IplImage *bin, CvScalar RGB) {
	// rgb 이미지에 bin이미지를 덫씌움. 디버깅을 위한 함수.
	unsigned char _TBinValue;

	// binary image에 255가 되는 값에 RGB 값으로 갱신.
	for (register int i = 0; i < rgb->width; i++)
		for (register int j = 0; j < rgb->height; j++) {
			_TBinValue = (unsigned char)bin->imageData[i + j * bin->widthStep];

			if (_TBinValue > 20) {
				rgb->imageData[(i * 3) + (j * rgb->widthStep)] = RGB.val[0];
				rgb->imageData[(i * 3) + (j * rgb->widthStep) + 1] = RGB.val[1];
				rgb->imageData[(i * 3) + (j * rgb->widthStep) + 2] = RGB.val[2];
			}
		}
}

void EngineS::See() {
	// Cam 으로부터의 영상입력.
	//double _TFps = cvGetCaptureProperty(_Cam, CV_CAP_PROP_FPS);
	IplImage *_TChessBoardDetection = cvCreateImage(cvSize(_Resolution_Width, _Resolution_Height), IPL_DEPTH_8U, 3);
	IplImage *_THandDetection = cvCreateImage(cvSize(_Resolution_Width, _Resolution_Height), IPL_DEPTH_8U, 3);

	bool _IsNoReleaseChessBoardDetection = false;
	bool _IsNoReleaseHandDetection = false;

	_CamOriginImage = cvQueryFrame(_Cam);
	if (_CamOriginImage->width >= _Resolution_Width
		&& _CamOriginImage->height >= _Resolution_Height) {	
		cvFlip(_CamOriginImage, _CamOriginImage, FLIP_MODE);
		//cvCvtColor(_CamOriginalImage, _CamHSV, CV_BGR2HSV);

		//_CamImageProtectMutex.lock();
		cvCopy(_CamOriginImage, _TChessBoardDetection);
		cvCopy(_CamOriginImage, _THandDetection);
		//_CamImageProtectMutex.unlock();

		// ** Note
		// Thread를 돌리려면, 하나의 Queue를 돌려서 그 자원을 공유하는 것 보다는,
		// 차라리 하나의 Image를 Copy하여 여러 Queue에 주는 것이 공유자원 문제를 피하는 가장 현명한 방법인 듯 하다.
		// Memory를 포기하는 것이 이 문제에서의 가장 현실적인 대안인 듯 하다.

		//cvSmooth(_CamOriginalImage, _CamOriginalImage, CV_MEDIAN, 3);

		//imgproc_mode();

		if (_OriginForChessBoardDetection->size() < 10)
			_OriginForChessBoardDetection->push(_TChessBoardDetection);
		else {
			// 버릴때 그냥 버리면 안되고, 무조건 해제를 해주고 버린다.
			IplImage *_TImage = _OriginForChessBoardDetection->front();
			cvReleaseImage(&_TImage);
			_OriginForChessBoardDetection->pop();
			cvReleaseImage(&_TChessBoardDetection);
		}

		if (_OriginForHandDetection->size() < 10)
			_OriginForHandDetection->push(_THandDetection);
		else {
			// 버릴때 그냥 버리면 안되고, 무조건 해제를 해주고 버린다.
			IplImage *_TImage = _OriginForHandDetection->front();
			cvReleaseImage(&_TImage);
			_OriginForHandDetection->pop();
			cvReleaseImage(&_THandDetection);
		}
	}
	else {
		cvReleaseImage(&_TChessBoardDetection);
		cvReleaseImage(&_THandDetection);
		MessageBox(NULL, _CodeConverter.CharToWChar(
			"Camera Resolution is Invalid at between\n"
			"Camera Resolution and Default Setting.\n"
			"Please Check your Camera device Connection."), _CodeConverter.CharToWChar("Error"), 0);
		EngineEnable = false;
	}

	//cvShowImage("CVES", _CamOriginalImage);
	//cvShowImage("HSV", _CamHSV);
}

void EngineS::Evaluation() {
	// 손이 들어온 이후, 빠져나간 상황에서 judge.
	// 차영상의 결과에 체스말의 이동경로 추적. 캐슬링, 앙파상 처리를 위하여 4개의 오브젝트를 디텍션
	CvPoint out[4];
	out[0] = out[1] = out[2] = out[3] = cvPoint(-1, -1);
	// 체스말의 움직임을 계산.
	_ChessObjectDetection->Get_Movement(_PrevImage, _PureImage, _CrossPoint, out);

	// 디텍션 된 결과가 두개 이상 존재한다면 실행
	if (out[0].x != -1 && out[1].x != -1) {
		// 이동 처리부.
		_InHandCheck = false;
		_SubCheck = false;
		_BeforeHandFirst = true;

		// 이전 보드의 상태를 보고 예측함
		int predicted_mode = _ChessGame->Read_Mode();
		// 예측값과 현재 디텍션된 값을 비교하여 실측값을 넘겨줌
		int out_count = 0;
		for (register int i = 0; i < 4; i++) {
			if (out[i].x != -1) {
				out_count++;
				//printf("(%d, %d)\n", out[i].x, out[i].y);
			}
		}
		predicted_mode = (out_count < predicted_mode ? out_count : predicted_mode);

		// chessgame 이동부.
		//printf("predict: %d, out_count : %d\n", predicted_mode, out_count);
		// Out 결과로, Turn을 출력한다.
		_IsTrun = _ChessGame->Chess_Process(out, predicted_mode);

//<<<<<<< HEAD
		//_DetectionResultOnlyImageProtectMutex.lock();
		//if (_ChessGame->Check_InvalidMove(_DetectionResultOnlyImage, _CrossPoint, out) != true) {
//=======
		// Check_InvalidMove가 false일 때는 정상움직임, Check_InvalildMove가 true일때는 InvalidMove
		if(_ChessGame->Check_InvalidMove(_DetectionResultOnlyImage, _CrossPoint, out, 100, 20) == false){
//>>>>>>> origin/CVES_NewEngine_Extended
			string _TString = string("Move ").append(string(_ChessGame->Get_RecentMove()));
			// Game을 하고 있는 Client를 검색한다.
			// 들어온 Client 중에 알맞은 Client에게 답을 보낸다.
			for_IterToEnd(list, ClientsList, _TelepathyServer->ClientList) {
				if (_IsTrun == true && strcmp("White", _TVal->ClientName) == 0) {
					// White Turn일 때.
					char *_TCharArr = (char *)_StringTools.StringToConstCharPointer(_TString.c_str());
					_TelepathyServer->SendDataToOne(_TCharArr, _TVal->ClientSocket);
				}
				else if (_IsTrun != true && strcmp("Black", _TVal->ClientName) == 0) {
					// Black Turn일 때.
					char *_TCharArr = (char *)_StringTools.StringToConstCharPointer(_TString.c_str());
					_TelepathyServer->SendDataToOne(_TCharArr, _TVal->ClientSocket);
				}
			}
		}
//<<<<<<< HEAD
		//_DetectionResultOnlyImageProtectMutex.unlock();
//=======

//>>>>>>> origin/CVES_NewEngine_Extended
#if defined(DEBUG_MODE)
		//uci에 맞춰 return하는 부분 현재 printf로 출력
		_ChessGame->Show_ChessImage();
#endif
	}

	// CVES process가 죽었을 경우를 대비하여 현재 경로들을 txt파일로 저장 & voting을 통하여 현재 말의 이동경로를 확정.
	// 구현 예정.
}

void EngineS::DisplayInfomation() {
	_DetectionResultOnlyImageProtectMutex.lock();
	if (IsStarted == true) {
		Draw_ROI(_DetectionResultOnlyImage, 
			(_ChessRecognitionProcessingFrames > _HandRecognitionProcessingFrames)
			? _ChessRecognitionProcessingFrames : _HandRecognitionProcessingFrames
			, _ROIRectColour);

		if(_ChessGame->Return_errorFlag() == true){
			_ChessGame->Draw_InvalidMove(_DetectionResultOnlyImage, _CrossPoint, 100, 20);
		}
	}
#if !defined(USING_QT)
	// 화면을 표시한다.
	cvShowImage("CVES", _DetectionResultOnlyImage);
#else
	if (TEngineSFrameCallback != NULL) {
		IplImage *_TCVESView = cvCloneImage(_DetectionResultOnlyImage);
		TEngineSFrameCallback(_TCVESView);
	}
#endif
	_DetectionResultOnlyImageProtectMutex.unlock();
}

void EngineS::Verdict() {
	int _TWait = 33;
	See();
	//Evaluation();
	DisplayInfomation();
	//imgproc_mode();
#if defined(WINDOWS_SYS)
#	if !defined(USING_QT)
	if (cvWaitKey(33) == 27)
		EngineEnable = false;
#	endif
	//_TWait -= 10;
	//Sleep(_TWait);
#endif
}

// 제거 대상 0순위.
//void EngineS::imgproc_mode() {
//	_CamOriginImage = cvQueryFrame(_Cam);
//	cvFlip(_CamOriginImage, _CamOriginImage, FLIP_MODE);
//	//cvCvtColor(_CamOriginalImage, _CamHSV, CV_BGR2HSV);
//
//	// 각 모드에 맞추어 image processing을 실행
//	// mode 0 : 카메라 설정, UI Color 설정, 관심영역 크기 설정
//	// mode 1 : Chessboard Recognition 확인부, 2초동안 프레임을 받아서 Chessboard recognition 수행
//	// mode 2 : 체스말의 움직임 Detection & Chess UI draw, ChessGame 부분 구동
//
//	// 내가 너희들을 다 갈아마셔 버리겠다.
//
//	static bool _TImageCreateCheck = false;
//	static time_t _TTempSec; // mode 1에서 시간을 체크할 변수
//
//	if (_ImageProcessMode == 0) {
//		// 관심영역 설정부.
//		if (_TImageCreateCheck == true) {
//			cvReleaseImage(&_ImageChess);
//			_TImageCreateCheck = false;
//		}
//#if !defined(USING_QT)
//		cvShowImage("CVES", _CamOriginImage);
//#endif
//		_ImageProcessMode++;
//		// 순서대로 B, G, R
//		_ROIRectColour = cvScalar(0, 0, 255);
//		_TTempSec = time(NULL);
//
//		// 관심영역 크기 고정.
//		// 640 * 480이므로, x: 0, y : 0 라고 한다면, 시작 영역은..
//		// 관심영역 x크키 : 440 , 관심영역 y의 크기 : 440.
//		// if, (x resolution > ROI) x and (y resolution > ROI y)
//		// 해상도로부터의 관심영역 영역 Point :
//		// x 축 : ((x축 해상도) / 2) - ((관심영역 크키 x) / 2), 줄여서 (x축 해상도 - 관심영역 크키 x) / 2
//		// y 축 : ((y축 해상도) / 2) - ((관심영역 크키 y) / 2), 줄여서 (y축 해상도 - 관심영역 크키 y) / 2
//		_ROIRect = cvRect((SERVER_VIEW_DEFAULT_WIDTH - ROI_DEFAULT_WIDTH) / 2, (SERVER_VIEW_DEFAULT_HEIGHT - ROI_DEFAULT_HEIGHT) / 2, ROI_DEFAULT_WIDTH, ROI_DEFAULT_HEIGHT);
//	}
//	else if (_ImageProcessMode == 1/* && IsStarted == true*/) {
//		// 관심영역 재설정 선택 OR 체스보드 인식 확인부.
//		int _TTick = GetTickCount();
//
//		if (_TImageCreateCheck == false) {
//			// 내부 연산에 사용되는 이미지 할당.
//			_ImageChess = cvCreateImage(cvSize(_ROIRect.width, _ROIRect.height), IPL_DEPTH_8U, 3);
//			_TempPrev = cvCreateImage(cvSize(_ROIRect.width, _ROIRect.height), IPL_DEPTH_8U, 3);
//			_TempPrev2 = cvCreateImage(cvSize(_ROIRect.width, _ROIRect.height), IPL_DEPTH_8U, 3);
//			_PureImage = cvCreateImage(cvSize(_ROIRect.width, _ROIRect.height), IPL_DEPTH_8U, 3);
//			_OtherBinaryImage = cvCreateImage(cvSize(_ROIRect.width, _ROIRect.height), IPL_DEPTH_8U, 1);
//			_TImageCreateCheck = true;
//			_ChessRecognition->Initialize_ChessRecognition(_ROIRect.width, _ROIRect.height, RECOGNITION_MODE);
//			_HandRecognition->Initialize_HandRecognition(_ROIRect.width, _ROIRect.height);
//
//			// 연산에 필요한 이미지 할당.
//			InternalImageCraete(_ROIRect.width, _ROIRect.height);
//		}
//
//		cvSetImageROI(_CamOriginImage, _ROIRect);
//		cvSmooth(_CamOriginImage, _CamOriginImage, CV_MEDIAN);
//		cvCopy(_CamOriginImage, _ImageChess);
//		cvCopy(_CamOriginImage, _PureImage);
//
//		// Chessboard recognition.
//		_ChessRecognition->Copy_Img(_ImageChess);
//		_ChessRecognition->Find_ChessPoint(_CamOriginImage, &_CrossPoint);
//		cvResetImageROI(_CamOriginImage);
//
//		// 체스보드 인식이 확실하면 시간 카운트 시작, 교차점이 81개 이하면 다음 단계로 넘어가지 못함.
//		if (_CrossPoint.size() < 81)
//			_TTempSec = time(NULL);
//
//		// mode 1에서 2초 이상 지났을 경우 다음 모드로 진행
//		if (time(NULL) - _TTempSec > 1) {
//			_ImageProcessMode++;
//			_ROIRectColour = cvScalar(0, 255);
//		}
//
//		//float temp_score[8][8];
//		//_ChessObjDetect.Detect_SobelCannyScore(_PureImage, _CrossPoint, temp_score);
//
//		// fps 계산.
//		_TTick = GetTickCount() - _TTick;
//		float _fps = 1000.f/ (float)_TTick;
//
//		// CVES main Window에 UI 구성
//		//DrawWindowS(_CamOriginalImage, _fps, _RGB);
//#if !defined(USING_QT)
//		cvShowImage("CVES", _CamOriginImage);
//#endif
//	}
//	else if (_ImageProcessMode == 2/* && IsStarted == true*/) {
//		// 실제 이미지 처리 실행부.
//		int _TTick = GetTickCount();
//
//		// 관심 영역 설정
//		cvSetImageROI(_CamOriginImage, _ROIRect);
//		cvSmooth(_CamOriginImage, _CamOriginImage, CV_MEDIAN);
//		cvCopy(_CamOriginImage, _ImageChess); // 이미지 처리에 사용될 이미지 복사 - 관심영역 크기의 이미지
//		cvCopy(_CamOriginImage, _PureImage); // 원본 이미지 - 관심영역 크기의 이미지
//
//		// 차영상 및 조건판정 부분.
//		if (_CrossPoint.size() == 81) {
//			if (_SubCheck == false) {
//				// Chessboard recognition.
//				_ChessRecognition->Copy_Img(_ImageChess);
//				_ChessRecognition->Find_ChessPoint(_CamOriginImage, &_CrossPoint);
//
//				// 손이 들어오기 직전 영상을 촬영.
//				_HandRecognition->Subtraction_PreviousFrame(_ImageChess, _ImageSkin, _BeforeHandFirst); // 턴별 차영상
//
//				// 손 진입을 체크하는 bool 변수 초기화 - 이전에 손이 들어왔었다면 false로 수정
//				if (_BeforeHandFirst)
//					_BeforeHandFirst = false;
//
//				if (_CheckInChessboard->Check_InChessboard(_ImageSkin, _CrossPoint)) {
//					// 물체가 체스보드 위로 들어옴.
//					cvCopy(_TempPrev2, _PrevImage);
//#if !defined(USING_QT)
//#	if defined(DEBUG_MODE)
//					cvShowImage("PREV", _PrevImage);
//#	endif
//#endif
//					_SubCheck = true;
//				}
//			}
//			else {
//				// 추후 해야할 작업 : 빠질때 어떻게 작업할 것인가.
//				// 손이 들어옴 판정 이후 작업.
//#if !defined(USING_QT)
//#	if defined(DEBUG_MODE)
//				cvShowImage("유레카1", _ImageChess);
//#	endif
//#endif
//				// 오브젝트 디텍션에 사용되는 차영상 연산 수행.
//				Sub_image(_PrevImage, _ImageChess, _ImageSkin);
//
//				// 차영상 결과를 이미지 처리에 사용되는 이미지로 색 부여
//				/*Compose_diffImage(_ImageChess, _ImageSkin, cvScalar(0, 255, 255));*/
//
//				// BlobLabeling
//				_BlobLabeling->SetParam(_ImageSkin, 1);
//				_BlobLabeling->DoLabeling();
//				_BlobLabeling->DrawLabel(_ImageChess, cvScalar(255,0,255));
//
//				// 손판정
//				// 손 정의 - 차영상 결과 디텍션된 오브젝트.
//				//          오브젝트 중 window 경계에 있는 물체
//				_BlobLabeling->GetSideBlob(_ImageSkin, &_PieceIndex, _OtherBinaryImage); // 손이 아니라고 판정되는 오브젝트를 이진 영상에서 제거
//				Compose_diffImage(_ImageChess, _ImageSkin, cvScalar(100, 100, 255)); // 손만 남은 이진 영상으로 원본 영상에 색을 부여
//
//				// 이미지 처리에 사용되는 이미지에 Chessboard recognition 결과로 연산된 좌표를 표기
//				//_ChessRecognition.drawPoint(_ImageChess, _CrossPoint);
//				cvDilate(_ImageSkin, _ImageSkin, 0, 5);
//#if !defined(USING_QT)
//#if defined(DEBUG_MODE)
//				cvShowImage("skin", _ImageSkin);
//#endif
//#endif
//
//				// 체스보드 안으로 손이 들어왔는지를 확인
//				if (_CheckInChessboard->Check_InChessboard(_ImageSkin, _CrossPoint)) {
//					// img_Skin은 손 추정물체만 남긴 이미지.
//					_InHandCheck = true;
//				}
//				else if (_InHandCheck == true) {
//					// 차영상의 결과에 체스말의 이동경로 추적. 캐슬링, 앙파상 처리를 위하여 4개의 오브젝트를 디텍션.
//					CvPoint out[4];
//					out[0] = out[1] = out[2] = out[3] = cvPoint(-1, -1);
//					// 체스말의 움직임을 계산.
//					//_CheckInChess->Calculate_Movement(_OtherBinaryImage, _CrossPoint, out);
//					_ChessObjectDetection->Get_Movement(_PrevImage, _PureImage, _CrossPoint, out);
//
//					// 디텍션 된 결과가 두개 이상 존재한다면 실행.
//					if (out[0].x != -1 && out[1].x != -1) {
//						// 이동 처리부.
//						_InHandCheck = false;
//						_SubCheck = false;
//						_BeforeHandFirst = true;
//
//						// 이전 보드의 상태를 보고 예측함
//						int predicted_mode = _ChessGame->Read_Mode();
//						// 예측값과 현재 디텍션된 값을 비교하여 실측값을 넘겨줌
//						int out_count = 0;
//						for(int i = 0; i < 4; i++){
//							if(out[i].x != -1){
//								out_count++;
//								//printf("(%d, %d)\n", out[i].x, out[i].y);
//							}
//						}
//						predicted_mode = (out_count < predicted_mode ? out_count : predicted_mode);
//
//						// chessgame 이동부.
//						//printf("predict: %d, out_count : %d\n", predicted_mode, out_count);
//						_IsTrun = _ChessGame->Chess_process(out, predicted_mode);
//
//						// .
//
//						string _TString = string("Move ").append(string(_ChessGame->Get_RecentMove()));
//						for_IterToEnd(list, ClientsList, _TelepathyServer->ClientList) {
//							if (_IsTrun == true && strcmp("White", _TVal->ClientName) == 0) {
//								// White Turn일 때.
//								char *_TCharArr = (char *)_StringTools.StringToConstCharPointer(_TString.c_str());
//								_TelepathyServer->SendDataToOne(_TCharArr, _TVal->ClientSocket);
//							}
//							else if (_IsTrun != true && strcmp("Black", _TVal->ClientName) == 0) {
//								// Black Turn일 때.
//								char *_TCharArr = (char *)_StringTools.StringToConstCharPointer(_TString.c_str());
//								_TelepathyServer->SendDataToOne(_TCharArr, _TVal->ClientSocket);
//							}
//						}
//#if defined(DEBUG_MODE)
//						// uci에 맞춰 return하는 부분 현재 printf로 출력
//						//printf("%s\n", buf);
//						_ChessGame->Show_ChessImage();
//#endif
//					}
//
//					// CVES process가 죽었을 경우를 대비하여 현재 경로들을 txt파일로 저장 & voting을 통하여 현재 말의 이동경로를 확정.
//					// 구현 예정.
//				}
//#if !defined(USING_QT)
//#	if defined(DEBUG_MODE)
//				cvShowImage("compose_diff", _ImageChess);
//#	endif
//#endif
//			}
//		}
//		// 차영상에 이용하기 위한 2프레임 이전 영상의 저장.
//		cvCopy(_TempPrev, _TempPrev2);
//		cvCopy(_PureImage, _TempPrev);
//		cvResetImageROI(_CamOriginImage);
//
//		// 설정된 관심영역 Rect 그리기.
//		//cvDrawRect(_CamOriginImage, cvPoint(_ROIRect.x, _ROIRect.y), cvPoint(_ROIRect.x + _ROIRect.width, _ROIRect.y + _ROIRect.height), _RGB, 2);
//
//		// 초당 프레임수 계산.
//		_TTick = GetTickCount() - _TTick;
//		float _fps = 1000.f/ (float)_TTick;
//
//		// Chessboard Main UI Drawing
//		Draw_ROI(_CamOriginImage, 0.0f, _ROIRectColour);
//#if !defined(USING_QT)
//		cvShowImage("CVES", _CamOriginImage);
//#endif
//	}
//
//	if (cvWaitKey(33) == 27)
//		_ImageProcessMode = 3;
//}

#pragma region Callbacks
void EngineS::ServerReceivedCallback(char *Buffer, SOCKET ClientSocket) {
	//Sleep(100);
	// using mutex.
	//G_EngineS->_QueueProtectMutex.lock();
	ServerGetInformation *_TServerGetInformation = new ServerGetInformation;
	char _TBuffer[BUFFER_MAX_32767];
	memset(_TBuffer, NULL, sizeof(_TBuffer));
	strcpy(_TBuffer, Buffer);
	_TServerGetInformation->Infomations = _TBuffer;
	_TServerGetInformation->AnySocket = ClientSocket;
	G_EngineS->CommandQueue->push(_TServerGetInformation);
	//G_EngineS->_QueueProtectMutex.unlock();
}

void EngineS::AnyConnentionNotifier(SOCKET ClientSocket) {
	Sleep(100);
	// 초기 접속시에 Type을 물어보기 위한 용도로 쓰인다.
	G_EngineS->_TelepathyServer->SendDataToOne((char *)G_EngineS->_StringTools.StringToConstCharPointer(STR_I_PTYPE), ClientSocket);
}
#pragma endregion Callbacks

#pragma region Threads
#pragma region Server Command Queue Processing Thread
// ServerCommandQueueProcessingThread
#if defined(WINDOWS_SYS)
UINT WINAPI
	//DWORD WINAPI
#elif defined(POSIX_SYS)
// using pthread
void *
#endif
	EngineS::ServerCommandQueueProcessingThread(
#if defined(WINDOWS_SYS)
	LPVOID
	//void *
#elif defined(POSIX_SYS)
	void *
#endif
	Param) {
	EngineS *_TEngine_S = (EngineS *)Param;

	while (_TEngine_S->EngineEnable != false) {
		if (_TEngine_S->CommandQueue->empty() != true  && _TEngine_S->_TelepathyServer->IsServerStarted) {
			//_TEngine_S->_QueueProtectMutex.lock();
			char _TStrBuffer[BUFFER_MAX_32767];
			ServerGetInformation *_TServerGetInformation;
			_TServerGetInformation =_TEngine_S->CommandQueue->front();

			memset(_TStrBuffer, NULL, sizeof(_TStrBuffer));
			strcpy(_TStrBuffer, _TServerGetInformation->Infomations);
			_TEngine_S->CommandQueue->pop();
			//_TEngine_S->_QueueProtectMutex.unlock();

			// 내부 Protocol 송신(CVES -> CVEC, CVES -> Observer).
			StringTokenizer *_TStringTokenizer = new StringTokenizer();

			_TStringTokenizer->SetInputCharString((const char *)_TStrBuffer);
			_TStringTokenizer->SetSingleToken(" ");
			if (_TStringTokenizer->StringTokenGo() == false)
				continue;
				//return ;

			CommandString *_TInternalProtocolCS = new CommandString(_TStringTokenizer->GetTokenedCharListArrays());
			int _TSeek_AnyToCVES = _TEngine_S->_InternalProtocolSeeker.InternalProtocolString_Seeker((const char *)*_TInternalProtocolCS->CharArrayListIter);

			switch (_TSeek_AnyToCVES) {
				// CVEC -> CVES
				case VALUE_I_SERVERKILL :
					// Server를 죽인다.
					// 이유를 불문하고 이 명령이 들어오면 바로 죽인다.
					_TEngine_S->EngineEnable = false;
					break;
				case VALUE_I_SERVERISALIVE :
					// CVEC가 Server가 살아있냐는 질문에 2가지 응답으로 답해야 한다.
					// 그런데 실제로, Server가 'Busy' 한다는건 '살아있냐'라는 질의에 대한 응답으로 좀 맞지 않으므로,
					// 'Alive'만 날려준다.
					_TEngine_S->_TelepathyServer->SendDataToOne((char *)_TEngine_S->_StringTools.StringToConstCharPointer(STR_I_ALIVE), _TServerGetInformation->AnySocket);
					break;
				case VALUE_I_STOP :
					// Stop the Server Image Processing.
					_TEngine_S->IsStarted = false;
					break;
				case VALUE_I_START :
					// Start the Server Image Processing.
					_TEngine_S->IsStarted = true;
					break;
				case VALUE_I_ISRESTOREPOSSIBLE :
					// 복구가 가능한가?
					// 만약 이 Message가 왔다면, 이미 한번 죽어서 다시 실행되었다는 뜻이므로, 이전 Chess판의
					// 정보가 있는지 없는지를 검사해야 한다.
					// 만약 Data가 없거나, Data의 Checksum이 맞지 않아 복원에 실패하였다면, 'No'.
					// 그게 아니라면 'Yes'를 날려준다.
					
					// 게임 중간 저장 파일의 유무 여부 판단. 
					//if (_TEngine_S->_IsRestorePossible == true) 
						_TEngine_S->_TelepathyServer->SendDataToOne((char *)_TEngine_S->_StringTools.StringToConstCharPointer(STR_I_RESTOREOK), _TServerGetInformation->AnySocket);
					//else
					//	_TEngine_S->_TelepathyServer->SendDataToOne((char *)_TEngine_S->_StringTools.StringToConstCharPointer(STR_I_RESTORENOT), _TServerGetInformation->AnySocket);

					break;
				case VALUE_I_ISSERVERREADY :
					// IsServerReady 질문을 하면 해야 할 것은 다음과 같다.
					// 1. 
					// Server(CVES)가 모든 준비 되었을 때, ServerisReady를 보낸다.
					_TEngine_S->_TelepathyServer->SendDataToOne((char *)_TEngine_S->_StringTools.StringToConstCharPointer(STR_I_SERVERISREADY), _TServerGetInformation->AnySocket);
					break;
				
				// Any(CVEC, CVEO) -> CVES
				case VALUE_I_INFO :
					// 여기에는 각종 Information이 온다.
					// Setting 값등 중요한 정보가 오기 때문에, 처리를 잘 해주어야 한다.
					_TEngine_S->Process_Info(_TInternalProtocolCS, _TServerGetInformation->AnySocket);
					break;
				case VALUE_I_INFO_TYPE :
					break;

				// Observer -> CVES
				case VALUE_I_STATUSNOW :
					// Status를 요청했으므로, 정보를 날려준다.
					
					break;
				case VALUE_I_TICTOKON :
					// Tictok is On.
					_TEngine_S->IsTictokEnable = true;
					_TEngine_S->_TelepathyServer->SendDataToOne((char *)_TEngine_S->_StringTools.StringToConstCharPointer(STR_I_TICTOKISON), _TServerGetInformation->AnySocket);
					break;
				case VALUE_I_TICTOKOFF :
					// Tictok is Off.
					_TEngine_S->IsTictokEnable = false;
					_TEngine_S->_TelepathyServer->SendDataToOne((char *)_TEngine_S->_StringTools.StringToConstCharPointer(STR_I_TICTOKISOFF), _TServerGetInformation->AnySocket);
					break;
			}

			delete _TInternalProtocolCS;
			delete _TStringTokenizer;
		}
		Sleep(10);
	}

#if defined(WINDOWS_SYS)
	_endthread();
#elif defined(POSIX_SYS)

#endif
	return 0;
}
#pragma endregion Server Command Queue Processing Thread

#pragma region Chess Recognition Processing Thread
// ChessRecognitionProcessingThread
#if defined(WINDOWS_SYS)
UINT WINAPI
//DWORD WINAPI
#elif defined(POSIX_SYS)
// using pthread
void *
#endif
	EngineS::ChessRecognitionProcessingThread(
#if defined(WINDOWS_SYS)
	LPVOID
#elif defined(POSIX_SYS)
	void *
#endif
	Param) {
	EngineS *_TEngine_S = (EngineS *)Param;
	bool first_recognition = false;

	// Chess 인식을 위한 Class 초기화.
	_TEngine_S->_ChessRecognition = new ChessRecognition();
	_TEngine_S->_ChessRecognition->Initialize_ChessRecognition(_TEngine_S->_ROIRect.width, _TEngine_S->_ROIRect.height, RECOGNITION_MODE);
	_TEngine_S->ChessRecognitionInitialize = true;

	while (_TEngine_S->EngineEnable != false) {
		if (/*_TEngine_S->IsStarted == true &&*/_TEngine_S->_OriginForChessBoardDetection->empty() != true
			&& _TEngine_S->_ChessRecognitionProcessingPause != true) {
			// 일단 Chess Image를 받음.
			if (_TEngine_S->_CamOriginImage != NULL || _TEngine_S->_CamOriginImage->imageData != '\0') {
				IplImage *_TChessBoardOriginImage;
				IplImage *_TChessBoardDetectionImage = cvCreateImage(cvSize(_TEngine_S->_ROIRect.width, _TEngine_S->_ROIRect.height), IPL_DEPTH_8U, 3);
				int _TTick = _TEngine_S->_Time.GetTick();
				
				cvZero(_TChessBoardDetectionImage);
				// Queue가 Empty가 아닌지 다시한번 Check.
				if (_TEngine_S->_OriginForChessBoardDetection->empty() != true) {
					// Empty가 아니면, Queue에 있는 Image를 받고, pop 시킨다.
					_TChessBoardOriginImage = _TEngine_S->_OriginForChessBoardDetection->front();
					_TEngine_S->_OriginForChessBoardDetection->pop();
				}
				
				// ROI를 Set 한다.
				cvSetImageROI(_TChessBoardOriginImage, _TEngine_S->_ROIRect);
				
				// Detection 할 Image를 원본 영상에서 Copy한다.
				cvCopy(_TChessBoardOriginImage, _TChessBoardDetectionImage);
				// ChessRecognition Module에 분석을 위한 Image를 보낸다.
				_TEngine_S->_ChessRecognition->Copy_Image(_TChessBoardDetectionImage);
				// Cross Point를 찾는다.
				_TEngine_S->_ChessRecognition->Find_ChessPoint(_TChessBoardOriginImage, &_TEngine_S->_CrossPoint);

				if(_TEngine_S->_CrossPoint.size() == 81 && first_recognition == false){
					// 체스판이 디텍션됬을 때는 색상을 초록색으로 변경한다
					_TEngine_S->_ROIRectColour = cvScalar(0,255);
					first_recognition = true;
				}

				// ROI를 원래대로 돌려 놓는다.
				cvResetImageROI(_TChessBoardOriginImage);

				_TTick = _TEngine_S->_Time.GetTick() - _TTick;
				_TEngine_S->_ChessRecognitionProcessingFrames = 1000.0f / (float)_TTick;

				_TEngine_S->_DetectionResultOnlyImageProtectMutex.lock();
				// Image를 초기화 하고, Detection 결과 전용 Image에 넣는다.
				cvZero(_TEngine_S->_DetectionResultOnlyImage);
				cvCopy(_TChessBoardOriginImage, _TEngine_S->_DetectionResultOnlyImage);
				_TEngine_S->_DetectionResultOnlyImageProtectMutex.unlock();

				// 썼던 Image를 Release한다.
				// Queue로 받아온 _TChessBoardOriginImage의 경우, 보내준 쪽(Go_ImageProcessing)이 아니라, 여기서 Release를 해야 함.
				cvReleaseImage(&_TChessBoardOriginImage);
				cvReleaseImage(&_TChessBoardDetectionImage);
			}
		}
		Sleep(10);
	}
	_TEngine_S->ChessRecognitionInitialize = false;
	_TEngine_S->_ChessRecognition->Deinitialize_ChessRecognition();
	delete _TEngine_S->_ChessRecognition;

#if defined(WINDOWS_SYS)
	_endthread();
#elif defined(POSIX_SYS)

#endif
	return 0;
}
#pragma endregion Chess Recognition Processing Thread

#pragma region Hand Recognition Processing Thread
// HandRecognitionProcessingThread
#if defined(WINDOWS_SYS)
UINT WINAPI
//DWORD WINAPI
#elif defined(POSIX_SYS)
// using pthread
void *
#endif
	EngineS::HandRecognitionProcessingThread(
#if defined(WINDOWS_SYS)
	LPVOID
#elif defined(POSIX_SYS)
	void *
#endif
	Param) {
	EngineS *_TEngine_S = (EngineS *)Param;

	_TEngine_S->_HandRecognition = new HandRecognition();
	_TEngine_S->_BlobLabeling = new BlobLabeling();
	_TEngine_S->_CheckInChessboard = new CheckInChessboard();
	_TEngine_S->_ChessObjectDetection = new ChessObjectDetection();
	_TEngine_S->_ChessGame = new ChessGame();

	_TEngine_S->_HandRecognition->Initialize_HandRecognition(_TEngine_S->_ROIRect.width, _TEngine_S->_ROIRect.height);
	_TEngine_S->_BlobLabeling->Initialize_BlobLabeling();
	_TEngine_S->HandRecognitionInitialize = true;

	while (_TEngine_S->EngineEnable != false) {
		if (_TEngine_S->IsStarted == true && _TEngine_S->_OriginForHandDetection->empty() != true) {
			if (_TEngine_S->_CamOriginImage != NULL || _TEngine_S->_CamOriginImage->imageData != '\0') {
				IplImage *_THandOriginImage;
				IplImage *_THandDetectionImage = cvCreateImage(cvSize(_TEngine_S->_ROIRect.width, _TEngine_S->_ROIRect.height), IPL_DEPTH_8U, 3);
				int _TTick = _TEngine_S->_Time.GetTick();
				
				cvZero(_THandDetectionImage);
				//_TEngine_S->_CamImageProtectMutex.lock();
				
				if (_TEngine_S->_OriginForHandDetection->empty() != true) {
				 _THandOriginImage = _TEngine_S->_OriginForHandDetection->front();
				 _TEngine_S->_OriginForHandDetection->pop();
				}
				cvSetImageROI(_THandOriginImage, _TEngine_S->_ROIRect);

				cvCopy(_THandOriginImage, _THandDetectionImage);
				cvCopy(_THandOriginImage, _TEngine_S->_PureImage); // 원본 이미지 - 관심영역 크기의 이미지
				
				// 차영상 및 조건판정 부분.
				// Chess Recognition Thread는 계속적으로 좌표를 뽑고 있기 때문에 처리 중간에 81개에서
				// 다른 값으로 변화할 수 있는 소지가 크므로, 중간에 Chess Recognition Thread를 중지시킨다.
				// Chess Recognition Thread와의 Sync를 맞추기 위한 조치.
				
				if (_TEngine_S->_CrossPoint.size() == 81) {
					_TEngine_S->_ChessRecognitionProcessingPause = true;	
					if (_TEngine_S->_SubCheck == false) {
						// 손이 들어오기 직전 영상을 촬영.
						_TEngine_S->_HandRecognition->Subtraction_PreviousFrame(_THandDetectionImage, _TEngine_S->_ImageSkin, _TEngine_S->_BeforeHandFirst); // 턴별 차영상

						// 손 진입을 체크하는 bool 변수 초기화 - 이전에 손이 들어왔었다면 false로 수정
						if (_TEngine_S->_BeforeHandFirst)
							_TEngine_S->_BeforeHandFirst = false;

						if (_TEngine_S->_CheckInChessboard->Check_InChessboard(_TEngine_S->_ImageSkin, _TEngine_S->_CrossPoint)) {
							// 물체가 체스보드 위로 들어옴.
							cvCopy(_TEngine_S->_TempPrev2, _TEngine_S->_PrevImage);
//#if !defined(USING_QT)
//#	if defined(DEBUG_MODE)
//							cvShowImage("PREV", _TEngine_S->_PrevImage);
//#	endif
//#endif
							_TEngine_S->_SubCheck = true;
							_TEngine_S->_ROIRectColour = cvScalar(0,255,255);
						}
						// debug
					}
					else {
						// 추후 해야할 작업 : 빠질때 어떻게 작업할 것인가.
						// 손이 들어옴 판정 이후 작업.
						// 오브젝트 디텍션에 사용되는 차영상 연산 수행.
						//_TEngine_S->Sub_image(_TEngine_S->_PrevImage, _THandDetectionImage, _TEngine_S->_ImageSkin);

						// 차영상 결과를 이미지 처리에 사용되는 이미지로 색 부여.
						/*_TEngine_S->Compose_diffImage(_THandDetectionImage, _TEngine_S->_ImageSkin, cvScalar(0, 255, 255));*/

						// BlobLabeling
//<<<<<<< HEAD
//						_TEngine_S->_BlobLabeling->Set_Parameter(_TEngine_S->_ImageSkin, 1);
//						_TEngine_S->_BlobLabeling->Go_Labeling();
//						_TEngine_S->_BlobLabeling->DrawLabel(_THandDetectionImage, cvScalar(255,0,255));
//=======
						//_TEngine_S->_BlobLabeling->SetParam(_TEngine_S->_ImageSkin, 1);
						//_TEngine_S->_BlobLabeling->DoLabeling();
						//_TEngine_S->_BlobLabeling->DrawLabel(_THandDetectionImage, cvScalar(255,0,255));
//>>>>>>> origin/CVES_NewEngine_Extended

						// 손판정
						// 손 정의 - 차영상 결과 디텍션된 오브젝트.
						//          오브젝트 중 window 경계에 있는 물체
//<<<<<<< HEAD
//						_TEngine_S->_BlobLabeling->Get_SideBlob(_TEngine_S->_ImageSkin, &_TEngine_S->_PieceIndex, _TEngine_S->_OtherBinaryImage); // 손이 아니라고 판정되는 오브젝트를 이진 영상에서 제거
//=======
						//_TEngine_S->_BlobLabeling->GetSideBlob(_TEngine_S->_ImageSkin, &_TEngine_S->_PieceIndex, _TEngine_S->_OtherBinaryImage); // 손이 아니라고 판정되는 오브젝트를 이진 영상에서 제거
//>>>>>>> origin/CVES_NewEngine_Extended
#if defined(DEBUG_MODE)
						//_TEngine_S->Compose_diffImage(_THandDetectionImage, _TEngine_S->_ImageSkin, cvScalar(100, 100, 255)); // 손만 남은 이진 영상으로 원본 영상에 색을 부여
#endif
						// 이미지 처리에 사용되는 이미지에 Chessboard recognition 결과로 연산된 좌표를 표기
						//_ChessRecognition.drawPoint(_ImageChess, _CrossPoint);
						_TEngine_S->_HandRecognition->Detect_SkinColour(_THandDetectionImage, _TEngine_S->_ImageSkin);
						cvDilate(_TEngine_S->_ImageSkin, _TEngine_S->_ImageSkin, 0, 5);
//#if !defined(USING_QT)
#	if defined(DEBUG_MODE)
						cvShowImage("skin", _TEngine_S->_ImageSkin);
#	endif
//#endif

						// 체스보드 안으로 손이 들어왔는지를 확인
						if (_TEngine_S->_CheckInChessboard->Check_InChessboard(_TEngine_S->_ImageSkin, _TEngine_S->_CrossPoint)) {
							// img_Skin은 손 추정물체만 남긴 이미지.
							_TEngine_S->_InHandCheck = true;
							_TEngine_S->_ROIRectColour = cvScalar(0,255,255);
						}
						else if(_TEngine_S->_InHandCheck == true) {
							// Evauluation 실행부

							_TEngine_S->Evaluation();
							_TEngine_S->_ROIRectColour = cvScalar(0,255);
						}

//#if !defined(USING_QT)
//#	if defined(DEBUG_MODE)
//						cvShowImage("compose_diff", _TEngine_S->_ImageChess);
//#	endif
//#endif
					}
					// debug
					_TEngine_S->_ChessRecognitionProcessingPause = false;	
				}

				cvCopy(_TEngine_S->_TempPrev, _TEngine_S->_TempPrev2);
				cvCopy(_TEngine_S->_PureImage, _TEngine_S->_TempPrev);
				//cvWaitKey(33); // Thread에서 Memory Leak을 유발.
				
				//cvShowImage("Skin Image", _TEngine_S->_ImageSkin);
				//cvShowImage("Pure Image", _THandDetectionImage);
				cvResetImageROI(_THandOriginImage);
				
				//_TEngine_S->_CamImageProtectMutex.unlock();
				_TTick = _TEngine_S->_Time.GetTick() - _TTick;
				_TEngine_S->_HandRecognitionProcessingFrames = 1000.0f/ (float)_TTick;

				cvReleaseImage(&_THandOriginImage);
				cvReleaseImage(&_THandDetectionImage);
			}
		}
		//Sleep(10);
		cvWaitKey(33);
	}
	_TEngine_S->HandRecognitionInitialize = false;
	_TEngine_S->_HandRecognition->Deinitialize_HandRecognition();
	_TEngine_S->_BlobLabeling->Deinitialize_BlobLabeling();

	delete _TEngine_S->_HandRecognition;
	delete _TEngine_S->_BlobLabeling;
	delete _TEngine_S->_CheckInChessboard;
	delete _TEngine_S->_ChessObjectDetection;
	delete _TEngine_S->_ChessGame;

#if defined(WINDOWS_SYS)
	_endthread();
#elif defined(POSIX_SYS)

#endif
	return 0;
}
#pragma endregion Hand Recognition Processing Thread

#pragma region CVES Processing Thread
#if defined(WINDOWS_SYS)
UINT WINAPI
	//DWORD WINAPI
#elif defined(POSIX_SYS)
// using pthread
void *
#endif
	EngineS::CVESProcessingThread(
#if defined(WINDOWS_SYS)
	LPVOID
#elif defined(POSIX_SYS)
	void *
#endif
	Param) {
	EngineS *_TEngine_S = (EngineS *)Param;

	// Initialize 순서.
	// 1. Telepathy Server.
	// 2. Camera Initialize.
	// 1. Engine이 Start 되면, 우선 Server 기동부터 한다.
	_TEngine_S->Engine_Initializing();
	// 2. 
	_TEngine_S->Initialize_ImageProcessing();
	_TEngine_S->Start_Server();
	_TEngine_S->Start_ImageProcessing();

	// 마치, 차의 시동키 같은 역할.
	while (_TEngine_S->EngineEnable) {
		// Image Processing Engine이 시작되지 않았다면 대기한다.
		// 전체적인 구문에 영향을 줄 수 있으므로 다른 구문을 강구해보는 것도 괜찮은 생각이다.
		if (_TEngine_S->IsStarted != true) {
			// Image 처리 중이었다면, 모든 처리를 중단하고 화면에 Stop을 넣고 중단시킨다.
			// cvPutText(img_Cam, "Stop", cvPoint(30, 30), &cvFont(1.0), cvScalar(0, 100, 100));
			// while (!IsStarted) ;
		}

		// Chess Recognition 및 Hand Recognition을 처리하는 단계가 포함된 함수.
		_TEngine_S->Verdict();

		// 이 구분은 나중에 삭제 해야할 것 같다.
		// 회의 뒤 삭제.
		/*if (_TEngine_S->Check_Exit() == true)
		_TEngine_S->EngineEnable = false;*/
	}

	_TEngine_S->Deinitialize_ImageProcessing();

#if defined(WINDOWS_SYS)
	_endthread();
#elif defined(POSIX_SYS)

#endif
	// 가장 마지막으로, Engine이 End가 됨을 알림.
	_TEngine_S->EngineEnd = true;
	return 0;
}
#pragma endregion CVES Processing Thread
#pragma endregion Threads
#pragma endregion Private Functions

#pragma region Public Functions
CvSize EngineS::Get_Resolution() {
	return _Resolution;
}

IplImage *EngineS::Get_FrameImage() {
	IplImage *_TFrame = cvCreateImage(_Resolution, IPL_DEPTH_8U, 3);
	_DetectionResultOnlyImageProtectMutex.lock();
	cvCopy(_DetectionResultOnlyImage, _TFrame);
	_DetectionResultOnlyImageProtectMutex.unlock();
	return _TFrame;
}

bool EngineS::IsAllInitialize() {
	return ChessRecognitionInitialize & HandRecognitionInitialize & EngineEnable;
}

void EngineS::EngineS_Start() {
	Sleep(100);
	_Thread.StartThread(CVESProcessingThread, this);
	/*
#if defined(WINDOWS_SYS)
	HANDLE _TThreadHandle = (HANDLE)_beginthreadex(NULL, 0, CVESProcessingThread, this, 0, NULL);
#elif defined(POSIX_SYS)
	pthread_t _TThread;
	pthread_attr_t _TThreadAttr;
	// pthread attribute initialize.
	pthread_attr_init(&_TThreadAttr);
	// Detached thread.
	pthread_attr_setdetachstate(&_TThreadAttr, PTHREAD_CREATE_DETACHED);
	// User space thread.
	pthread_attr_setscope(&_TThreadAttr, PTHREAD_SCOPE_SYSTEM);
	// Create thread.
	pthread_create(&_TThread, NULL, CVESProcessingThread, (void *)this);
#endif
	*/
}

void EngineS::EngineS_Destroy() {
	EngineEnable = false;
}
#pragma endregion Public Functions
