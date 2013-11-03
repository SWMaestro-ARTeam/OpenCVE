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

	if (_TelepathyServer->IsInitializeServer == true)
		_TelepathyServer->ServerStart();
}

void EngineS::Engine_DeInitializing() {
	Deinitialize_TServer();
}

bool EngineS::Check_Exit() {
	if (_ImageProcessMode == 3)
		return true;
	else
		return false;
}

void EngineS::Go_ImageProcessing(){
	//char _TKeyWait, _TBuffer[32];
	//static bool _TImageCreateCheck = false;

	// Cam으로부터의 영상입력.
	_CamOriginalImage = cvQueryFrame(_Cam);
	cvFlip(_CamOriginalImage, _CamOriginalImage, FLIP_MODE);

	//모드에 따른 이미지 프로세스 수행
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
			unsigned char SUB_L = abs((unsigned char)Lab_src1->imageData[(i * 3) + (j * Lab_src1->widthStep)] - (unsigned char)Lab_src2->imageData[(i * 3) + (j * Lab_src2->widthStep)]);
			unsigned char SUB_a = abs((unsigned char)Lab_src1->imageData[(i * 3) + (j * Lab_src1->widthStep) + 1] - (unsigned char)Lab_src2->imageData[(i * 3) + (j * Lab_src2->widthStep) + 1]);
			unsigned char SUB_b = abs((unsigned char)Lab_src1->imageData[(i * 3) + (j * Lab_src1->widthStep) + 2] - (unsigned char)Lab_src2->imageData[(i * 3) + (j * Lab_src2->widthStep) + 2]);

			/*if ((SUB_L > SUB_THRESHOLD) && (SUB_a > SUB_THRESHOLD || SUB_b > SUB_THRESHOLD)) {
				dst->imageData[i + (j * dst->widthStep)] = (unsigned char)255;
			}*/
			if (SUB_L > SUB_THRESHOLD || SUB_a > SUB_THRESHOLD || SUB_b > SUB_THRESHOLD) {
				dst->imageData[i + (j * dst->widthStep)] = (unsigned char)/*(SUB_L + SUB_a + SUB_b)/3*/255;
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
	//2차원 좌표 p,q,r로 생성되는 삼각형의 넓이를 구함.
	return (float)abs(((p.x * q.y) + (q.x * r.y) + (r.x * p.y)) - ((p.y * q.x) + (q.y * r.x) + (r.y * p.x))) / 2.0;
}

bool EngineS::Check_InChessboard(IplImage *img, vector<ChessPoint> point){
	// Chess_point를 통하여 binary image의 픽셀이 chess board 내부에 존재하는지를 확인.
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
		_TelepathyServer->TServerReceivedCallback = ServerReceivedCallback;
		_TelepathyServer->ServerStart();
		_TIsStarted = true;
	}
	return _TIsStarted;
}

void EngineS::Stop_Server() {

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

void ServerReceivedCallback(char *Buffer, SOCKET ClientSocket) {
	// 내부 Protocol 송신(CVES -> CVEC, CVES -> Observer).
	StringTokenizer *_TStringTokenizer = new StringTokenizer();
	InternalProtocolSeeker _TInternalProtocolSeeker;

	_TStringTokenizer->SetInputCharString((const char *)Buffer);
	_TStringTokenizer->SetSingleToken(" ");
	if (_TStringTokenizer->StringTokenGo() == false)
		return ;

	CommandString *_TInternalProtocolCS = new CommandString(_TStringTokenizer->GetTokenedCharListArrays());

	int _TSeek_AnyToCVES = _TInternalProtocolSeeker.InternalProtocolString_Seeker((const char *)*_TInternalProtocolCS->CharArrayListIter);

	switch (_TSeek_AnyToCVES) {
		// CVEC -> CVES
		case VALUE_I_SERVERKILL :
			// Server를 죽인다.
			// 이유를 불문하고 이 명령이 들어오면 바로 죽인다.
			G_EngineS->EngineEnable = false;
			break;
		case VALUE_I_SERVERISALIVE :
			// CVEC가 Server가 살아있냐는 질문에 2가지 응답으로 답해야 한다.
			// 그런데 실제로, Server가 'Busy' 한다는건 '살아있냐'라는 질의에 대한 응답으로 좀 맞지 않으므로,
			// 'Alive'만 날려준다.
			G_EngineS->Get_Telepathy_Server()->SendDataToOne("Alive", ClientSocket);
			break;
		case VALUE_I_IMFIRST :
			// Server에 Socket 중, White에 Naming을 할 Socket이 필요.
			// 어떤 Socket인지 검색하여 찾아 Naming 한다.
			/*
			for_IterToBegin(list, ClientsList, G_EngineS->Get_Telepathy_Server()->ClientList) {
				if (_TVal->ClientSocket == ClientSocket) {
					_TVal->ClientName = "White";
				}
			}
			*/
			break;
		case VALUE_I_STOP :
			// Stop the Server Image Processing.
			G_EngineS->IsStarted = false;
			break;
		case VALUE_I_START :
			// Start the Server Image Processing.
			G_EngineS->IsStarted = true;
			break;
		case VALUE_I_ISRESTOREPOSSIBLE :
			// 복구가 가능한가?
			// 만약 이 Message가 왔다면, 이미 한번 죽어서 다시 실행되었다는 뜻이므로, 이전 Chess판의
			// 정보가 있는지 없는지를 검사해야 한다.
			// 만약 Data가 없거나, Data의 Checksum이 맞지 않아 복원에 실패하였다면, 'No'.
			// 그게 아니라면 'Yes'를 날려준다.
			break;

		// Observer -> CVES
		case VALUE_I_STATUSNOW :
			// Status를 요청했으므로, 정보를 날려준다.
			G_EngineS->IsStarted = false;
			break;
		case VALUE_I_TICTOKON :
			// Tictok is On.
			G_EngineS->IsTictokEnable = true;
			break;
		case VALUE_I_TICTOKOFF :
			// Tictok is Off.
			G_EngineS->IsTictokEnable = false;
			break;
	}

	delete _TInternalProtocolCS;
	delete _TStringTokenizer;
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
	const int LineLength = 30;	//관심영역을 그릴 라인.
	const int ROI_Length = 440; //정사각형 관심영역 크기.
	char _TBuffer[32];

	//CvPoint window_center = cvPoint(SERVER_VIEW_DEFAULT_WIDTH/2, SERVER_VIEW_DEFAULT_HEIGHT/2);

	CvPoint ROI_Lefttop =cvPoint(_ROIRect.x, _ROIRect.y);
	CvPoint ROI_RightBot = cvPoint(_ROIRect.x + _ROIRect.width, _ROIRect.y + _ROIRect.width);

	// 관심영역 격자를 그림.
	cvDrawLine(src, cvPoint(ROI_Lefttop.x + LineLength, ROI_Lefttop.y),  ROI_Lefttop, RGB, 4);
	cvDrawLine(src, cvPoint(ROI_Lefttop.x, ROI_Lefttop.y + LineLength),  ROI_Lefttop, RGB, 4);
	cvDrawLine(src, cvPoint(ROI_RightBot.x - LineLength, ROI_RightBot.y),  ROI_RightBot, RGB, 4);
	cvDrawLine(src, cvPoint(ROI_RightBot.x, ROI_RightBot.y - LineLength),  ROI_RightBot, RGB, 4);

	// Write Frame Per Sec.
	sprintf(_TBuffer, "%.2f fps", fps);
	CvFont _TCvFont = cvFont(1.0);
	cvPutText(src, _TBuffer, cvPoint(30, 30), &_TCvFont, cvScalar(0, 0, 255));
	//cvPutText(src, _TBuffer, cvPoint(30, 30), &cvFont(1.0), cvScalar(0, 0, 255));
}

void EngineS::imgproc_mode(){
	//char _TBuffer[32];
	static bool _TImageCreateCheck = false;
	static time_t _tempsec;

	if (_ImageProcessMode == 0) {
		// 관심영역 설정부.
		if (_TImageCreateCheck == true) {
			cvReleaseImage(&_ImageChess);
			_TImageCreateCheck = false;
		}
		//cvDrawRect(_CamOriginalImage, cvPoint(_ROIRect.x, _ROIRect.y), cvPoint(_ROIRect.x + _ROIRect.width, _ROIRect.y + _ROIRect.height), _RGB, 2);
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

		if (time(NULL) - _tempsec > 2) {
			_ImageProcessMode++;
			_RGB = cvScalar(0, 255);
		}

		// fps 계산.
		_TTick = GetTickCount() - _TTick;
		float _fps = 1000.f/ (float)_TTick;
		
		DrawWindowS(_CamOriginalImage, _fps, _RGB);
		cvShowImage("CVES", _CamOriginalImage);
	}
	else if (_ImageProcessMode == 2 /*&& IsStarted == true*/) {
		// 실제 이미지 처리 실행부.
		int _TTick = GetTickCount();

		cvSetImageROI(_CamOriginalImage, _ROIRect);
		cvCopy(_CamOriginalImage, _ImageChess);
		cvCopy(_CamOriginalImage, _PureImage);

		// 차영상 및 조건판정 부분.
		if (_CrossPoint.size() == 81) {
			if (_SubCheck == false) {
				// Chessboard recognition.
				_ChessRecognition.Copy_Img(_ImageChess);
				_ChessRecognition.Chess_recog_wrapper(_CamOriginalImage, &_CrossPoint);

				// 손이 들어오기 직전 영상을 촬영.
				_HandRecognition.Sub_prevFrame(_ImageChess, _ImageSkin, _BeforeHandFirst); // 실시간 차영상 -> 턴별 차영상
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
				Sub_image(_PrevImage, _ImageChess, _ImageSkin);
				Compose_diffImage(_ImageChess, _ImageSkin, cvScalar(0, 255, 255));

				// BlobLabeling
				_BlobLabeling.SetParam(_ImageSkin, 1);
				_BlobLabeling.DoLabeling();
				_BlobLabeling.DrawLabel(_ImageChess, cvScalar(255,0,255));

				// 손판정
				_BlobLabeling.GetSideBlob(_ImageSkin, &_PieceIndex, _OtherBinaryImage);
				Compose_diffImage(_ImageChess, _ImageSkin, cvScalar(100, 100, 255));

				_ChessRecognition.drawPoint(_ImageChess, _CrossPoint);
				cvDilate(_ImageSkin, _ImageSkin, 0, 5);
				cvShowImage("skin", _ImageSkin);

				if (Check_InChessboard(_ImageSkin, _CrossPoint)) {
					// img_Skin은 손 추정물체만 남긴 이미지.
					_InHandCheck = true;
				}
				else if (_InHandCheck == true) {
					// 차영상의 결과에 체스말의 이동경로 추적.
					CvPoint out[4];
					out[0] = out[1] = out[2] = out[3] = cvPoint(-1, -1);
					Calculate_Movement(_OtherBinaryImage, _CrossPoint, &out[0], &out[1]);

					// 결과가 
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
						printf("%s\n", buf);
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

		DrawWindowS(_CamOriginalImage, _fps, _RGB);

		cvShowImage("CVES", _CamOriginalImage);
	}

	if (cvWaitKey(33) == 27)
		_ImageProcessMode++;
}
