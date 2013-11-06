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

ChessRecognition::ChessRecognition() {
	_CLSA = new ChessLineSearchAlg();
}

ChessRecognition::~ChessRecognition() {
	CloseHandle(hThread);
	DeleteCriticalSection(&cs);
	DeleteCriticalSection(&vec_cs);
	cvReleaseImage(&img_process);
}

void ChessRecognition::exit() {
	thread_exit = true;

	CloseHandle(hThread);
	DeleteCriticalSection(&cs);
	DeleteCriticalSection(&vec_cs);
	cvReleaseImage(&img_process);
}

void ChessRecognition::Initialize_ChessRecognition(int width, int height, int mode) {
	// 관심영역의 width와 height를 입력받고, ChessRecogniton mode를 설정함. 현재 모드 1,2 제공
	static bool first_check = false;

	thread_exit = false;
	
	//init이 처음 호출되었을 경우 thread 동기화를 위한 critical section 초기화
	if (first_check == false) {
		first_check = true;
		InitializeCriticalSection(&cs);
		InitializeCriticalSection(&vec_cs);
	}
	else
		cvReleaseImage(&img_process);

	// mode에 따라 thread를 가동.
	// 1 - linefitting을 이용한 chessboard recognition.
	// 2 - x, y 선형 탐색을 이용한 chessboard recognition.
	if (mode == 1) {
		hThread = (HANDLE)_beginthreadex(NULL, 0, thread_hough, this, 0, NULL);
	}
	else if (mode == 2) {
		hThread = (HANDLE)_beginthreadex(NULL, 0, thread_ChessLineSearchAlg, this, 0, NULL);
	}

	//내부 연산에 사용되는 이미지
	img_process = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);

	_MODE = mode;
	cvZero(img_process);

	_Width = width;	_Height = height;
}

void ChessRecognition::drawLines(vector<pair<float, float> > lines, IplImage* image) {
	// line fitting을 이용한 chessboard recognition의 경우 image에 라인을 그려줌.
	for (register int i = 0; i < MIN(lines.size(), 100); i++) {
		// 호도법에 의해 표현된 line이므로,
		// 각 끝점으로 변환.
		float rho = lines.at(i).first;
		float theta = lines.at(i).second;
		CvPoint pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a * rho, y0 = b * rho; // 수직의 시작이 되는 점.

		pt1.x = cvRound(x0 + (1000 * (-b))); // 끝까지로 쭉 그려버림.
		pt1.y = cvRound(y0 + (1000 * (a)));
		pt2.x = cvRound(x0 - (1000 * (-b)));
		pt2.y = cvRound(y0 - (1000 * (a)));
		cvLine(image, pt1, pt2, CV_RGB(255, 0, 0), 1, 8);

		cvDrawCircle(image, cvPoint(x0, y0), 3, cvScalar(255, 255), -1);
	}
}

void ChessRecognition::drawPoint(IplImage *src, vector<ChessPoint> point) {
	// src image에 chessboard의 교점과 각 교점의 index를 표기.
	char buf[32];

	// display the points in an image 
	for (register int i = 0; i < point.size(); i++) {
		cvCircle(src, point.at(i).Cordinate, 5, cvScalar(0, 255), 2);
		sprintf(buf, "(%d,%d)", point.at(i).index.x, point.at(i).index.y);
		CvFont _TCvFont = cvFont(1.0);
		cvPutText(src, buf, point.at(i).Cordinate, &_TCvFont, cvScalar(255, 0, 0));
	}
}

void ChessRecognition::findIntersections(vector<pair<float, float> > linesX, vector<pair<float, float> > linesY, vector<ChessPoint> *point) {
	// line fitting을 이용한 chessboard recognition의 경우 각 라인의 교점을 연산.
	char buf[32];
	ChessPoint temp_cp;

	point->clear();

	//X축 방향의 line과 Y축 방향의 Line의 교점을 연산
	for (register int i = 0; i < MIN(linesX.size(), 100); i++) {
		for (register int j = 0; j < MIN(linesY.size(), 100); j++)	{                
			float rhoX = linesX.at(i).first;
			float rhoY = linesY.at(j).first;
			float thetaX = linesX.at(i).second, thetaY = linesY.at(j).second;

			double aX = cos(thetaX), bX = sin(thetaX);
			double aY = cos(thetaY), bY = sin(thetaY);

			CvPoint c; // the intersection point of lineX[i] and lineY[j] 

			double Cx = ((rhoX * bY) - (rhoY * bX)) / ((aX * bY) - (bX * aY));
			double Cy = (rhoX - (aX * Cx)) / bX;

			c.x = cvRound(Cx);
			c.y = cvRound(Cy);

			//save point
			temp_cp.Cordinate = c;
			temp_cp.index = cvPoint(j, i);
			point->push_back(temp_cp);
		}
	}
}

void ChessRecognition::Get_Line(vector<pair<float, float> > *linesX, vector<pair<float, float> > *linesY) {
	// line fitting을 이용한 chessboard recognition의 경우 thread를 통하여 연산해낸 결과를 lineX와 LineY에 return;
	linesX->clear();
	linesY->clear();

	//critical section을 통하여 thread 동기를 맞춤
	EnterCriticalSection(&vec_cs);
	copy(vec_LineX.begin(), vec_LineX.end(), back_inserter(*linesX));
	copy(vec_LineY.begin(), vec_LineY.end(), back_inserter(*linesY));
	LeaveCriticalSection(&vec_cs);

	//thread로 부터 가져온 라인을 9개로 merge.
	mergeLine(linesX);
	mergeLine(linesY);
}

// 이웃들의 값을 살펴보고 조건에 해당하지 않으면 지움.
void ChessRecognition::Non_Maximum_Suppression(IplImage* image, IplImage* image2, int kernel)	{
	//조건은 이웃값보다 작음
	float neighbor, neighbor2;

	for (register int y = 0; y < image->height; y++)	{
		for (register int x = 0; x < image->width; x++) {
			float intensity = CV_IMAGE_ELEM(image, float, y, x);

			if (intensity > 0) {
				int flag = 0;

				for (register int ky = -kernel; ky <= kernel; ky++) {
					// in y-direction
					if (y + ky < 0 || y + ky >= image->height) {
						// border check
						continue;
					}
					// in x-direction
					for (register int kx = -kernel; kx <= kernel; kx++) {
						if (x + kx < 0 || x + kx >= image->width) {
							// border check
							continue;
						}
						neighbor = CV_IMAGE_ELEM(image, float, y + ky, x + kx);
						neighbor2 = CV_IMAGE_ELEM(image2, float, y + ky, x + kx);
						// if ( intensity < neighbor ) {
						if (intensity < neighbor || intensity < neighbor2) {
							CV_IMAGE_ELEM(image, float, y, x) = 0.0;
							flag = 1;
							break;
						}
					}
					if (1 == flag) {
						break;
					}
				}
			}
			else {
				CV_IMAGE_ELEM(image, float, y, x) = 0.0;
			}
		}  
	}
}

void ChessRecognition::cast_seq(CvSeq* linesX, CvSeq* linesY) {
	// CvSeq 객체를 rho, theta로 이루이전 직선으로 변환 후, stl vector에 저장.
	vec_LineX.clear();
	vec_LineY.clear();

	// X축 방향의 직선에 대하여 연산.
	for (register int i = 0; i < MIN(linesX->total, 100); i++)	{
		//cvSeq로 부터 rho, theta 연산 
		float *line = (float *)cvGetSeqElem(linesX, i);
		float rho = line[0];
		float theta = line[1];

		vec_LineX.push_back(pair<float, float>(rho, theta));
	}

	// Y축 방향에 대하여 직선 연산.
	for (register int i = 0; i < MIN(linesY->total, 100); i++) {
		float *line = (float*)cvGetSeqElem(linesY, i);
		float rho = line[0];
		float theta = line[1];

		vec_LineY.push_back(pair<float, float>(rho, theta));
	}
}

bool sort_first(pair<float, float> a, pair<float, float> b) {
	return a.first < b.first;										//각도로 정렬
}

void ChessRecognition::mergeLine(vector<std::pair<float, float> > *Lines) {
	// Threshold를 기준으로 유사한 라인을 병합함.
	float SUB_UNDER = 0.0, SUB_MIN = 9999;
	vector<std::pair<float, float> > temp;
	pair<int, int> Min_pair;

	//연산 초기화
	for (register int i = 0; i < Lines->size(); i++) {
		if (Lines->at(i).first < 0) {
			Lines->at(i).first = fabs(Lines->at(i).first);
			Lines->at(i).second = Lines->at(i).second - CV_PI;
		}
	}

	// 벡터 복수 & 직선 벡터를 각도로 정렬.
	copy(Lines->begin(), Lines->end(), back_inserter(temp));
	sort(temp.begin(), temp.end(), sort_first);

	// chessboard에 존재하는 직선의 갯수가 9개 이므로 9개가 남을때까지 병합.
	if (temp.size() > 9) {
		for (register int i = 0; i < (int)Lines->size() - 9; i++) {
			SUB_MIN = 9999;		
			for (register int j = 1; j < temp.size(); j++) {
				float SUB = temp.at(j).first - temp.at(j-1).first;

				if (SUB < SUB_MIN) {
					SUB_MIN = SUB;
					Min_pair.first = j - 1;
					Min_pair.second = j;
				}
			}
			// vector의 first는 rho, second는 theta.
			temp.at(Min_pair.first).first = (temp.at(Min_pair.first).first + temp.at(Min_pair.second).first) / 2.0;
			temp.at(Min_pair.first).second = (temp.at(Min_pair.first).second + temp.at(Min_pair.second).second) / 2.0;

			temp.erase(temp.begin() + Min_pair.second);
		}

		Lines->clear();
		copy(temp.begin(), temp.end(), back_inserter(*Lines));
	}
}

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
	IplImage *iplDoGyClone = cvCloneImage(iplDoGy), *iplDoGxClone = cvCloneImage(iplDoGx);
	IplImage *iplEdgeX = cvCreateImage(cvGetSize(iplTemp), 8, 1);
	IplImage *iplEdgeY = cvCreateImage(cvGetSize(iplTemp), 8, 1);

	CvMemStorage* storageX = cvCreateMemStorage(0), *storageY = cvCreateMemStorage(0);

	while (1) {
		// 이미지를 받아옴. main루프와 동기를 맞추기 위해서 critical section 사용.
		EnterCriticalSection(&(_TChessRecognition->cs));
		cvConvert(_TChessRecognition->img_process, iplTemp);
		LeaveCriticalSection(&_TChessRecognition->cs);

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
		_TChessRecognition->Non_Maximum_Suppression(iplDoGx, iplDoGyClone, kernel);
		_TChessRecognition->Non_Maximum_Suppression(iplDoGy, iplDoGxClone, kernel);

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
		EnterCriticalSection(&_TChessRecognition->vec_cs);
		_TChessRecognition->cast_seq(LineX, LineY);
		LeaveCriticalSection(&_TChessRecognition->vec_cs);

		Sleep(10);

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
		EnterCriticalSection(&(_TChessRecognition->cs));
		
		if (_TChessRecognition->img_process->nChannels != 1)
			cvConvert(_TChessRecognition->img_process, _TGray);
		else
			cvCopy(_TChessRecognition->img_process, _TGray);

		LeaveCriticalSection(&_TChessRecognition->cs);

		// 복사한 이미지를 실제 연산에 사용.
		EnterCriticalSection(&_TChessRecognition->vec_cs);
		_TChessRecognition->Chess_recognition_process(_TGray, &_TChessRecognition->_CP);
		LeaveCriticalSection(&_TChessRecognition->vec_cs);

		// Sleep 10은 왜?
		Sleep(10);

		if (_TChessRecognition->thread_exit == true)
			break;
	}

	_endthread();

	// 이미지 할당 해제.
	cvReleaseImage(&_TGray);

	return 0;
}

void ChessRecognition::Copy_Img(IplImage *src){
	// main의 cam으로부터 받아온 이미지를 thread의 연산에 사용하기 위해서 복사해오는 함수.
	// 연산에 사용되는 이미지는 Grayscale이기 때문에 색상 변환 연산 진행.

	EnterCriticalSection(&cs);
	if (src->nChannels == 1) {
		cvCopy(src, img_process);
	}
	else {
		cvCvtColor(src, img_process, CV_BGR2GRAY);
	}
	LeaveCriticalSection(&cs);
}

void ChessRecognition::Refine_CrossPoint(vector<ChessPoint> *point){
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

void ChessRecognition::Chess_recog_wrapper(IplImage *src, vector<ChessPoint> *point) {
	//init함수를 통하여 설정한 mode에 맞추어 chessboard recognition & 좌표 보정 & src에 좌표 그리기 진행.
	//src : 좌표를 그릴 이미지, point : 연산을 통하여 cross point를 저장할 vector.
	vector<std::pair<float, float> > CH_LineX, CH_LineY;
	point->clear();

	if (_MODE == 1) {
		Get_Line(&CH_LineX, &CH_LineY);
		drawLines(CH_LineX, src);
		drawLines(CH_LineY, src);
		findIntersections(CH_LineX, CH_LineY, point);
		Refine_CrossPoint(point);
		drawPoint(src, *point);
	}
	else if (_MODE == 2) {
		EnterCriticalSection(&vec_cs);
		copy(_CP.begin(), _CP.end(), back_inserter(*point));
		LeaveCriticalSection(&vec_cs);
		//Chess_recognition_process(point);
		Refine_CrossPoint(point);
		drawPoint(src, *point);
	}
}

void ChessRecognition::Chess_recognition_process(IplImage *src, vector<ChessPoint> *point) {
	// 영상 이진화.
	_CLSA->GrayImageBinarization(src);

	/*while (1) {
		cvShowImage("ImageBin",src);
		if (cvWaitKey(33))
			break;
	}*/
	// 해당 영상에서의 좌표 x,y 와 grayscale을 추출하여 vector<MyGrayPoint> 형의 line에 저장.
	_CLSA->GetLinegrayScale(src, _CLSA->Linefindcount_x1, _CLSA->Linefindcount_y1, _CLSA->Linefindcount_x2, _CLSA->Linefindcount_y2);

	// 체스판의 경계를 구하여 in_line_point 변수들에 저장.
	_CLSA->GetgraySidelinesPoint(src);
	
	while (1) {
		cvShowImage("findCrossPoint", src);
		if (cvWaitKey(33))
			break;
	}
	
	// 해당 라인에서 9곳의 체스판 경계를 찾지 못 하였으면,
	// 탐색라인을 이동시켜 적절한 탐색라인을 찾는다.
	// flag의 값에 따라 Linefindcount의 값을 변경한다.
	// true  : +
	// false : -
	// 가로 기준으로는 너비를 5로 나눈 값으로 각각 2/5와 3/5 위치를 기준으로 탐색을 한다
	// 세로 기준으로는 높이를 7로 나는 값으로 각각 3/7과 4/7 위치를 기죽으로 탐색을 한다

	// 만약 9곳의 경계를 모두 찾게 되면 해당 라인으로 고정시킨다.

	// x1 ~ y2의 flag여부에 따라 값을 증가시킬지 감소시킬지를 판단하는 요소로 사용한다
	if (_CLSA->Linefindcount_x1 >= (src->width / 5) * 2 - 10)
		_CLSA->flag_x1 = false;

	if (_CLSA->Linefindcount_y1 >= (src->height / 7) * 3 - 10)
		_CLSA->flag_y1 = false;

	if (_CLSA->Linefindcount_x2 >= (src->width / 5) * 2 - 10)
		_CLSA->flag_x2 = false;

	if (_CLSA->Linefindcount_y2 >= (src->height / 7) * 3 - 10)
		_CLSA->flag_y2 = false;

	if (_CLSA->Linefindcount_x1 <= 3)
		_CLSA->flag_x1 = true;

	if (_CLSA->Linefindcount_y1 <= 3)
		_CLSA->flag_y1 = true;	

	if (_CLSA->Linefindcount_x2 <= 3)
		_CLSA->flag_x2 = true;

	if (_CLSA->Linefindcount_y2 <= 3)
		_CLSA->flag_y2 = true;

	// 각 라인이 모든 체스판의 경계를 찾았다면 다음 과정으로 넘어가고,
	// 찾지 못하였으면 x축 또는 y축을 이동시켜 경계를 찾을 수 있는 라인을 찾는다.
	// 만약 9개의 정점을 찾지 못하면 적합한 탐색라인이 아니라 판단하여 
	// 라인 위치를 바꾼다
	// count : 3
	
	if (_CLSA->in_line_point_x1.size() == 9 && _CLSA->in_line_point_x2.size() ==  9 && _CLSA->in_line_point_y1.size() == 9 && _CLSA->in_line_point_y2.size() == 9) {
		// 각 찾은 경계점들의 수직이 되는 점 모두의 교차점을 찾는다.
		_CLSA->GetInCrossPoint(src, point);
	}
	else if (_CLSA->in_line_point_x1.size() != 9 || _CLSA->in_line_point_x2.size() != 9) {

		// flag의 방향성과 모든경계를 찾지 못한 경우에는 해당되는 라인을 3씩 증감시켜준다

		if (_CLSA->flag_x1 && (_CLSA->in_line_point_x1.size() != 9))
			_CLSA->Linefindcount_x1 += 3;
		else if (!_CLSA->flag_x1 && (_CLSA->in_line_point_x1.size() != 9))
			_CLSA->Linefindcount_x1 -= 3;
		if (_CLSA->flag_x2 && (_CLSA->in_line_point_x2.size() != 9))
			_CLSA->Linefindcount_x2 += 3;
		else if (!_CLSA->flag_x2 && (_CLSA->in_line_point_x2.size() != 9))
			_CLSA->Linefindcount_x2 -= 3;
	}
	else if (_CLSA->in_line_point_y1.size() != 9 || _CLSA->in_line_point_y2.size() != 9) {
		if (_CLSA->flag_y1 && (_CLSA->in_line_point_y1.size() != 9))
			_CLSA->Linefindcount_y1 += 3;
		else if (!_CLSA->flag_y1 && (_CLSA->in_line_point_y1.size() != 9))
			_CLSA->Linefindcount_y1 -= 3;
		if (_CLSA->flag_y2 && (_CLSA->in_line_point_y2.size() != 9))
			_CLSA->Linefindcount_y2 += 3;
		else if (!_CLSA->flag_y2 && (_CLSA->in_line_point_y2.size() != 9))
			_CLSA->Linefindcount_y2 -= 3;
	}

	// 메모리 초기화.
	_CLSA->MemoryClear();
}
