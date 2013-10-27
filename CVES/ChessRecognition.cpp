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

	//mode에 따라 thread를 가동.
	//1 - linefitting을 이용한 chessboard recognition
	//2 - 
	if (mode == 1) {
		hThread = (HANDLE)_beginthreadex(NULL, 0, thread_hough, this, 0, NULL);
	}
	else if (mode == 2) {
		Linefindcount_x1 = 0, Linefindcount_y1 = 0, Linefindcount_x2 = 0, Linefindcount_y2 = 0;
		line_avg_x1 = 40, line_avg_x2 = 40, line_avg_y1 = 40, line_avg_y2 = 40;
		hThread = (HANDLE)_beginthreadex(NULL, 0, thread_GH, this, 0, NULL);
	}

	img_process = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);

	_MODE = mode;
	cvZero(img_process);

	_Width = width;	_Height = height;
}

void ChessRecognition::drawLines(vector<pair<float, float>> lines, IplImage* image) {
	// line fitting을 이용한 chessboard recognition의 경우 image에 라인을 그려줌
	for (register int i = 0; i < MIN(lines.size(),100); i++) {
		//호도법에 의해 표현된 line이므로
		//각 끝점으로 변환
		float rho = lines.at(i).first;
		float theta = lines.at(i).second;
		CvPoint pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;								//수직의 시작이 되는점

		pt1.x = cvRound(x0 + (1000 * (-b)));							//끝까지로 쭉 그려버림
		pt1.y = cvRound(y0 + (1000 * (a)));
		pt2.x = cvRound(x0 - (1000 * (-b)));
		pt2.y = cvRound(y0 - (1000 * (a)));
		cvLine(image, pt1, pt2, CV_RGB(255, 0, 0), 1, 8);

		cvDrawCircle(image, cvPoint(x0, y0), 3, cvScalar(255, 255), -1);
	}
}

void ChessRecognition::drawPoint(IplImage *src, vector<ChessPoint> point) {
	// src image에 chessboard의 교점과 각 교점의 index를 표기
	char buf[32];

	// display the points in an image 
	for (register int i = 0; i < point.size(); i++) {
		cvCircle(src, point.at(i).Cordinate, 5, cvScalar(0, 255), 2);
		sprintf(buf, "(%d,%d)", point.at(i).index.x, point.at(i).index.y);
		cvPutText(src, buf, point.at(i).Cordinate, &cvFont(1.0), cvScalar(255, 0, 0));
	}
}

void ChessRecognition::findIntersections(vector<pair<float, float>> linesX, vector<pair<float, float>> linesY, vector<ChessPoint> *point) {
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

//<<<<<<< HEAD:CVES/ChessRecognition.cpp
void ChessRecognition::Get_Line(vector<pair<float, float>> *linesX, vector<pair<float, float>> *linesY) {
//=======
//void Chess_recognition::Get_Line(vector<pair<float, float>> *linesX, vector<pair<float, float>> *linesY) {
	// line fitting을 이용한 chessboard recognition의 경우 thread를 통하여 연산해낸 결과를 lineX와 LineY에 return;
//>>>>>>> CVES_HandRecognition:CVES/Chess_recognition.cpp
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

//이웃들의 값을 살펴보고 조건에 해당하지 않으면 지움
void ChessRecognition::NMS2(IplImage* image, IplImage* image2, int kernel)	{
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
	// CvSeq 객체를 rho, theta로 이루이전 직선으로 변환 후, stl vector에 저장
	vec_LineX.clear();
	vec_LineY.clear();

	// X축방향의 직선에 대하여 연산
	for (register int i = 0; i < MIN(linesX->total, 100); i++)	{
		//cvSeq로 부터 rho, theta 연산 
		float *line = (float *)cvGetSeqElem(linesX, i);
		float rho = line[0];
		float theta = line[1];

		vec_LineX.push_back(pair<float, float>(rho, theta));
	}

	// Y축방향에 대하여 직선 연산
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

void ChessRecognition::mergeLine(vector<std::pair<float, float>> *Lines) {
	// Threshold를 기준으로 유사한 라인을 병합함.
	float SUB_UNDER = 0.0, SUB_MIN = 9999;
	vector<std::pair<float, float>> temp;
	pair<int, int> Min_pair;

	//연산 초기화
	for(register int i = 0; i < Lines->size(); i++) {
		if (Lines->at(i).first < 0) {
			Lines->at(i).first = fabs(Lines->at(i).first);
			Lines->at(i).second = Lines->at(i).second - CV_PI;
		}
	}

	//벡터 복수 & 직선 벡터를 각도로 정렬
	copy(Lines->begin(), Lines->end(), back_inserter(temp));
	sort(temp.begin(), temp.end(), sort_first);

	//chessboard에 존재하는 직선의 갯수가 9개 이므로 9개가 남을때까지 병합.
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
			//vector의 first는 rho, second는 theta
			temp.at(Min_pair.first).first = (temp.at(Min_pair.first).first + temp.at(Min_pair.second).first) / 2.0;
			temp.at(Min_pair.first).second = (temp.at(Min_pair.first).second + temp.at(Min_pair.second).second) / 2.0;

			temp.erase(temp.begin() + Min_pair.second);
		}

		Lines->clear();
		copy(temp.begin(), temp.end(), back_inserter(*Lines));
	}
}

//<<<<<<< HEAD:CVES/ChessRecognition.cpp
UINT WINAPI ChessRecognition::thread_hough(void *arg) {
	// 실제로 뒤에서 동작하는 windows용 thread함수.
	// 함수 인자로 클래스를 받아옴.
	ChessRecognition* _TChessRecognition = (ChessRecognition*)arg;
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
		// 이미지를 받아옴. main루프와 동기를 맞추기 위해서 critical section 사용
		EnterCriticalSection(&(_TChessRecognition->cs));
		cvConvert(_TChessRecognition->img_process, iplTemp);
		LeaveCriticalSection(&_TChessRecognition->cs);

		// 각 X축 Y축 라인을 검출해 내기 위해서 filter 적용
		cvFilter2D(iplTemp, iplDoGx, &DoGx); // 라인만 축출해내고
		cvFilter2D(iplTemp, iplDoGy, DoGy);
		cvAbs(iplDoGx, iplDoGx);
		cvAbs(iplDoGy, iplDoGy);

		// 이미지 내부에서 최댓값과 최소값을 구하여 정규화.
		cvMinMaxLoc(iplDoGx, &minValx, &maxValx);
		cvMinMaxLoc(iplDoGy, &minValy, &maxValy);
		cvMinMaxLoc(iplTemp, &minValt, &maxValt);
		cvScale(iplDoGx, iplDoGx, 2.0 / maxValx); // 정규화
		cvScale(iplDoGy, iplDoGy, 2.0 / maxValy);
		cvScale(iplTemp, iplTemp, 2.0 / maxValt);

		cvCopy(iplDoGy, iplDoGyClone);
		cvCopy(iplDoGx, iplDoGxClone);

		// NMS진행후 추가 작업
		_TChessRecognition->NMS2(iplDoGx, iplDoGyClone, kernel);
		_TChessRecognition->NMS2(iplDoGy, iplDoGxClone, kernel);

		cvConvert(iplDoGx, iplEdgeY); // IPL_DEPTH_8U로 다시 재변환
		cvConvert(iplDoGy, iplEdgeX);

		double rho = 1.0; // distance resolution in pixel-related units
		double theta = 1.0; // angle resolution measured in radians
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

	// mat 할당 해제
	cvReleaseMat(&DoGy);

	// 내부 연산에 사용된 이미지 할당 해제.
	cvReleaseImage(&iplTemp);
	cvReleaseImage(&iplDoGx);
	cvReleaseImage(&iplDoGy);
	cvReleaseImage(&iplDoGyClone);
	cvReleaseImage(&iplDoGxClone);
	cvReleaseImage(&iplEdgeX);
	cvReleaseImage(&iplEdgeY);

	// houghline2에 사용된 opencv 메모리 할당 해제
	cvReleaseMemStorage(&storageX);
	cvReleaseMemStorage(&storageY);

	_endthread();

	return 0;
}

// mode 2를 통하여 chessboard recognition을 수행하기 위한 thread 함수.
UINT WINAPI ChessRecognition::thread_GH(void *arg) {
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

		//복사한 이미지를 실제 연산에 사용.
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
		// cross point가 81개라면 이전좌표를 통한 보정 연산 진행.
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

void ChessRecognition::Set_CalculationDomain(CvCapture *Cam, int *ROI_WIDTH, int *ROI_HEIGHT){
}

///////////////////////////////////////////////////////////////////////////////////////////

void ChessRecognition::Chess_recognition_process(IplImage *src, vector<ChessPoint> *point) {
	// 영상 이진화
	GrayImageBinarization(src);

	// 해당 영상에서의 좌표 x, y 와 grayscale을 추출하여 vector<MyGrayPoint> 형의 line에 저장.
	GetLinegrayScale(src, Linefindcount_x1, Linefindcount_y1, Linefindcount_x2, Linefindcount_y2);

	// 체스판의 경계를 구하여 in_line_point 변수들에 저장.
	GetgraySidelinesPoint(src);

	// 해당 라인에서 9곳의 체스판 경계를 찾지 못 하였으면
	// 탐색라인을 이동시켜 적절한 탐색라인을 찾는다
	// flag의 값에 따라 Linefindcount의 값을 변경한다
	// true  : +
	// false : -

	// 만약 9곳의 경계를 모두 찾게 되면 해당 라인으로 고정시킨다

	if (Linefindcount_x1 >= (src->width / 5) * 2 - 10)
		flag_x1 = false;

	if (Linefindcount_y1 >= (src->height / 5) * 2 - 10)
		flag_y1 = false;

	if (Linefindcount_x2 >= (src->width / 5) * 2 - 10)
		flag_x2 = false;

	if (Linefindcount_y2 >= (src->height / 5) * 2 - 10)
		flag_y2 = false;

	if (Linefindcount_x1 <= 1)
		flag_x1 = true;

	if (Linefindcount_y1 <= 1)
		flag_y1 = true;	
	
	if (Linefindcount_x2 <= 1)
		flag_x2 = true;

	if (Linefindcount_y2 <= 1)
		flag_y2 = true;

	// 각 라인이 모든 체스판의 경계를 찾았다면 다음 과정으로 넘어가고
	// 찾지 못하였으면 x축 또는 y축을 이동시켜 경계를 찾을 수 있는 라인을 찾는다
	if (in_line_point_x1.size() == 9 && in_line_point_x2.size() ==  9 && in_line_point_y1.size() == 9 && in_line_point_y2.size() == 9) {
		// 각 찾은 경계점들의 수직이 되는 점 모두의 교차점을 찾는다
		GetInCrossPoint(src, point);
	}
	else if (in_line_point_x1.size() != 9 || in_line_point_x2.size() != 9) {
		if (flag_x1 && (in_line_point_x1.size() != 9))
			Linefindcount_x1 += 3;
		else if (!flag_x1 && (in_line_point_x1.size() != 9))
			Linefindcount_x1 -= 3;
		if (flag_x2 && (in_line_point_x2.size() != 9))
			Linefindcount_x2 += 3;
		else if (!flag_x2 && (in_line_point_x2.size() != 9))
			Linefindcount_x2 -= 3;
	}
	else if(in_line_point_y1.size() != 9 || in_line_point_y2.size() != 9) {
		if (flag_y1 && (in_line_point_y1.size() != 9))
			Linefindcount_y1 += 3;
		else if (!flag_y1 && (in_line_point_y1.size() != 9))
			Linefindcount_y1 -= 3;
		if (flag_y2 && (in_line_point_y2.size() != 9))
			Linefindcount_y2 += 3;
		else if (!flag_y2 && (in_line_point_y2.size() != 9))
			Linefindcount_y2 -= 3;
	}

	// 메모리 초기화

	MemoryClear();
}

void ChessRecognition::GetLinegrayScale(IplImage *gray_image, int linefindcount_x1, int linefindcount_y1, int linefindcount_x2, int linefindcount_y2) {
	// 중간 부터 검사

	int image_y = gray_image->height, image_x = gray_image->width;
	int x1, x2, y1, y2;

	// x축의 grayscale을 얻기위해 y축의 탐색위치를 결정
	y1 = ((image_y / 7) * 3) - linefindcount_x1;
	y2 = ((image_y / 7) * 4) + linefindcount_x2;

	// 처음 vector 배열에 중심이 되는 값을 넣어주고
	// 그 이후에 짝수는 오른쪽, 홀수는 왼쪽의 수치를 넣어준다
	line_x1.push_back(setMyGrayPoint(Getgrayscale(gray_image, image_x / 2, y1), image_x / 2, y1));
	line_x2.push_back(setMyGrayPoint(Getgrayscale(gray_image, image_x / 2, y2), image_x / 2, y2));

	for (register int x = 1; x <= image_x / 2; x++) {
		line_x1.push_back(setMyGrayPoint(Getgrayscale(gray_image, (image_x / 2) + x, y1), (image_x / 2) + x, y1));
		line_x1.push_back(setMyGrayPoint(Getgrayscale(gray_image, (image_x / 2) - x, y1), (image_x / 2) - x, y1));

		line_x2.push_back(setMyGrayPoint(Getgrayscale(gray_image, (image_x / 2) + x, y2) ,(image_x / 2) + x, y2));
		line_x2.push_back(setMyGrayPoint(Getgrayscale(gray_image, (image_x / 2) - x, y2) ,(image_x / 2) - x, y2));
	}

	// y축의 grayscale을 얻기위해 x축의 탐색위치를 결정
	x1 = ((image_x / 5) * 2) - linefindcount_y1;
	x2 = ((image_x / 5) * 3) + linefindcount_y2;

	// 처음 vector 배열에 중심이 되는 값을 넣어주고
	// 그 이후에 짝수는 아래쪽, 홀수는 위쪽의 수치를 넣어준다
	line_y1.push_back(setMyGrayPoint(Getgrayscale(gray_image, x1, image_y / 2), x1, image_y / 2));
	line_y2.push_back(setMyGrayPoint(Getgrayscale(gray_image, x2, image_y / 2), x2, image_y / 2));

	for (register int y = 1; y <= image_y / 2; y++) {
		line_y1.push_back(setMyGrayPoint(Getgrayscale(gray_image, x1, (image_y / 2) + y), x1, (image_y / 2) + y));
		line_y1.push_back(setMyGrayPoint(Getgrayscale(gray_image, x1, (image_y / 2) - y), x1, (image_y / 2) - y));

		line_y2.push_back(setMyGrayPoint(Getgrayscale(gray_image, x2, (image_y / 2) + y), x2, (image_y / 2) + y));
		line_y2.push_back(setMyGrayPoint(Getgrayscale(gray_image, x2, (image_y / 2) - y), x2, (image_y / 2) - y));
	}
}

void ChessRecognition::GetgraySidelinesPoint(IplImage *chess_image) {
	// 각 grayscale이 저장되어 있는 vector 배열에서 해당 라인의 교차점을 구한다
	GetgraySidelines(chess_image, &line_x1, &line_point_x1, &in_line_point_x1, true);
	GetgraySidelines(chess_image, &line_x2, &line_point_x2, &in_line_point_x2, true);
	GetgraySidelines(chess_image, &line_y1, &line_point_y1, &in_line_point_y1, false);
	GetgraySidelines(chess_image, &line_y2, &line_point_y2, &in_line_point_y2, false);
}

void ChessRecognition::GetInCrossPoint(IplImage *chess_image, vector<ChessPoint> *point) {
	point->clear();

	// in_line_point 오름차순 정렬
	// 찾은 경계점들을 x또는 y를 중심으로 재정렬한다
	for (register int i = 0; i < in_line_point_x1.size(); i++) {
		for (register int j = i + 1; j < in_line_point_x1.size(); j++) {
			if (in_line_point_x1[i].x > in_line_point_x1[j].x) {
				MyPoint t_point = in_line_point_x1[i];

				in_line_point_x1[i] = in_line_point_x1[j];
				in_line_point_x1[j] = t_point;
			}
			if (in_line_point_x2[i].x > in_line_point_x2[j].x) {
				MyPoint t_point = in_line_point_x2[i];

				in_line_point_x2[i] = in_line_point_x2[j];
				in_line_point_x2[j] = t_point;
			}
			if (in_line_point_y1[i].y > in_line_point_y1[j].y) {
				MyPoint t_point = in_line_point_y1[i];

				in_line_point_y1[i] = in_line_point_y1[j];
				in_line_point_y1[j] = t_point;
			}
			if (in_line_point_y2[i].y > in_line_point_y2[j].y) {
				MyPoint t_point = in_line_point_y2[i];

				in_line_point_y2[i] = in_line_point_y2[j];
				in_line_point_y2[j] = t_point;
			}
		}
	}

	// 찾은 모든 경계점들을 수직이 되는 라인의 경계점들과의 모든 교차점을 찾는다
	// 9 * 9 = 81
	// 그후 모든 교차점을 point 변수에 넣는다 
	MyLinePoint t_in_line_point_x, t_in_line_point_y;
	MyPoint t_in_point;

	for (register int i = 0; i < in_line_point_x1.size(); i++) {
		for (register int j = 0; j < in_line_point_x1.size(); j++) {
			SetMyLinePoint(in_line_point_x1[i].x, in_line_point_x1[i].y, in_line_point_x2[i].x, in_line_point_x2[i].y, &t_in_line_point_x);
			SetMyLinePoint(in_line_point_y1[j].x, in_line_point_y1[j].y, in_line_point_y2[j].x, in_line_point_y2[j].y, &t_in_line_point_y);

			GetCrossPoint(t_in_line_point_x, t_in_line_point_y, &t_in_point);

			ChessPoint temp;
			temp.Cordinate = cvPoint(t_in_point.x, t_in_point.y);
			temp.index = cvPoint(i, j);
			point->push_back(temp);
		}
	}
}

void ChessRecognition::SetMyLinePoint(int x1, int y1, int x2, int y2, MyLinePoint *setLinePoint){
	setLinePoint->x1 = x1;
	setLinePoint->x2 = x2;
	setLinePoint->y1 = y1;
	setLinePoint->y2 = y2;
}

int ChessRecognition::Getgrayscale(IplImage *gray_image, int x, int y){
	int index = x + y*gray_image->widthStep ;
	unsigned char value = gray_image->imageData[index];

	return (int)value;
}

ChessRecognition::MyGrayPoint ChessRecognition::setMyGrayPoint(int grayscale, int x, int y){
	MyGrayPoint _TGrayPoint;

	_TGrayPoint.grayscale = grayscale;
	_TGrayPoint.x = x;
	_TGrayPoint.y = y;

	return _TGrayPoint;
}

ChessRecognition::MyPoint ChessRecognition::setMyPoint(int x, int y) {
	MyPoint _TPoint;
	_TPoint.x = x;
	_TPoint.y = y;

	return _TPoint;
}

bool ChessRecognition::GetCrossPoint(MyLinePoint line1, MyLinePoint line2, MyPoint *out) {
	float x12 = line1.x1 - line1.x2;
	float x34 = line2.x1 - line2.x2;
	float y12 = line1.y1 - line1.y2;
	float y34 = line2.y1 - line2.y2;

	float c = (x12 * y34) - (y12 * x34);

	if (fabs(c) < 0.01)	{
		// No intersection
		return false;
	}
	else {
		// Intersection
		float a = (line1.x1 * line1.y2) - (line1.y1 * line1.x2);
		float b = (line2.x1 * line2.y2) - (line2.y1 * line2.x2);

		float x = ((a * x34) - (b * x12)) / c;
		float y = ((a * y34) - (b * y12)) / c;

		out->x = (int)x;
		out->y = (int)y;

		return true;
	}
}

void ChessRecognition::GrayImageBinarization(IplImage *gray_image) {
	// 이미지의 grayscale을 저장할 변수
	float hist[256]={0,};
	int temp[256];

	memset(temp, 0, sizeof(int) * 256);

	bool flag = true;

	// 영상의 grayscale을 저장한다

	for(register int i = 0; i < gray_image->width; i++) {
		for(register int j = 0; j < gray_image->height; j++) {
			temp[Getgrayscale(gray_image, i, j)]++;

			// 맨처음 영상이 단일색으로 나올 경우가 있기 때문에 예외처리를 해준다
			if (Getgrayscale(gray_image, i, j) != 0)
				flag = false;
		}
	}

	if (flag)
		return;

	float area = (float)gray_image->width * gray_image->height;

	// grayscale의 평균값

	for (register int i = 1; i < 256; i++)
		hist[i] = temp[i] / area;

	int T, Told;

	float sum = 0.f;
	for (register int i=1; i < 256; i++)
		sum += (i * hist[i]);

	T = (int)sum;

	do {
		Told = T;
		int a1, a2, b1, b2, u1, u2;

		a1 = b1 = 0;

		for (register int i = 0; i < Told; i++) {
			a1 += (i * temp[i]);
			b1 += temp[i];
		}

		u1 = a1 / b1;
		a2 = b2 = 0;

		for (register int i = Told + 1; i < 256; i++) {
			a2 += (i * temp[i]);
			b2 += (temp[i]);
		}
		u2 = a2 / b2;

		if (b1 == 0) b1 = 1.f;
		if (b2 == 0) b2 = 1.f;

		T = (int)((u1 + u2) / 2);
	} while(T != Told);

	for(register int i = 0; i < gray_image->width; i++) {
		for(register int j = 0; j < gray_image->height; j++) {
			int index = i + (j * gray_image->widthStep);

			// 해당 위치의 grayscale을 T값을 기준으로 이진화를 결정한다.
			gray_image->imageData[index] = Getgrayscale(gray_image, i, j) > T ? 255 : 0;
		}
	}
}

void ChessRecognition::GetgraySidelines(IplImage *image, vector<MyGrayPoint> *line, MyLinePoint *line_point, vector<MyPoint> *in_line_point, bool XYFlag){
	// 경계를 찾은 후 어느 정도의 경계에는 계산을 하지 않는다.
	int line_count = 0, jump_count_p = 0, jump_count_m = 0, jump_count = 0 ;

	if(XYFlag){
		jump_count = image->width / 12;
		line_point->x1 = image->width / 2;
	}
	else{
		jump_count = image->height / 12;
		line_point->y1 = image->height / 2;
	}

	// 교차되는 체스판의 경계를 검출 할 때 체스 말이 판과 대비가 될 경우
	// 경계선으로 인식 되는 경우를 막는다.
	bool change_flag_line_t, change_flag_line_t1, change_flag_line_t2;

	// line vector함수 차수 변환

	vector<MyGrayPoint> _TT = *((vector<MyGrayPoint> *)line);

	_TT[0].grayscale;
	_TT[1].grayscale;

	change_flag_line_t1 = 255 >= _TT[0].grayscale ? true : false;
	change_flag_line_t2 = 255 >= _TT[1].grayscale ? true : false;

	for (register int i = 2; i < line->size() - 10; i++) {
		if ((i % 2 == 1) && (jump_count_p > 0)) {
			jump_count_p--;
		}
		else if ((i % 2 == 0) && (jump_count_m > 0)) {
			jump_count_m--;
		}
		else {
			bool change_flag_t;

			change_flag_t = 255 >= _TT[i].grayscale ? true : false;

			if (i % 2 == 0)
				change_flag_line_t = change_flag_line_t1;
			else
				change_flag_line_t = change_flag_line_t2;

			if (_TT[i].grayscale != _TT[i + 2].grayscale) {
				int flag = true;

				// 해당 위치에서 그다음 픽셀이 대비가 된다면 경계선으로 인식.

				// 이 부분에서 대각선 방향을 처리해 준다. XYFlag가 ture 면 x축, false이면 y축
				// 해당 방향으로 뻗어있는 두 대각선 방향의 색을 비교하여 차이가나면 경계선으로 인식한다.
				if (XYFlag) {
					if (i % 2 == 1 && (Getgrayscale(image, _TT[i].x + 2, _TT[i].y - 2) != Getgrayscale(image, _TT[i].x + 2, _TT[i].y + 2)))
						return;
					else if (i % 2 ==0  && (Getgrayscale(image, _TT[i].x - 2, _TT[i].y - 2) != Getgrayscale(image, _TT[i].x - 2, _TT[i].y + 2)))
						return;
				}
				else {
					if (i % 2 == 1 && (Getgrayscale(image, _TT[i].x + 2, _TT[i].y + 2) != Getgrayscale(image, _TT[i].x - 2, _TT[i].y + 2))) 
						return;
					else if (i % 2 == 0 && (Getgrayscale(image, _TT[i].x - 2, _TT[i].y - 2) != Getgrayscale(image, _TT[i].x + 2, _TT[i].y - 2)))
						return;
				}

				// 확실히 하기위해 최소 3픽셀 까지 대비가 되면 경계선으로  인식한다

				for (register int j = 1; j <= 3; j++) {
					if(i + (j * 2) > _TT.size())
						continue;

					if (_TT[i].grayscale == _TT[i + (j * 2)].grayscale && change_flag_t == change_flag_line_t) {
						flag = false;
						break;
					}
				}
				if (flag) {
					if (line_count < 9){
						if (line_point->x1 > _TT[i].x) {
							line_point->x1 = _TT[i].x;
							line_point->y1 = _TT[i].y;
						}

						if (line_point->x2 < _TT[i].x) {
							line_point->x2 = _TT[i].x;
							line_point->y2 = _TT[i].y;
						}

						in_line_point->push_back(setMyPoint(_TT[i].x, _TT[i].y));

						cvCircle(image, cvPoint(_TT[i].x, _TT[i].y), 5, cvScalar(0, 0, 0));

						line_count++;

						if (i % 2 == 0)
							change_flag_line_t1 = !change_flag_t;
						else
							change_flag_line_t2 = !change_flag_t;
					}
					if (i % 2 == 1)
						jump_count_p = 10;
					else
						jump_count_m = 10;	
				}
			}
		}

		// 경계선을 9개 다 찾으면 더 이상 찾을 필요가 없으므로 캔슬한다
		if(line_count == 9) {
			break;
		}
	}
}

void ChessRecognition::MemoryClear() {
	line_x1.clear(), line_x2.clear(), line_x_mid.clear(), line_y1.clear(), line_y2.clear(), line_y_mid.clear();

	in_line_point_x1.clear(), in_line_point_x2.clear(), in_line_point_y1.clear(), in_line_point_y2.clear();
}

void ChessRecognition::Chess_recog_wrapper(IplImage *src, vector<ChessPoint> *point) {
	//init함수를 통하여 설정한 mode에 맞추어 chessboard recognition & 좌표 보정 & src에 좌표 그리기 진행.
	//src : 좌표를 그릴 이미지, point : 연산을 통하여 cross point를 저장할 vector
	vector<std::pair<float, float>> CH_LineX, CH_LineY;
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