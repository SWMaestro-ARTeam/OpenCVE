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

#include "ChessRecognition.hpp"

#pragma region Constructor & Destructor
ChessRecognition::ChessRecognition() {
	//_LSB = new LineSearchBased();
}

ChessRecognition::~ChessRecognition() {
	//exit();
}
#pragma endregion Constructor & Destructor

#pragma region Private Functions
void ChessRecognition::DrawPoints(IplImage *Source, vector<ChessPoint> Point) {
	// src image에 chessboard의 교점과 각 교점의 index를 표기.
	char buf[32];

	// display the points in an image 
	for (register int i = 0; i < Point.size(); i++) {
		cvCircle(Source, Point.at(i).Cordinate, 4, cvScalar(0, 255), 1);
		sprintf(buf, "(%d,%d)", Point.at(i).index.x, Point.at(i).index.y);
		CvFont _TCvFont = cvFont(1.0);
		cvPutText(Source, buf, Point.at(i).Cordinate, &_TCvFont, cvScalar(255, 0, 0));
	}
}

void ChessRecognition::Get_Line(vector<pair<float, float> > *XLines, vector<pair<float, float> > *YLines) {
	// line fitting을 이용한 chessboard recognition의 경우 thread를 통하여 연산해낸 결과를 lineX와 LineY에 return;
	XLines->clear();
	YLines->clear();

	// Mutex를 통하여 thread 동기를 맞춤.
	_Vec_CSProtectionMutex.lock();
	copy(_HoughLineBased._Vector_LineX.begin(), _HoughLineBased._Vector_LineX.end(), back_inserter(*XLines));
	copy(_HoughLineBased._Vector_LineY.begin(), _HoughLineBased._Vector_LineY.end(), back_inserter(*YLines));
	_Vec_CSProtectionMutex.unlock();

	//thread로 부터 가져온 라인을 9개로 merge.
	_HoughLineBased.MergeLine(XLines);
	_HoughLineBased.MergeLine(YLines);
}

void ChessRecognition::Refine_CrossPoint(vector<ChessPoint> *point) {
	// chessboard recognition을 통하여 생성한 교점의 좌표를 보정하는 함수.
	// (alpha * 이전 좌표) - ((1 - alpha) * 현재 좌표)
	// 0 <= alpha <= 1.0

	static bool first_check = false;
	static vector<CvPoint> prev_point;
	const float Refine_const = 0.9; // alpha값.

	// 처음 chessboard recognition시 이전값이 존재하지 않으므로 초기화만 진행.
	if (first_check == false && point->size() == 81) {
		for (register int i = 0; i < 81; i++)
			prev_point.push_back(point->at(i).Cordinate);

		first_check = true;
	}
	// 처음이 아닐 경우 실제 좌표보정 연산 진행.
	else if (first_check == true) {
		// cross point가 81개가 되지 않으면 이전 81개를 반영.
		if (point->size() < 81) {
			point->clear();
			for (register int i = 0; i < 81; i++) {
				ChessPoint CP_temp;

				CP_temp.Cordinate = cvPoint(prev_point.at(i).x, prev_point.at(i).y); 
				CP_temp.index = cvPoint(i % 9, i / 9);
				point->push_back(CP_temp);
			}
		}
		// cross point가 81개라면 이전 좌표를 통한 보정 연산 진행.
		else {
			for (register int i = 0; i < 81; i++) {
				// (alpha * 이전 좌표) - ((1 - alpha) * 현재 좌표)
				// 0 <= alpha <= 1.0
				point->at(i).Cordinate.x = cvRound((Refine_const * (float)prev_point.at(i).x) + ((1.0 - Refine_const) * (float)point->at(i).Cordinate.x));
				point->at(i).Cordinate.y = cvRound((Refine_const * (float)prev_point.at(i).y) + ((1.0 - Refine_const) * (float)point->at(i).Cordinate.y));

				prev_point.at(i) = cvPoint(point->at(i).Cordinate.x, point->at(i).Cordinate.y);
			}
		}
	}
}

#pragma region Threads
UINT WINAPI ChessRecognition::thread_hough(void *arg) {
	// 실제로 뒤에서 동작하는 windows용 thread함수.
	// 함수 인자로 클래스를 받아옴.
	ChessRecognition *_TChessRecognition = (ChessRecognition *)arg;
	CvSeq *LineX, *LineY;
	double h[] = { -1, -7, -15, 0, 15, 7, 1 };

	CvMat DoGx = cvMat(1, 7, CV_64FC1, h);
	CvMat* DoGy = cvCreateMat(7, 1, CV_64FC1);
	cvTranspose(&DoGx, DoGy); // transpose(&DoGx) -> DoGy

	double minValx, maxValx, minValy, maxValy, minValt, maxValt;
	int kernel = 1;

	IplImage *iplTemp = cvCreateImage(cvSize(_TChessRecognition->_Width, _TChessRecognition->_Height), IPL_DEPTH_32F, 1);                   
	IplImage *iplDoGx = cvCreateImage(cvGetSize(iplTemp), IPL_DEPTH_32F, 1);  
	IplImage *iplDoGy = cvCreateImage(cvGetSize(iplTemp), IPL_DEPTH_32F, 1);  
	IplImage *iplDoGyClone = cvCloneImage(iplDoGy);
	IplImage *iplDoGxClone = cvCloneImage(iplDoGx);
	IplImage *iplEdgeX = cvCreateImage(cvGetSize(iplTemp), 8, 1);
	IplImage *iplEdgeY = cvCreateImage(cvGetSize(iplTemp), 8, 1);

	CvMemStorage* storageX = cvCreateMemStorage(0), *storageY = cvCreateMemStorage(0);

	while (1) {
		// 이미지를 받아옴. main루프와 동기를 맞추기 위해서 critical section 사용.
		_TChessRecognition->_CSProtectionMutex.lock();
		//EnterCriticalSection(&(_TChessRecognition->cs));
		cvConvert(_TChessRecognition->img_process, iplTemp);
		//LeaveCriticalSection(&_TChessRecognition->cs);
		_TChessRecognition->_CSProtectionMutex.unlock();

		// 각 X축 Y축 라인을 검출해 내기 위해서 filter 적용.
		cvFilter2D(iplTemp, iplDoGx, &DoGx); // 라인만 축출해내고.
		cvFilter2D(iplTemp, iplDoGy, DoGy);
		cvAbs(iplDoGx, iplDoGx);
		cvAbs(iplDoGy, iplDoGy);

		// 이미지 내부에서 최댓값과 최소값을 구하여 정규화.
		cvMinMaxLoc(iplDoGx, &minValx, &maxValx);
		cvMinMaxLoc(iplDoGy, &minValy, &maxValy);
		cvMinMaxLoc(iplTemp, &minValt, &maxValt);
		cvScale(iplDoGx, iplDoGx, 2.0 / maxValx); // 정규화.
		cvScale(iplDoGy, iplDoGy, 2.0 / maxValy);
		cvScale(iplTemp, iplTemp, 2.0 / maxValt);

		cvCopy(iplDoGy, iplDoGyClone);
		cvCopy(iplDoGx, iplDoGxClone);

		// NMS진행후 추가 작업
		_TChessRecognition->_HoughLineBased.NonMaximumSuppression(iplDoGx, iplDoGyClone, kernel);
		_TChessRecognition->_HoughLineBased.NonMaximumSuppression(iplDoGy, iplDoGxClone, kernel);

		cvConvert(iplDoGx, iplEdgeY); // IPL_DEPTH_8U로 다시 재변환.
		cvConvert(iplDoGy, iplEdgeX);

		double rho = 1.0; // distance resolution in pixel-related units.
		double theta = 1.0; // angle resolution measured in radians.
		int threshold = 20;

		if (threshold == 0)
			threshold = 1;

		// detecting 해낸 edge에서 hough line 검출.
		LineX = cvHoughLines2(iplEdgeX, storageX, CV_HOUGH_STANDARD, 1.0 * rho, CV_PI / 180 * theta, threshold, 0, 0);
		LineY = cvHoughLines2(iplEdgeY, storageY, CV_HOUGH_STANDARD, 1.0 * rho, CV_PI / 180 * theta, threshold, 0, 0);

		// cvSeq를 vector로 바꾸기 위한 연산.
		_TChessRecognition->_Vec_CSProtectionMutex.lock();
		//EnterCriticalSection(&_TChessRecognition->vec_cs);
		_TChessRecognition->_HoughLineBased.CastSequence(LineX, LineY);
		//LeaveCriticalSection(&_TChessRecognition->vec_cs);
		_TChessRecognition->_Vec_CSProtectionMutex.unlock();

		Sleep(2);

		if (_TChessRecognition->thread_exit == true)
			break;
	}

	// mat 할당 해제.
	cvReleaseMat(&DoGy);

	// 내부 연산에 사용된 이미지 할당 해제.
	cvReleaseImage(&iplTemp);
	cvReleaseImage(&iplDoGx);
	cvReleaseImage(&iplDoGy);
	cvReleaseImage(&iplDoGyClone);
	cvReleaseImage(&iplDoGxClone);
	cvReleaseImage(&iplEdgeX);
	cvReleaseImage(&iplEdgeY);

	// houghline2에 사용된 opencv 메모리 할당 해제.
	cvReleaseMemStorage(&storageX);
	cvReleaseMemStorage(&storageY);

	_endthread();

	return 0;
}

UINT WINAPI ChessRecognition::thread_ChessLineSearchAlg(void *arg) {
	// mode 2를 통하여 chessboard recognition을 수행하기 위한 thread 함수.
	ChessRecognition *_TChessRecognition = (ChessRecognition *)arg;
	IplImage *_TGray = cvCreateImage(cvSize(_TChessRecognition->_Width, _TChessRecognition->_Height), IPL_DEPTH_8U, 1);

	while (1) {
		// 연산에 필요한 이미지를 main으로부터 복사해 옴.
		_TChessRecognition->_CSProtectionMutex.lock();
		//EnterCriticalSection(&(_TChessRecognition->cs));
		
		if (_TChessRecognition->img_process->nChannels != 1)
			cvConvert(_TChessRecognition->img_process, _TGray);
		else
			cvCopy(_TChessRecognition->img_process, _TGray);

		//LeaveCriticalSection(&_TChessRecognition->cs);
		_TChessRecognition->_CSProtectionMutex.unlock();

		// 복사한 이미지를 실제 연산에 사용.
		_TChessRecognition->_CSProtectionMutex.lock();
		//EnterCriticalSection(&_TChessRecognition->vec_cs);
		_TChessRecognition->_LineSearchBased.ChessLineSearchProcess(_TGray, &_TChessRecognition->_CP);
		//LeaveCriticalSection(&_TChessRecognition->vec_cs);
		_TChessRecognition->_CSProtectionMutex.unlock();

		// Sleep 10은 왜?
		Sleep(2);

		if (_TChessRecognition->thread_exit == true)
			break;
	}

	_endthread();

	// 이미지 할당 해제.
	cvReleaseImage(&_TGray);
	//cvReleaseImage(&img_process);
	return 0;
}
#pragma endregion Threads
#pragma endregion Private Functions

#pragma region Public Functions
void ChessRecognition::Initialize_ChessRecognition(int Width, int Height, int Mode) {
	// 관심영역의 width와 height를 입력받고, ChessRecogniton mode를 설정함. 현재 모드 1,2 제공
	static bool _TFirstCheck = false;

	thread_exit = false;

	//init이 처음 호출되었을 경우 thread 동기화를 위한 critical section 초기화
	if (_TFirstCheck == false) {
		_TFirstCheck = true;
	}
	else
		cvReleaseImage(&img_process);

	// mode에 따라 thread를 가동.
	// 1 - linefitting을 이용한 chessboard recognition.
	// 2 - x, y 선형 탐색을 이용한 chessboard recognition.
	if (Mode == 1) {
		hThread = (HANDLE)_beginthreadex(NULL, 0, thread_hough, this, 0, NULL);
	}
	else if (Mode == 2) {
		hThread = (HANDLE)_beginthreadex(NULL, 0, thread_ChessLineSearchAlg, this, 0, NULL);
	}

	//내부 연산에 사용되는 이미지
	img_process = cvCreateImage(cvSize(Width, Height), IPL_DEPTH_8U, 1);
	cvZero(img_process);

	_MODE = Mode;
	_Width = Width;
	_Height = Height;
}


void ChessRecognition::Copy_Img(IplImage *src) {
	// main의 cam으로부터 받아온 이미지를 thread의 연산에 사용하기 위해서 복사해오는 함수.
	// 연산에 사용되는 이미지는 Grayscale이기 때문에 색상 변환 연산 진행.
	_CSProtectionMutex.lock();
	//EnterCriticalSection(&cs);
	if (src->nChannels == 1) {
		cvCopy(src, img_process);
	}
	else {
		cvCvtColor(src, img_process, CV_BGR2GRAY);
	}
	//LeaveCriticalSection(&cs);
	_CSProtectionMutex.unlock();
}

void ChessRecognition::exit() {
	thread_exit = true;

	CloseHandle(hThread);
	cvReleaseImage(&img_process);
}

void ChessRecognition::Find_ChessPoint(IplImage *Source, vector<ChessPoint> *Point) {
	// init함수를 통하여 설정한 mode에 맞추어 chessboard recognition & 좌표 보정 & src에 좌표 그리기 진행.
	// src : 좌표를 그릴 이미지, point : 연산을 통하여 cross point를 저장할 vector.
	vector<std::pair<float, float> > CH_LineX, CH_LineY;
	Point->clear();

	if (_MODE == 1) {
		Get_Line(&CH_LineX, &CH_LineY);
		_HoughLineBased.DrawLines(CH_LineX, Source);
		_HoughLineBased.DrawLines(CH_LineY, Source);
		_HoughLineBased.FindIntersections(CH_LineX, CH_LineY, Point);
		Refine_CrossPoint(Point);
		DrawPoints(Source, *Point);
	}
	else if (_MODE == 2) {
		_Vec_CSProtectionMutex.lock();
		copy(_CP.begin(), _CP.end(), back_inserter(*Point));
		_Vec_CSProtectionMutex.unlock();
		//Refine_CrossPoint(point);
		DrawPoints(Source, *Point);
	}
}
#pragma endregion Public Functions