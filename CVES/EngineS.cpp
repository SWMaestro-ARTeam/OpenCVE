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

EngineS::EngineS() {
	EngineEnable = false;
	IsStarted = false;
	IsTictokEnable = false;
	G_EngineS = this;

	// 내부 연산에 사용되는 이미지 포인터 초기화
	_TempPrev = NULL;
	_TempPrev2 = NULL;
	_ImageChess = NULL;
	_ImageSkin = NULL;
	_PrevImage = NULL;
	_ImageSub = NULL;
}

EngineS::~EngineS() {
	EngineEnable = false;
	IsStarted = false;
	IsTictokEnable = false;
	G_EngineS = NULL;
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
	//Cam init
	// Engine의 Cam을 가져온다. 그것도 0번째.
	_Cam = cvCaptureFromCAM(0);
	if (_Cam != NULL) {
		cvSetCaptureProperty(_Cam, CV_CAP_PROP_FRAME_WIDTH, SERVER_VIEW_DEFAULT_WIDTH);
		cvSetCaptureProperty(_Cam, CV_CAP_PROP_FRAME_HEIGHT, SERVER_VIEW_DEFAULT_HEIGHT);
	}

	// 모드 초기화.
	_ImageProcessMode = 0;

	// window 생성 & 마우스 콜백 붙이기.
	cvNamedWindow("CVES");
	/*cvSetMouseCallback("CVES", MouseCallback_SetROI, this);*/

	// 관심영역 설정 사각형 좌표, 크기 초기화
	_ROIRect = cvRect(0, 0, 0, 0);

	_SubCheck	= false;
	_InHandCheck = false;
	_BeforeHandFirst = false;
}

void EngineS::Deinitialize_ImageProcessing(){
	//이미지 처리를 보여주기 위한 모든 window 삭제
	//연결된 Cam Disconnect
	//연산에 사용된 모든 이미지 할당해제
	cvDestroyAllWindows();

	cvReleaseCapture(&_Cam);

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

bool EngineS::Check_Exit() {
	if (_ImageProcessMode == 3)
		return true;
	else
		return false;
}

void EngineS::Go_ImageProcessing(){
	// Cam으로부터의 영상입력.
	_CamOriginalImage = cvQueryFrame(_Cam);
	cvFlip(_CamOriginalImage, _CamOriginalImage, FLIP_MODE);
	//cvSmooth(_CamOriginalImage, _CamOriginalImage, CV_MEDIAN, 3);
	// 모드에 따른 이미지 프로세스 수행.

	imgproc_mode();
}

void EngineS::MouseCallback_SetROI(int event, int x, int y, int flags, void *param) {
	// mouse input을 사용하여 관심영역 설정.
	EngineS* _TEngineS = (EngineS*)param;
	static bool _TDownCheck = false;

	if (_TEngineS->_ImageProcessMode == 0) {
		switch (event) {
			case CV_EVENT_LBUTTONDOWN:
				// mouse left button down 사각형 코너점 저장.
				_TEngineS->_ROIRect.x = x;
				_TEngineS->_ROIRect.y = y;
				_TEngineS->_ROIRect.width = 0;
				_TEngineS->_ROIRect.height = 0;
				_TDownCheck = true;
				_TEngineS->_RGB = cvScalar(255,0,0);
				break;
			case CV_EVENT_LBUTTONUP:
				// mouse left button up 마지막 마우스 이동 경로에 따른 관심영역 사각형 설정.
				if (_TEngineS->_ROIRect.width < 0) {
					_TEngineS->_ROIRect.x += _TEngineS->_ROIRect.width;
					_TEngineS->_ROIRect.width *= -1;
				}
				if (_TEngineS->_ROIRect.height < 0) {
					_TEngineS->_ROIRect.y += _TEngineS->_ROIRect.height;
					_TEngineS->_ROIRect.height *= -1;
				}
				_TEngineS->_ImageProcessMode++;

				_TDownCheck = false;
				break;
			case CV_EVENT_MOUSEMOVE:
				// mouse move 관심영역의 width & height 계산.
				if (_TDownCheck == true) {
					_TEngineS->_ROIRect.width = x - _TEngineS->_ROIRect.x;
					_TEngineS->_ROIRect.height = y - _TEngineS->_ROIRect.y;
				}
				break;
		}
	}
}

void EngineS::Inter_imageCraete(int roi_width, int roi_height){
	// 내부 연산에 사용되는 이미지 할당.
	_ImageSkin = cvCreateImage(cvSize(roi_width, roi_height), IPL_DEPTH_8U, 1);
	_PrevImage = cvCreateImage(cvSize(roi_width, roi_height), IPL_DEPTH_8U, 3);
	_ImageSub = cvCreateImage(cvSize(roi_width, roi_height), IPL_DEPTH_8U, 1);
}

void EngineS::Sub_image(IplImage *src1, IplImage *src2, IplImage *dst) {
	// src1, src2에 차영상 연산하여 dst의 저장.
	// dst zero이미지로 초기화.
	cvZero(dst);

	// 그림자 보정을 위한 Lab 색상계 변환.
	IplImage *Lab_src1 = cvCreateImage(cvGetSize(src1), IPL_DEPTH_8U, 3);
	IplImage *Lab_src2 = cvCreateImage(cvGetSize(src1), IPL_DEPTH_8U, 3);
	/*cvCvtColor(src1, Lab_src1, CV_BGR2Lab);
	cvCvtColor(src2, Lab_src2, CV_BGR2Lab);*/
	cvCopy(src1, Lab_src1); // 기존 RGB로 테스트.
	cvCopy(src2, Lab_src2);

	// 차영상 연산. 각 R,G,B값에 SUB_THRESHOLD를 적용하여 binary image 생성.
	for (register int i = 0; i < src1->width; i++) {
		for (register int j = 0; j < src1->height; j++) {
			//3채널에 대하여 각각 픽셀 값의 차이 절댓값을 연산
			unsigned char SUB_L = abs((unsigned char)Lab_src1->imageData[(i * 3) + (j * Lab_src1->widthStep)] - (unsigned char)Lab_src2->imageData[(i * 3) + (j * Lab_src2->widthStep)]);
			unsigned char SUB_a = abs((unsigned char)Lab_src1->imageData[(i * 3) + (j * Lab_src1->widthStep) + 1] - (unsigned char)Lab_src2->imageData[(i * 3) + (j * Lab_src2->widthStep) + 1]);
			unsigned char SUB_b = abs((unsigned char)Lab_src1->imageData[(i * 3) + (j * Lab_src1->widthStep) + 2] - (unsigned char)Lab_src2->imageData[(i * 3) + (j * Lab_src2->widthStep) + 2]);

			/*if ((SUB_L > SUB_THRESHOLD) && (SUB_a > SUB_THRESHOLD || SUB_b > SUB_THRESHOLD)) {
				dst->imageData[i + (j * dst->widthStep)] = (unsigned char)255;
			}*/
			if (SUB_L > SUB_THRESHOLD || SUB_a > SUB_THRESHOLD || SUB_b > SUB_THRESHOLD) {
				dst->imageData[i + (j * dst->widthStep)] = (unsigned char)255;
			}
		}
	}

	// 차영상 연산 결과에 median filter 적용 & 후추 소금 노이즈를 제거하기 위한 mopology 연산 적용.
	cvErode(dst, dst, 0, 2);
	cvDilate(dst, dst, 0, 2);

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

float EngineS::area_tri(CvPoint p, CvPoint q, CvPoint r) {
	// 2차원 좌표 p,q,r로 생성되는 삼각형의 넓이를 구함.
	return (float)abs(((p.x * q.y) + (q.x * r.y) + (r.x * p.y)) - ((p.y * q.x) + (q.y * r.x) + (r.y * p.x))) / 2.0;
}

bool EngineS::Check_InChessboard(IplImage *img, vector<ChessPoint> point){
	// Chess_point를 통하여 binary image의 픽셀이 chess board 내부에 존재하는지를 확인.
	// 1) 체스판 사각형의 크기를 연산하여 _TTriangleArea에 저장
	// 2) 차영상 결과 Detection된 픽셀의 좌표와 체스판 사각형의 끝점들로 이루어진 삼각형의 넓이를 각각 계산
	// 3) 2번 과정에서 연산된 삼각형 네개의 합이 체스판 사각형의 크기와 차이가 적을 경우 픽셀의 좌표가 사각형 내부에 있음으로 판정
	// 4) 사각형 내부의 픽셀 좌표가 있다는 판정이 되면 return true;

	CvPoint _T_LH, _T_LL, _T_RH, _T_RL; // 왼쪽 위, 왼쪽 아래, 오른쪽 위 오른쪽 아래.

	// tArea : chessboard 전체 넓이, 나머지 : 삼각형 넓이.
	float _TTriangleArea, _TTriangle1Area, _TTriangle2Area, _TTriangle3Area, _TTriangle4Area;
	
	// chessboard의 코너점을 구함.
	_T_LH = point.at(0).Cordinate;
	_T_RH = point.at(8).Cordinate;
	_T_LL = point.at(72).Cordinate;
	_T_RL = point.at(80).Cordinate;

	// 코너점을 이용하여 chessboard의 넓이 연산.
	_TTriangleArea = area_tri(_T_LH, _T_LL, _T_RH) + area_tri(_T_RH, _T_RL, _T_LL);

	// binary image에 존재하는 모든 픽셀을 대상으로 각 코너점과 이루는 삼각형의 넓이를 연산.
	for (register int i = 0; i < img->width; i++) {
		for (register int j = 0; j < img->height; j++) {
			UCHAR pixel_value = (UCHAR)img->imageData[i + j * img->widthStep];
			
			if (pixel_value == 255) {
				// Detection된 임의의 픽셀 좌표와 체스판의 코너점으로 이루어진 삼각형 네개의 넓이를 연산 
				_TTriangle1Area = area_tri(cvPoint(i,j), _T_LH, _T_RH);
				_TTriangle2Area = area_tri(cvPoint(i,j), _T_RH, _T_RL);
				_TTriangle3Area = area_tri(cvPoint(i,j), _T_RL, _T_LL);
				_TTriangle4Area = area_tri(cvPoint(i,j), _T_LL, _T_LH);

				float totalArea = _TTriangle1Area + _TTriangle2Area + _TTriangle3Area + _TTriangle4Area;

				// 각 삼각형의 합이 chessboard의 넓이와 같다면 return true; => 픽셀이 chessboard 내부에 존재함.
				if (fabs(_TTriangleArea - totalArea) < 2)
					return true;
			}
		}
	}

	// binary image의 픽셀이 chessboard 내부에 존재하지 않음
	return false;
}

bool EngineS::Check_imgZero(IplImage *img){
	// binary image에 픽셀값이 모드 0인지를 확인
	unsigned char _TPixelValue;

	for (register int i = 0; i < img->width; i++)
		for (register int j = 0; j < img->height; j++) {
			_TPixelValue = (unsigned char)img->imageData[i + (j * img->widthStep)];

			if (_TPixelValue != 0)
				return false;
		}

		return true;
}

CvPoint EngineS::Get_Chessidx(CvPoint point, vector<ChessPoint> cross_point){
	// chessboard의 교점의 index를 부여.
	// left-top이 원점인 좌표계로 index를 부여
	for (register int i = 0; i < cross_point.size() - 10; i++) {
		if (cross_point.at(i).Cordinate.x <= point.x && cross_point.at(i).Cordinate.y <= point.y) {
			if (cross_point.at(i + 10).Cordinate.x > point.x && cross_point.at(i + 10).Cordinate.y > point.y)
				return cross_point.at(i).index;
		}
	}
	return cvPoint(-1, -1);
}

bool EngineS::Start_Server() {
	bool _TIsStarted = false;

	if (_TelepathyServer->IsInitializeServer == true) {
		_TelepathyServer->TAnyConnentionNotifier = AnyConnentionNotifier;
		_TelepathyServer->TServerReceivedCallback = ServerReceivedCallback;
		if (_TelepathyServer->ServerStart()) {
			// Command Queue 생성.
			CommandQueue = new queue<ServerGetInformation *>();
			// Command 처리용 Thread를 생성.
			HANDLE _TThreadHandle = (HANDLE)_beginthreadex(NULL, 0, ServerCommandQueueProcessingThread, this, 0, NULL);

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

Telepathy::Server *EngineS::Get_Telepathy_Server() {
	return _TelepathyServer;
}

void EngineS::EngineS_Start() {
	// 1. Engine이 Start 되면, 우선 Server 기동부터 한다.
	Engine_Initializing();
	// 2. CVEC에서 "ServerIsAlive"가 날아오면, 
	Initialize_ImageProcessing();
	Start_Server();

	while (EngineEnable) {
		// Image Processing Engine이 시작되지 않았다면 대기한다.
		// 전체적인 구문에 영향을 줄 수 있으므로 다른 구문을 강구해보는 것도 괜찮은 생각이다.
		if (IsStarted != true) {
			// Image 처리 중이었다면, 모든 처리를 중단하고 화면에 Stop을 넣고 중단시킨다.
			// cvPutText(img_Cam, "Stop", cvPoint(30, 30), &cvFont(1.0), cvScalar(0, 100, 100));
			// while (!IsStarted) ;
		}

		// Chess Recognition 및 Hand Recognition을 처리하는 단계가 포함된 함수.
		Go_ImageProcessing();

		// 이 구분은 나중에 삭제 해야할 것 같다.
		// 회의 뒤 삭제.
		if (Check_Exit() == true)
			EngineEnable = false;
	}

	Deinitialize_ImageProcessing();
}

void EngineS::Calculate_Movement(IplImage *bin, vector<ChessPoint> cross_point, CvPoint *out1, CvPoint *out2){
	// 차영상의 결과 이미지를 이용하여 체스보드의 score를 부여.
	// score / 면적 을 이용하여 가장 많이 변한 두 좌표를 반환.
	float score_box[8][8]; // 면적비율 저장 배열.
	float chess_area[8][8]; // 체스 영역 저장 배열.
	const float score_threshold = 0.1; // 면적 비율 threshold.

	// 각 체스 영역 면적 계산부
	for(int i = 0; i < 8; i++){
		for(int j = 0; j < 8; j++){
			int cross_idx = i + (j * 9);
			CvPoint Head_point = cross_point.at(cross_idx).Cordinate;
			CvPoint Head_right = cross_point.at(cross_idx + 1).Cordinate;
			CvPoint Head_down = cross_point.at(cross_idx + 9).Cordinate;
			CvPoint right_down = cross_point.at(cross_idx + 10).Cordinate;
			score_box[i][j] = 0;
			chess_area[i][j] = area_tri(Head_point, Head_right, Head_down) + area_tri(Head_right, right_down, Head_down);
		}
	}

	// 차영상 면적 계산부.
	for (int i = 0; i < bin->width; i++){
		for (int j = 0; j < bin->height; j++){
			unsigned char pixel_value = bin->imageData[i + (j * bin->widthStep)];

			// 어느 좌표에 위치하는지 확인.
			if (pixel_value != 0){
				CvPoint chessbox_pos = Get_ChessboxPos(i, j, cross_point);
				if (chessbox_pos.x != -1 || chessbox_pos.y != -1)
					score_box[chessbox_pos.x][chessbox_pos.y]++;
			}
		}
	}

	// 스코어를 면적으로 나눠줘서 비율을 구함.
	// 가장 비율이 큰 두 좌표를 리턴.
	float temp_max1, temp_max2;
	CvPoint p_max1, p_max2;

	temp_max1 = temp_max2 = -1.0;
	p_max1 = p_max2 = cvPoint(-1, -1);
	for (int i = 0 ; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			score_box[i][j] /= chess_area[i][j];

			if (score_box[i][j] >= score_threshold) {
				if (temp_max1 <= score_box[i][j]) {
					temp_max2 = temp_max1;
					p_max2 = p_max1;
					temp_max1 = score_box[i][j];
					p_max1 = cvPoint(i, j);
				}
				else if (temp_max1 > score_box[i][j] && score_box[i][j] > temp_max2) {
					temp_max2 = score_box[i][j];
					p_max2 = cvPoint(i, j);
				}
			}
		}
	}

	*out1 = p_max1, *out2 = p_max2;
}

CvPoint EngineS::Get_ChessboxPos(int width, int height, vector<ChessPoint> cross_point) {
	// width, height가 가리키는 픽셀이 어느 체스보드 인덱스를 가지는지를 계산하여 반환.
	// 차영상 결과로 이진화된 이미지의 true 값을 가지는 픽셀의 좌표로 체스판의 인덱스 반환.
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			CvPoint top_left = cross_point.at((i * 9) + j).Cordinate;
			CvPoint bot_right = cross_point.at((i * 9) + j + 10).Cordinate;

			if (top_left.x <= width && top_left.y <= height)
				if (bot_right.x > width && bot_right.y > height)
					return cvPoint(j, i);
		}
	}

	//error return;
	return cvPoint(-1,-1);
}

void EngineS::DrawWindowS(IplImage *src, float fps, CvScalar RGB){
	// CVES 메인 윈도우에 이미지 UI 구성 및 Frame Per Second 표기
	const int LineLength = 30;	// 관심영역을 그릴 라인.
	const int ROI_Length = 440; // 정사각형 관심영역 크기.
	char _TBuffer[32];

	//CvPoint window_center = cvPoint(SERVER_VIEW_DEFAULT_WIDTH/2, SERVER_VIEW_DEFAULT_HEIGHT/2);

	CvPoint ROI_Lefttop = cvPoint(_ROIRect.x, _ROIRect.y);
	//
	CvPoint ROI_LeftBot = cvPoint(_ROIRect.x, _ROIRect.y + _ROIRect.width);
	//
	CvPoint ROI_Righttop = cvPoint(_ROIRect.x + _ROIRect.width, _ROIRect.y);
	CvPoint ROI_RightBot = cvPoint(_ROIRect.x + _ROIRect.width, _ROIRect.y + _ROIRect.width);

	// 관심영역 격자를 그림.
	cvDrawLine(src, cvPoint(ROI_Lefttop.x + LineLength, ROI_Lefttop.y), ROI_Lefttop, RGB, 4);
	cvDrawLine(src, cvPoint(ROI_Lefttop.x, ROI_Lefttop.y + LineLength), ROI_Lefttop, RGB, 4);

	cvDrawLine(src, cvPoint(ROI_RightBot.x - LineLength, ROI_RightBot.y), ROI_RightBot, RGB, 4);
	cvDrawLine(src, cvPoint(ROI_RightBot.x, ROI_RightBot.y - LineLength), ROI_RightBot, RGB, 4);

	// 추가.
	cvDrawLine(src, cvPoint(ROI_LeftBot.x + LineLength, ROI_LeftBot.y), ROI_LeftBot, RGB, 4);
	cvDrawLine(src, cvPoint(ROI_LeftBot.x, ROI_LeftBot.y - LineLength), ROI_LeftBot, RGB, 4);

	cvDrawLine(src, cvPoint(ROI_Righttop.x - LineLength, ROI_Righttop.y), ROI_Righttop, RGB, 4);
	cvDrawLine(src, cvPoint(ROI_Righttop.x, ROI_Righttop.y + LineLength), ROI_Righttop, RGB, 4);

	// Write Frame Per Sec.
	sprintf(_TBuffer, "%3.2f fps", fps);
	CvFont _TCvFont = cvFont(1.0);
	cvPutText(src, _TBuffer, cvPoint(30, 30), &_TCvFont, cvScalar(0, 0, 255));
}

void EngineS::imgproc_mode(){
	// 각 모드에 맞추어 image processing을 실행
	// mode 0 : 카메라 설정, UI Color 설정, 관심영역 크기 설정
	// mode 1 : Chessboard Recognition 확인부, 2초동안 프레임을 받아서 Chessboard recognition 수행
	// mode 2 : 체스말의 움직임 Detection & Chess UI draw, ChessGame 부분 구동
	static bool _TImageCreateCheck = false;
	static time_t _tempsec; // mode 1에서 시간을 체크할 변수

	if (_ImageProcessMode == 0) {
		// 관심영역 설정부.
		if (_TImageCreateCheck == true) {
			cvReleaseImage(&_ImageChess);
			_TImageCreateCheck = false;
		}
		cvShowImage("CVES", _CamOriginalImage);
		_ImageProcessMode++;
		_RGB = cvScalar(0, 0, 255);
		_tempsec = time(NULL);

		//관심영역 크기 고정
		_ROIRect = cvRect(100, 20, 440, 440);
	}
	else if (_ImageProcessMode == 1 /*&& IsStarted == true*/) {
		// 관심영역 재설정 선택 OR 체스보드 인식 확인부.
		int _TTick = GetTickCount();

		if (_TImageCreateCheck == false) {
			// 내부 연산에 사용되는 이미지 할당
			_ImageChess = cvCreateImage(cvSize(_ROIRect.width, _ROIRect.height), IPL_DEPTH_8U, 3);
			_TempPrev = cvCreateImage(cvSize(_ROIRect.width, _ROIRect.height), IPL_DEPTH_8U, 3);
			_TempPrev2 = cvCreateImage(cvSize(_ROIRect.width, _ROIRect.height), IPL_DEPTH_8U, 3);
			_PureImage = cvCreateImage(cvSize(_ROIRect.width, _ROIRect.height), IPL_DEPTH_8U, 3);
			_OtherBinaryImage = cvCreateImage(cvSize(_ROIRect.width, _ROIRect.height), IPL_DEPTH_8U, 1);
			_TImageCreateCheck = true;
			_ChessRecognition.Initialize_ChessRecognition(_ROIRect.width, _ROIRect.height, RECOGNITION_MODE);
			_HandRecognition.Init(_ROIRect.width, _ROIRect.height);
			
			// 연산에 필요한 이미지 할당.
			Inter_imageCraete(_ROIRect.width, _ROIRect.height);
		}
		cvSetImageROI(_CamOriginalImage, _ROIRect);
		cvCopy(_CamOriginalImage, _ImageChess);
		cvCopy(_CamOriginalImage, _PureImage);
		
		// Chessboard recognition.
		_ChessRecognition.Copy_Img(_ImageChess);
		_ChessRecognition.Chess_recog_wrapper(_CamOriginalImage, &_CrossPoint);
		cvResetImageROI(_CamOriginalImage);

		// 체스보드 인식이 확실하면 시간 카운트 시작, 교차점이 81개 이하면 다음 단계로 넘어가지 못함.
		if (_CrossPoint.size() < 81)
			_tempsec = time(NULL);

		// mode 1에서 2초 이상 지났을 경우 다음 모드로 진행
		if (time(NULL) - _tempsec > 2) {
			//_ImageProcessMode++;
			_RGB = cvScalar(0, 255);
		}

		// fps 계산.
		_TTick = GetTickCount() - _TTick;
		float _fps = 1000.f/ (float)_TTick;
		
		// CVES main Window에 UI 구성
		DrawWindowS(_CamOriginalImage, _fps, _RGB);
		cvShowImage("CVES", _CamOriginalImage);
	}
	else if (_ImageProcessMode == 2 /*&& IsStarted == true*/) {
		// 실제 이미지 처리 실행부.
		int _TTick = GetTickCount();

		// 관심 영역 설정
		cvSetImageROI(_CamOriginalImage, _ROIRect);
		cvCopy(_CamOriginalImage, _ImageChess); // 이미지 처리에 사용될 이미지 복사 - 관심영역 크기의 이미지
		cvCopy(_CamOriginalImage, _PureImage); // 원본 이미지 - 관심영역 크기의 이미지

		// 차영상 및 조건판정 부분.
		if (_CrossPoint.size() == 81) {
			if (_SubCheck == false) {
				// Chessboard recognition.
				_ChessRecognition.Copy_Img(_ImageChess);
				_ChessRecognition.Chess_recog_wrapper(_CamOriginalImage, &_CrossPoint);

				// 손이 들어오기 직전 영상을 촬영.
				_HandRecognition.Sub_prevFrame(_ImageChess, _ImageSkin, _BeforeHandFirst); // 턴별 차영상
				
				// 손 진입을 체크하는 bool 변수 초기화 - 이전에 손이 들어왔었다면 false로 수정
				if (_BeforeHandFirst)
					_BeforeHandFirst = false;

				if (Check_InChessboard(_ImageSkin, _CrossPoint)) {
					// 물체가 체스보드 위로 들어옴.
					cvCopy(_TempPrev2, _PrevImage);
#ifdef DEBUG_MODE
					cvShowImage("PREV", _PrevImage);
#endif
					_SubCheck = true;
				}
			}
			else {
				// 추후 해야할 작업 : 빠질때 어떻게 작업할 것인가.
				// 손이 들어옴 판정 이후 작업.
#ifdef DEBUG_MODE
				cvShowImage("유레카1", _ImageChess);
#endif
				// 오브젝트 디텍션에 사용되는 차영상 연산 수행
				Sub_image(_PrevImage, _ImageChess, _ImageSkin);
				// 차영상 결과를 이미지 처리에 사용되는 이미지로 색 부여
				Compose_diffImage(_ImageChess, _ImageSkin, cvScalar(0, 255, 255));

				// BlobLabeling
				_BlobLabeling.SetParam(_ImageSkin, 1);
				_BlobLabeling.DoLabeling();
				_BlobLabeling.DrawLabel(_ImageChess, cvScalar(255,0,255));

				// 손판정
				// 손 정의 - 차영상 결과 디텍션된 오브젝트.
				//          오브젝트 중 window 경계에 있는 물체
				_BlobLabeling.GetSideBlob(_ImageSkin, &_PieceIndex, _OtherBinaryImage); // 손이 아니라고 판정되는 오브젝트를 이진 영상에서 제거
				Compose_diffImage(_ImageChess, _ImageSkin, cvScalar(100, 100, 255)); // 손만 남은 이진 영상으로 원본 영상에 색을 부여

				// 이미지 처리에 사용되는 이미지에 Chessboard recognition 결과로 연산된 좌표를 표기
				_ChessRecognition.drawPoint(_ImageChess, _CrossPoint);
				cvDilate(_ImageSkin, _ImageSkin, 0, 5);
				cvShowImage("skin", _ImageSkin);

				// 체스보드 안으로 손이 들어왔는지를 확인
				if (Check_InChessboard(_ImageSkin, _CrossPoint)) {
					// img_Skin은 손 추정물체만 남긴 이미지.
					_InHandCheck = true;
				}
				else if (_InHandCheck == true) {
					// 차영상의 결과에 체스말의 이동경로 추적. 캐슬링, 앙파상 처리를 위하여 4개의 오브젝트를 디텍션
					CvPoint out[4];
					out[0] = out[1] = out[2] = out[3] = cvPoint(-1, -1);
					// 체스말의 움직임을 계산.
					Calculate_Movement(_OtherBinaryImage, _CrossPoint, &out[0], &out[1]);

					// 디텍션 된 결과가 두개 이상 존재한다면 실행
					if (out[0].x != -1 && out[1].x != -1) {
						// 이동 처리부.
						_InHandCheck = false;
						_SubCheck = false;
						_BeforeHandFirst = true;

						// chessgame 이동부.
						_ChessGame.Chess_process(out, 0);

						//텔레파시 콜백 호출
						char buf[32];
						_ChessGame.Get_RecentMove(buf);
						//_TelepathyServer->SendDataToOne(buf, );
#ifdef DEBUG_MODE
						// uci에 맞춰 return하는 부분 현재 printf로 출력
						//printf("%s\n", buf);
						_ChessGame.Show_chessImage();
#endif
					}

					//CVES process가 죽었을 경우를 대비하여 현재 경로들을 txt파일로 저장 & voting을 통하여 현재 말의 이동경로를 확정.
					//구현 예정.
				}
#ifdef DEBUG_MODE
				cvShowImage("compose_diff", _ImageChess);
#endif
			}
		}
		// 차영상에 이용하기 위한 2프레임 이전 영상의 저장.
		cvCopy(_TempPrev, _TempPrev2);
		cvCopy(_PureImage, _TempPrev);
		cvResetImageROI(_CamOriginalImage);

		// 설정된 관심영역 Rect 그리기.
		//cvDrawRect(_CamOriginalImage, cvPoint(_ROIRect.x, _ROIRect.y), cvPoint(_ROIRect.x + _ROIRect.width, _ROIRect.y + _ROIRect.height), _RGB, 2);

		// 초당 프레임수 계산.
		_TTick = GetTickCount() - _TTick;
		float _fps = 1000.f/ (float)_TTick;

		//Chessboard Main UI Drawing
		DrawWindowS(_CamOriginalImage, _fps, _RGB);

		cvShowImage("CVES", _CamOriginalImage);
	}

	if (cvWaitKey(33) == 27)
		_ImageProcessMode = 3;
}

void EngineS::ServerReceivedCallback(char *Buffer, SOCKET ClientSocket) {
	// using mutex.
	G_EngineS->_QueueProtectMutex.lock();
	ServerGetInformation *_TServerGetInformation = new ServerGetInformation;
	char _TBuffer[BUFFER_MAX_32767];
	memset(_TBuffer, NULL, sizeof(_TBuffer));
	strcpy(_TBuffer, Buffer);
	_TServerGetInformation->Infomations = _TBuffer;
	_TServerGetInformation->AnySocket = ClientSocket;
	G_EngineS->CommandQueue->push(_TServerGetInformation);
	G_EngineS->_QueueProtectMutex.unlock();
}

void EngineS::AnyConnentionNotifier(SOCKET ClientSocket) {
	// 초기 접속시에 Type을 물어보기 위한 용도로 쓰인다.
	G_EngineS->_TelepathyServer->SendDataToOne(STR_I_PTYPE, ClientSocket);
}

#if WINDOWS_SYS
UINT WINAPI
	//DWORD WINAPI
#elif POSIX_SYS
// using pthread
void *
#endif
	EngineS::ServerCommandQueueProcessingThread(
#if WINDOWS_SYS
	LPVOID
	//void *
#elif POSIX_SYS
	void *
#endif
	Param) {
	EngineS *_TEngine_S = (EngineS *)Param;

	while (_TEngine_S->_TelepathyServer->IsServerStarted) {
		Sleep(10);
		if (_TEngine_S->CommandQueue->empty() != true) {
			_TEngine_S->_QueueProtectMutex.lock();
			char _TStrBuffer[BUFFER_MAX_32767];
			ServerGetInformation *_TServerGetInformation;
			_TServerGetInformation =_TEngine_S->CommandQueue->front();

			memset(_TStrBuffer, NULL, sizeof(_TStrBuffer));
			strcpy(_TStrBuffer, _TServerGetInformation->Infomations);
			_TEngine_S->CommandQueue->pop();
			_TEngine_S->_QueueProtectMutex.unlock();

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
					_TEngine_S->_TelepathyServer->SendDataToOne(STR_I_ALIVE, _TServerGetInformation->AnySocket);
					break;
				case VALUE_I_IMWHITE :
					// Server에 Socket 중, White에 Naming을 할 Socket이 필요.
					// 어떤 Socket인지 검색하여 찾아 Naming 한다.
					for_IterToEnd(list, ClientsList, _TEngine_S->Get_Telepathy_Server()->ClientList) {
						if (_TVal->ClientSocket == _TServerGetInformation->AnySocket) {
							_TVal->ClientName = "White";
						}
					}
					
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
					break;
				case VALUE_I_ISSERVERREADY :
					// IsServerReady 질문을 하면 해야 할 것은 다음과 같다.
					// 1. 
					// Server(CVES)가 모든 준비 되었을 때, ServerisReady를 보낸다.
					_TEngine_S->_TelepathyServer->SendDataToOne("ServerisReady", _TServerGetInformation->AnySocket);
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
					_TEngine_S->IsStarted = false;
					break;
				case VALUE_I_TICTOKON :
					// Tictok is On.
					_TEngine_S->IsTictokEnable = true;
					break;
				case VALUE_I_TICTOKOFF :
					// Tictok is Off.
					_TEngine_S->IsTictokEnable = false;
					break;
			}

			delete _TInternalProtocolCS;
			delete _TStringTokenizer;
		}
	}
	return 0;
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
		StringTools _TStringTools;
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
				for_IterToEnd(list, ClientsList, _TelepathyServer->ClientList) {
					if (_TVal->ClientSocket == Socket) {
						char _TCharArr[10] = "White";
						_TVal->ClientName = _TCharArr;
					}
					else if (strcmp(_TVal->ClientType, "Client") == 0) {
						char _TCharArr[10] = "Black";
						_TVal->ClientName = _TCharArr;
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
							_TVal->ClientType = _TStringTools.ConstCharToChar(STR_I_INFO_TYPE_CLIENT);
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
							_TVal->ClientType = _TStringTools.ConstCharToChar(STR_I_INFO_TYPE_OBSERVER);
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
					}
					else if (_TIsWhiteTime) {
						_TIsWhiteTime = false;
						// White Time을 처리할 구문.
					}
					else if (_TIsTurn) {
						_TIsTurn = false;
						// Turn을 처리할 구문.
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

//void EngineS::Process_Info_Go(CommandString *IPCS) {
//	while(IPCS->NextCharArrayIter()) {
//		int _NSeek_GUIToEngine = _InternalProtocolSeeker.InternalProtocolString_Seeker((const char *)*IPCS->CharArrayListIter);
//
//		switch (_NSeek_GUIToEngine) {
//			case VALUE_I_INFO_GO :
//				Process_Info_Go(IPCS);
//				break;
//			case VALUE_I_INFO_POSITION :
//				Process_Info_Position(IPCS);
//				break;
//		}
//	}
//}
//
//void EngineS::Process_Info_Position(CommandString *IPCS) {
//	while(IPCS->NextCharArrayIter()) {
//		int _NSeek_GUIToEngine = _InternalProtocolSeeker.InternalProtocolString_Seeker((const char *)*IPCS->CharArrayListIter);
//
//		switch (_NSeek_GUIToEngine) {
//			case VALUE_I_INFO_GO :
//				Process_Info_Go(IPCS);
//				break;
//			case VALUE_I_INFO_POSITION :
//				Process_Info_Position(IPCS);
//				break;
//		}
//	}
//}

void EngineS::Set_ClientData( SOCKET Socket, int Type )
{

}
