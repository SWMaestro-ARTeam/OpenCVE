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
	_IsInitialized = false;
	_EndThread = false;
}

ChessRecognition::~ChessRecognition() {
	_IsInitialized = false;
	_EndThread = false;
}
#pragma endregion Constructor & Destructor

#pragma region Private Functions
// 결과만 반환.
void ChessRecognition::DrawPoints(IplImage *Source, vector<ChessPoint> Point) {
	if (_EnableThread != false) {
		// src image에 chessboard의 교점과 각 교점의 index를 표기.
		char buf[32];

		// display the points in an image 
		for (register int i = 0; i < Point.size(); i++) {
			cvCircle(Source, Point.at(i).Cordinate, 4, cvScalar(0, 255), 1);
			sprintf(buf, "(%d,%d)", Point.at(i).Index.x, Point.at(i).Index.y);
			CvFont _TCvFont = cvFont(1.0);
			cvPutText(Source, buf, Point.at(i).Cordinate, &_TCvFont, cvScalar(255, 0, 0));
		}
	}
	else {
		// Not Initialize.
		// Not Enabled Thread.
	}
}

// only hough Lines.
// 작동만 하면 결과만 반환.
void ChessRecognition::Get_Line(vector<pair<float, float> > *XLines, vector<pair<float, float> > *YLines) {
	if (_EnableThread != false) {
		if (_HoughLineBased != NULL) {
			// line fitting을 이용한 chessboard recognition의 경우 thread를 통하여 연산해낸 결과를 lineX와 LineY에 return;
			XLines->clear();
			YLines->clear();

			// Mutex를 통하여 thread 동기를 맞춤.
			_Vec_ProtectionMutex.lock();
			copy(_HoughLineBased->_Vector_LineX.begin(), _HoughLineBased->_Vector_LineX.end(), back_inserter(*XLines));
			copy(_HoughLineBased->_Vector_LineY.begin(), _HoughLineBased->_Vector_LineY.end(), back_inserter(*YLines));
			_Vec_ProtectionMutex.unlock();
			//thread로 부터 가져온 라인을 9개로 merge.
			_HoughLineBased->MergeLine(XLines);
			_HoughLineBased->MergeLine(YLines);
		}
		else {
			// Not Initialize.
			// Not Enabled Thread.
		}
	}
}

// 작동만 하면 결과만 반환.
void ChessRecognition::Refine_CrossPoint(vector<ChessPoint> *CrossPoint) {
	if (_EnableThread != false) {
		// chessboard recognition을 통하여 생성한 교점의 좌표를 보정하는 함수.
		// (alpha * 이전 좌표) - ((1 - alpha) * 현재 좌표)
		// 0 <= alpha <= 1.0

		static bool first_check = false;
		static vector<CvPoint> prev_point;
		const float Refine_const = 0.9; // alpha값.

		// 처음 chessboard recognition시 이전값이 존재하지 않으므로 초기화만 진행.
		if (first_check == false && CrossPoint->size() == 81) {
			for (register int i = 0; i < 81; i++)
				prev_point.push_back(CrossPoint->at(i).Cordinate);

			first_check = true;
		}
		// 처음이 아닐 경우 실제 좌표보정 연산 진행.
		else if (first_check == true) {
			// cross point가 81개가 되지 않으면 이전 81개를 반영.
			if (CrossPoint->size() < 81) {
				CrossPoint->clear();
				for (register int i = 0; i < 81; i++) {
					ChessPoint CP_temp;

					CP_temp.Cordinate = cvPoint(prev_point.at(i).x, prev_point.at(i).y); 
					CP_temp.Index = cvPoint(i % 9, i / 9);
					CrossPoint->push_back(CP_temp);
				}
			}
			// cross point가 81개라면 이전 좌표를 통한 보정 연산 진행.
			//else {
			//	for (register int i = 0; i < 81; i++) {
			//		// (alpha * 이전 좌표) - ((1 - alpha) * 현재 좌표)
			//		// 0 <= alpha <= 1.0
			//		point->at(i).Cordinate.x = cvRound((Refine_const * (float)prev_point.at(i).x) + ((1.0 - Refine_const) * (float)point->at(i).Cordinate.x));
			//		point->at(i).Cordinate.y = cvRound((Refine_const * (float)prev_point.at(i).y) + ((1.0 - Refine_const) * (float)point->at(i).Cordinate.y));

			//		prev_point.at(i) = cvPoint(point->at(i).Cordinate.x, point->at(i).Cordinate.y);
			//	}
			//}
		}
	}
	else {
		// Not Initialize.
		// Not Enabled Thread.
	}
}

#pragma region Threads
#if defined(WINDOWS_SYS)
UINT WINAPI
//DWORD WINAPI
#elif defined(POSIX_SYS)
// using pthread
void *
#endif
	ChessRecognition::HoughLineThread(
#if defined(WINDOWS_SYS)
	LPVOID
#elif defined(POSIX_SYS)
	void *
#endif
	Param) {
	// 실제로 뒤에서 동작하는 windows용 thread함수.
	// 함수 인자로 클래스를 받아옴.
	ChessRecognition *_TChessRecognition = (ChessRecognition *)Param;
	_TChessRecognition->_HoughLineBased = new HoughLineBased();

	CvSeq *_TLineX, *_TLineY;
	double _TH[] = { -1, -7, -15, 0, 15, 7, 1 };

	CvMat _TDoGX = cvMat(1, 7, CV_64FC1, _TH);
	CvMat* _TDoGY = cvCreateMat(7, 1, CV_64FC1);
	cvTranspose(&_TDoGX, _TDoGY); // transpose(&DoGx) -> DoGy

	double _TMinValX, _TMaxValX, _TMinValY, _TMaxValY, _TMinValT, _TMaxValT;
	int _TKernel = 1;

	// Hough 사용되는 Image에 대한 Initialize.
	IplImage *iplTemp = cvCreateImage(cvSize(_TChessRecognition->_Width, _TChessRecognition->_Height), IPL_DEPTH_32F, 1);                   
	IplImage *iplDoGx = cvCreateImage(cvGetSize(iplTemp), IPL_DEPTH_32F, 1);  
	IplImage *iplDoGy = cvCreateImage(cvGetSize(iplTemp), IPL_DEPTH_32F, 1);  
	IplImage *iplDoGyClone = cvCloneImage(iplDoGy);
	IplImage *iplDoGxClone = cvCloneImage(iplDoGx);
	IplImage *iplEdgeX = cvCreateImage(cvGetSize(iplTemp), 8, 1);
	IplImage *iplEdgeY = cvCreateImage(cvGetSize(iplTemp), 8, 1);

	CvMemStorage* _TStorageX = cvCreateMemStorage(0), *_TStorageY = cvCreateMemStorage(0);

	while (_TChessRecognition->_EnableThread != false) {
		// 이미지를 받아옴. main루프와 동기를 맞추기 위해서 critical section 사용.
		_TChessRecognition->_ChessBoardDetectionInternalImageProtectMutex.lock();
		//EnterCriticalSection(&(_TChessRecognition->cs));
		cvConvert(_TChessRecognition->_ChessBoardDetectionInternalImage, iplTemp);
		//LeaveCriticalSection(&_TChessRecognition->cs);
		_TChessRecognition->_ChessBoardDetectionInternalImageProtectMutex.unlock();

		// 각 X축 Y축 라인을 검출해 내기 위해서 filter 적용.
		cvFilter2D(iplTemp, iplDoGx, &_TDoGX); // 라인만 축출해내고.
		cvFilter2D(iplTemp, iplDoGy, _TDoGY);
		cvAbs(iplDoGx, iplDoGx);
		cvAbs(iplDoGy, iplDoGy);

		// 이미지 내부에서 최댓값과 최소값을 구하여 정규화.
		cvMinMaxLoc(iplDoGx, &_TMinValX, &_TMaxValX);
		cvMinMaxLoc(iplDoGy, &_TMinValY, &_TMaxValY);
		cvMinMaxLoc(iplTemp, &_TMinValT, &_TMaxValT);
		cvScale(iplDoGx, iplDoGx, 2.0 / _TMaxValX); // 정규화.
		cvScale(iplDoGy, iplDoGy, 2.0 / _TMaxValY);
		cvScale(iplTemp, iplTemp, 2.0 / _TMaxValT);

		cvCopy(iplDoGy, iplDoGyClone);
		cvCopy(iplDoGx, iplDoGxClone);

		// NMS진행후 추가 작업
		_TChessRecognition->_HoughLineBased->NonMaximumSuppression(iplDoGx, iplDoGyClone, _TKernel);
		_TChessRecognition->_HoughLineBased->NonMaximumSuppression(iplDoGy, iplDoGxClone, _TKernel);

		cvConvert(iplDoGx, iplEdgeY); // IPL_DEPTH_8U로 다시 재변환.
		cvConvert(iplDoGy, iplEdgeX);

		double rho = 1.0; // distance resolution in pixel-related units.
		double theta = 1.0; // angle resolution measured in radians.
		int threshold = 20;

		if (threshold == 0)
			threshold = 1;

		// detecting 해낸 edge에서 hough line 검출.
		_TLineX = cvHoughLines2(iplEdgeX, _TStorageX, CV_HOUGH_STANDARD, 1.0 * rho, CV_PI / 180 * theta, threshold, 0, 0);
		_TLineY = cvHoughLines2(iplEdgeY, _TStorageY, CV_HOUGH_STANDARD, 1.0 * rho, CV_PI / 180 * theta, threshold, 0, 0);

		// cvSeq를 vector로 바꾸기 위한 연산.
		_TChessRecognition->_Vec_ProtectionMutex.lock();
		//EnterCriticalSection(&_TChessRecognition->vec_cs);
		_TChessRecognition->_HoughLineBased->CastSequence(_TLineX, _TLineY);
		//LeaveCriticalSection(&_TChessRecognition->vec_cs);
		_TChessRecognition->_Vec_ProtectionMutex.unlock();

		Sleep(2);
	}

	// mat 할당 해제.
	cvReleaseMat(&_TDoGY);

	// 내부 연산에 사용된 이미지 할당 해제.
	cvReleaseImage(&iplTemp);
	cvReleaseImage(&iplDoGx);
	cvReleaseImage(&iplDoGy);
	cvReleaseImage(&iplDoGyClone);
	cvReleaseImage(&iplDoGxClone);
	cvReleaseImage(&iplEdgeX);
	cvReleaseImage(&iplEdgeY);

	// houghline2에 사용된 opencv 메모리 할당 해제.
	cvReleaseMemStorage(&_TStorageX);
	cvReleaseMemStorage(&_TStorageY);

	delete _TChessRecognition->_HoughLineBased;
#if defined(WINDOWS_SYS)
	_endthread();
#elif defined(POSIX_SYS)

#endif
	_TChessRecognition->_EndThread = true;
	return 0;
}

#if defined(WINDOWS_SYS)
UINT WINAPI
//DWORD WINAPI
#elif defined(POSIX_SYS)
// using pthread
void *
#endif
	ChessRecognition::LineSearchThread(
#if defined(WINDOWS_SYS)
	LPVOID
#elif defined(POSIX_SYS)
	void *
#endif
	Param) {
	// mode 2를 통하여 chessboard recognition을 수행하기 위한 thread 함수.
	ChessRecognition *_TChessRecognition = (ChessRecognition *)Param;
	_TChessRecognition->_LineSearchBased = new LineSearchBased();

	IplImage *_TGray = cvCreateImage(cvSize(_TChessRecognition->_Width, _TChessRecognition->_Height), IPL_DEPTH_8U, 1);

	while (_TChessRecognition->_EnableThread != false) {
		// 연산에 필요한 이미지를 main으로부터 복사해 옴.
		_TChessRecognition->_ChessBoardDetectionInternalImageProtectMutex.lock();
		//EnterCriticalSection(&(_TChessRecognition->cs));
		
		if (_TChessRecognition->_ChessBoardDetectionInternalImage->nChannels != 1)
			cvConvert(_TChessRecognition->_ChessBoardDetectionInternalImage, _TGray);
		else
			cvCopy(_TChessRecognition->_ChessBoardDetectionInternalImage, _TGray);

		//LeaveCriticalSection(&_TChessRecognition->cs);
		_TChessRecognition->_ChessBoardDetectionInternalImageProtectMutex.unlock();

		// 복사한 이미지를 실제 연산에 사용.
		_TChessRecognition->_ChessBoardDetectionInternalImageProtectMutex.lock();
		//EnterCriticalSection(&_TChessRecognition->vec_cs);
		_TChessRecognition->_LineSearchBased->ChessLineSearchProcess(_TGray, &_TChessRecognition->_CP);
		//LeaveCriticalSection(&_TChessRecognition->vec_cs);
		_TChessRecognition->_ChessBoardDetectionInternalImageProtectMutex.unlock();

		Sleep(2);
	}

	// 이미지 할당 해제.
	cvReleaseImage(&_TGray);
	//cvReleaseImage(&img_process);
	delete _TChessRecognition->_LineSearchBased;

#if defined(WINDOWS_SYS)
	_endthread();
#elif defined(POSIX_SYS)

#endif
	_TChessRecognition->_EndThread = true;
	return 0;
}
#pragma endregion Threads
#pragma endregion Private Functions

#pragma region Public Functions
void ChessRecognition::Initialize_ChessRecognition(int Width, int Height, int Mode) {
	// 관심영역의 width와 height를 입력받고, ChessRecogniton mode를 설정함. 현재 모드 1,2 제공
	//static bool _TFirstCheck = false;

	_EnableThread = true;

	//init이 처음 호출되었을 경우 thread 동기화를 위한 critical section 초기화
	/*if (_TFirstCheck == false) {
	_TFirstCheck = true;
	}
	else
	cvReleaseImage(&_ChessBoardDetectionInternalImage);*/

	// 내부 연산에 사용되는 이미지.
	_ChessBoardDetectionInternalImage = cvCreateImage(cvSize(Width, Height), IPL_DEPTH_8U, 1);
	cvZero(_ChessBoardDetectionInternalImage);

	_MODE = Mode;
	_Width = Width;
	_Height = Height;

	// mode에 따라 thread를 가동.
	// 1 - linefitting을 이용한 chessboard recognition.
	// 2 - x, y 선형 탐색을 이용한 chessboard recognition.
	if (Mode == 1) {
		_Thread.StartThread(HoughLineThread, this);
		/*
#if defined(WINDOWS_SYS)
		HANDLE _TThreadHandle = (HANDLE)_beginthreadex(NULL, 0, thread_hough, this, 0, NULL);
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
		pthread_create(&_TThread, NULL, thread_hough, (void *)this);
#endif
		*/
	}
	else if (Mode == 2) {
		_Thread.StartThread(LineSearchThread, this);
		/*
#if defined(WINDOWS_SYS)
		HANDLE _TThreadHandle = (HANDLE)_beginthreadex(NULL, 0, thread_ChessLineSearchAlg, this, 0, NULL);
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
		pthread_create(&_TThread, NULL, thread_ChessLineSearchAlg, (void *)this);
#endif
		*/
	}
}

void ChessRecognition::Deinitialize_ChessRecognition() {
	_EnableThread = false;
	// Thread 안전 종료.
	while (_EndThread != true) Sleep(10);
	// 이후 모든 자원 반환.
	cvZero(_ChessBoardDetectionInternalImage);
	cvReleaseImage(&_ChessBoardDetectionInternalImage);
}

void ChessRecognition::Copy_Img(IplImage *Source) {
	// main의 cam으로부터 받아온 이미지를 thread의 연산에 사용하기 위해서 복사해오는 함수.
	// 연산에 사용되는 이미지는 Grayscale이기 때문에 색상 변환 연산 진행.
	if (_EnableThread != false) {
		_ChessBoardDetectionInternalImageProtectMutex.lock();
		if (Source->nChannels == 1) {
			cvCopy(Source, _ChessBoardDetectionInternalImage);
		}
		else {
			cvCvtColor(Source, _ChessBoardDetectionInternalImage, CV_BGR2GRAY);
		}
		_ChessBoardDetectionInternalImageProtectMutex.unlock();
	}
	else {
		// Not Initialize.
		// Not Enabled Thread.
	}
}

void ChessRecognition::Find_ChessPoint(IplImage *Source, vector<ChessPoint> *Point) {
	if (_EnableThread != false) {
		// init함수를 통하여 설정한 mode에 맞추어 chessboard recognition & 좌표 보정 & src에 좌표 그리기 진행.
		// src : 좌표를 그릴 이미지, point : 연산을 통하여 cross point를 저장할 vector.
		Point->clear();

		if (_MODE == 1) {
			if (_HoughLineBased != NULL) {
				vector<std::pair<float, float> > _TCH_LineX, _TCH_LineY;

				Get_Line(&_TCH_LineX, &_TCH_LineY);
#if !defined(USING_QT)
				_HoughLineBased->DrawLines(_TCH_LineX, Source);
				_HoughLineBased->DrawLines(_TCH_LineY, Source);
#endif
				_HoughLineBased->FindIntersections(_TCH_LineX, _TCH_LineY, Point);
				Refine_CrossPoint(Point);
#if !defined(USING_QT)
				DrawPoints(Source, *Point);
#endif
			}
			else {
				// Not Initialize.
			}
		}
		else if (_MODE == 2) {
			if (_LineSearchBased != NULL) {
				_Vec_ProtectionMutex.lock();
				copy(_CP.begin(), _CP.end(), back_inserter(*Point));
				_Vec_ProtectionMutex.unlock();
				// 보정으로 인한 문제가 발생하여 잠시 보류.
				//Refine_CrossPoint(point);
#if !defined(USING_QT)
				DrawPoints(Source, *Point);
#endif
			}
			else {
				// Not Initialize.
			}
		}
	}
}
#pragma endregion Public Functions
