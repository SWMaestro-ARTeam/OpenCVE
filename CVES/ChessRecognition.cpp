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
	static bool first_check = false;

	thread_exit = false;

	if (first_check == false) {
		first_check = true;
		InitializeCriticalSection(&cs);
		InitializeCriticalSection(&vec_cs);
	}
	else
		cvReleaseImage(&img_process);

	if (mode == 1) {
		hThread = (HANDLE)_beginthreadex(NULL, 0, thread_hough, this, 0, NULL);
	}
	else if (mode == 2) {
		Linefindcount_x = 0, Linefindcount_y = 0;
		line_avg_x1 = 40, line_avg_x2 = 40, line_avg_y1 = 40, line_avg_y2 = 40;
		hThread = (HANDLE)_beginthreadex(NULL, 0, thread_GH, this, 0, NULL);
	}

	img_process = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);

	MODE = mode;
	cvZero(img_process);

	_Width = width;	_Height = height;
}

void ChessRecognition::drawLines(vector<pair<float, float>> lines, IplImage* image) {
	for (register int i = 0; i < MIN(lines.size(),100); i++) {
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

void ChessRecognition::drawPoint(IplImage *src, vector<Chess_point> point) {
	char buf[32];

	// display the points in an image 
	for (register int i = 0; i < point.size(); i++) {
		cvCircle(src, point.at(i).Cordinate, 5, cvScalar(0, 255), 2);
		sprintf(buf, "(%d,%d)", point.at(i).index.x, point.at(i).index.y);
		cvPutText(src, buf, point.at(i).Cordinate, &cvFont(1.0), cvScalar(255, 0, 0));
	}
}

void ChessRecognition::findIntersections(vector<pair<float, float>> linesX, vector<pair<float, float>> linesY, vector<Chess_point> *point) {
	char buf[32];
	Chess_point temp_cp;

	point->clear();

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

void ChessRecognition::Get_Line(vector<pair<float, float>> *linesX, vector<pair<float, float>> *linesY) {
	linesX->clear();
	linesY->clear();
	EnterCriticalSection(&vec_cs);
	copy(vec_LineX.begin(), vec_LineX.end(), back_inserter(*linesX));
	copy(vec_LineY.begin(), vec_LineY.end(), back_inserter(*linesY));
	LeaveCriticalSection(&vec_cs);

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
	vec_LineX.clear();
	vec_LineY.clear();
	for (register int i = 0; i < MIN(linesX->total, 100); i++)	{
		float *line = (float *)cvGetSeqElem(linesX, i);
		float rho = line[0];
		float theta = line[1];

		vec_LineX.push_back(pair<float, float>(rho, theta));
	}

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

	copy(Lines->begin(), Lines->end(), back_inserter(temp));
	sort(temp.begin(), temp.end(), sort_first);

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
			temp.at(Min_pair.first).first = (temp.at(Min_pair.first).first + temp.at(Min_pair.second).first) / 2.0;
			temp.at(Min_pair.first).second = (temp.at(Min_pair.first).second + temp.at(Min_pair.second).second) / 2.0;

			temp.erase(temp.begin() + Min_pair.second);
		}

		Lines->clear();
		copy(temp.begin(), temp.end(), back_inserter(*Lines));
	}
}

UINT WINAPI ChessRecognition::thread_hough(void *arg) {
	ChessRecognition* p = (ChessRecognition*)arg;

	CvSeq *LineX, *LineY;
	double h[] = { -1, -7, -15, 0, 15, 7, 1 };

	CvMat DoGx = cvMat(1, 7, CV_64FC1, h);
	CvMat* DoGy = cvCreateMat(7, 1, CV_64FC1);
	cvTranspose(&DoGx, DoGy); // transpose(&DoGx) -> DoGy

	double minValx, maxValx, minValy, maxValy, minValt, maxValt;
	int kernel = 1;

	IplImage *iplTemp = cvCreateImage(cvSize(p->_Width, p->_Height), IPL_DEPTH_32F, 1);                   
	IplImage *iplDoGx = cvCreateImage(cvGetSize(iplTemp), IPL_DEPTH_32F, 1);  
	IplImage *iplDoGy = cvCreateImage(cvGetSize(iplTemp), IPL_DEPTH_32F, 1);  
	IplImage *iplDoGyClone = cvCloneImage(iplDoGy), *iplDoGxClone = cvCloneImage(iplDoGx);
	IplImage *iplEdgeX = cvCreateImage(cvGetSize(iplTemp), 8, 1);
	IplImage *iplEdgeY = cvCreateImage(cvGetSize(iplTemp), 8, 1);

	CvMemStorage* storageX = cvCreateMemStorage(0), *storageY = cvCreateMemStorage(0);

	while (1) {
		EnterCriticalSection(&(p->cs));
		cvConvert(p->img_process, iplTemp);
		LeaveCriticalSection(&p->cs);

		cvFilter2D(iplTemp, iplDoGx, &DoGx);								//라인만 축출해내고
		cvFilter2D(iplTemp, iplDoGy, DoGy);
		cvAbs(iplDoGx, iplDoGx);
		cvAbs(iplDoGy, iplDoGy);

		cvMinMaxLoc(iplDoGx, &minValx, &maxValx);
		cvMinMaxLoc(iplDoGy, &minValy, &maxValy);
		cvMinMaxLoc(iplTemp, &minValt, &maxValt);
		cvScale(iplDoGx, iplDoGx, 2.0 / maxValx);			//정규화
		cvScale(iplDoGy, iplDoGy, 2.0 / maxValy);
		cvScale(iplTemp, iplTemp, 2.0 / maxValt);

		cvCopy(iplDoGy, iplDoGyClone);
		cvCopy(iplDoGx, iplDoGxClone);

		//NMS진행후 추가 작업
		p->NMS2(iplDoGx, iplDoGyClone, kernel);
		p->NMS2(iplDoGy, iplDoGxClone, kernel);

		cvConvert(iplDoGx, iplEdgeY);							//IPL_DEPTH_8U로 다시 재변환
		cvConvert(iplDoGy, iplEdgeX);

		double rho = 1.0; // distance resolution in pixel-related units
		double theta = 1.0; // angle resolution measured in radians
		int threshold = 20;

		if (threshold == 0)
			threshold = 1;

		LineX = cvHoughLines2(iplEdgeX, storageX, CV_HOUGH_STANDARD, 1.0 * rho, CV_PI / 180 * theta, threshold, 0, 0);
		LineY = cvHoughLines2(iplEdgeY, storageY, CV_HOUGH_STANDARD, 1.0 * rho, CV_PI / 180 * theta, threshold, 0, 0);

		EnterCriticalSection(&p->vec_cs);
		p->cast_seq(LineX, LineY);
		LeaveCriticalSection(&p->vec_cs);

		Sleep(10);

		if (p->thread_exit == true)
			break;
	}

	cvReleaseMat(&DoGy);

	cvReleaseImage(&iplTemp);
	cvReleaseImage(&iplDoGx);
	cvReleaseImage(&iplDoGy);
	cvReleaseImage(&iplDoGyClone);
	cvReleaseImage(&iplDoGxClone);
	cvReleaseImage(&iplEdgeX);
	cvReleaseImage(&iplEdgeY);

	cvReleaseMemStorage(&storageX);
	cvReleaseMemStorage(&storageY);

	_endthread();

	return 0;
}

UINT WINAPI ChessRecognition::thread_GH(void *arg) {
	ChessRecognition* p = (ChessRecognition*)arg;
	IplImage *gray = cvCreateImage(cvSize(p->_Width, p->_Height), IPL_DEPTH_8U, 1);

	while (1) {
		EnterCriticalSection(&(p->cs));
		
		if (p->img_process->nChannels != 1)
			cvConvert(p->img_process, gray);
		else
			cvCopy(p->img_process, gray);

		LeaveCriticalSection(&p->cs);

		EnterCriticalSection(&p->vec_cs);
		p->Chess_recognition_process(gray, &p->CP);
		LeaveCriticalSection(&p->vec_cs);

		Sleep(10);

		if (p->thread_exit == true)
			break;
	}

	_endthread();

	cvReleaseImage(&gray);

	return 0;
}

void ChessRecognition::Copy_Img(IplImage *src){
	EnterCriticalSection(&cs);
	if (src->nChannels == 1) {
		cvCopy(src, img_process);
	}
	else {
		cvCvtColor(src, img_process, CV_BGR2GRAY);
	}
	LeaveCriticalSection(&cs);
}

void ChessRecognition::Refine_CrossPoint(vector<Chess_point> *point){
	static bool first_check = false;
	static vector<CvPoint> prev_point;
	const float Refine_const = 0.9;

	if (first_check == false && point->size() == 81) {
		for (register int i = 0; i < 81; i++)
			prev_point.push_back(point->at(i).Cordinate);

		first_check = true;
	}
	else if (first_check == true) {
		if (point->size() < 81) {
			point->clear();
			for (register int i = 0; i < 81; i++) {
				Chess_point CP_temp;

				CP_temp.Cordinate = cvPoint(prev_point.at(i).x, prev_point.at(i).y); 
				CP_temp.index = cvPoint(i % 9, i / 9);
				point->push_back(CP_temp);
			}
		}
		else {
			for (register int i = 0; i < 81; i++) {
				point->at(i).Cordinate.x = cvRound((Refine_const * (float)prev_point.at(i).x) + ((1.0 - Refine_const) * (float)point->at(i).Cordinate.x));
				point->at(i).Cordinate.y = cvRound((Refine_const * (float)prev_point.at(i).y) + ((1.0 - Refine_const) * (float)point->at(i).Cordinate.y));
				//printf("after: %d %d\n", point->at(i).Cordinate.x, point->at(i).Cordinate.y);

				//prev_point.clear();
				prev_point.at(i) = cvPoint(point->at(i).Cordinate.x, point->at(i).Cordinate.y);
			}
		}
	}
}

void ChessRecognition::Set_CalculationDomain(CvCapture *Cam, int *ROI_WIDTH, int *ROI_HEIGHT){
}

///////////////////////////////////////////////////////////////////////////////////////////

void ChessRecognition::Chess_recognition_process(IplImage *src, vector<Chess_point> *point) {
	
	GrayImageBinarization(src);

	GetLinegrayScale(src, Linefindcount_x, Linefindcount_y);
	GetgraySidelinesPoint(src);

	if (Linefindcount_x >= (src->width / 5) * 2 && (in_line_point_x1.size() != 9 || in_line_point_x2.size() != 9))
		flag_x = false;
	
	if (Linefindcount_y >= (src->height / 5) * 2 && (in_line_point_y1.size() != 9 || in_line_point_y2.size() != 9))
		flag_y = false;

	if (Linefindcount_x <= 1 && (in_line_point_x1.size() != 9 || in_line_point_x2.size() != 9))
		flag_x = true;
	
	if (Linefindcount_y <= 1 && (in_line_point_y1.size() != 9 || in_line_point_y2.size() != 9))
		flag_y = true;

	if (in_line_point_x1.size() == 9 && in_line_point_x2.size() ==  9 && in_line_point_y1.size() == 9 && in_line_point_y2.size() == 9) {
		GetSquarePoint(src);
		GetInCrossPoint(src, point);
	}
	else if (in_line_point_x1.size() != 9 || in_line_point_x2.size() != 9) {
		if (flag_x)
			Linefindcount_x += 5;
		else
			Linefindcount_x -= 5;
	}
	else if (in_line_point_y1.size() != 9 || in_line_point_y2.size() != 9) {
		if (flag_y)
			Linefindcount_y += 5;
		else
			Linefindcount_y -= 5;
	}
	
	MemoryClear();
}

void ChessRecognition::GetLinegrayScale(IplImage *gray_image, int linefindcount_x, int linefindcount_y) {
	// 중간 부터 검사

	int image_y = gray_image->height, image_x = gray_image->width;
	int x1, x2, x_mid, y1, y2, y_mid;

	y1 = ((image_y / 5) * 2) - linefindcount_x;
	y2 = ((image_y / 5) * 3) + linefindcount_x;
	y_mid = (image_y / 5) * 2;

	line_x1.push_back(setMyGrayPoint(Getgrayscale(gray_image, image_x / 2, y1), image_x / 2, y1));
	line_x2.push_back(setMyGrayPoint(Getgrayscale(gray_image, image_x / 2, y2), image_x / 2, y2));
	line_x_mid.push_back(setMyGrayPoint(Getgrayscale(gray_image, image_x / 2, y_mid),image_x / 2, y_mid));

	for (register int x = 1; x <= image_x / 2; x++) {
		line_x1.push_back(setMyGrayPoint(Getgrayscale(gray_image, (image_x / 2) + x, y1), (image_x / 2) + x, y1));
		line_x1.push_back(setMyGrayPoint(Getgrayscale(gray_image, (image_x / 2) - x, y1), (image_x / 2) - x, y1));

		line_x2.push_back(setMyGrayPoint(Getgrayscale(gray_image, (image_x / 2) + x, y2) ,(image_x / 2) + x, y2));
		line_x2.push_back(setMyGrayPoint(Getgrayscale(gray_image, (image_x / 2) - x, y2) ,(image_x / 2) - x, y2));

		line_x_mid.push_back(setMyGrayPoint(Getgrayscale(gray_image, (image_x / 2) + x, y_mid), (image_x / 2) + x, y_mid));
		line_x_mid.push_back(setMyGrayPoint(Getgrayscale(gray_image, (image_x / 2) - x, y_mid), (image_x / 2) - x, y_mid));
	}

	x1 = ((image_x / 5) * 2) - linefindcount_y;
	x2 = ((image_x / 5) * 3) + linefindcount_y;
	x_mid = (image_x / 5) * 2;

	line_y1.push_back(setMyGrayPoint(Getgrayscale(gray_image, x1, image_y / 2), x1, image_y / 2));
	line_y2.push_back(setMyGrayPoint(Getgrayscale(gray_image, x2, image_y / 2), x2, image_y / 2));
	line_y_mid.push_back(setMyGrayPoint(Getgrayscale(gray_image, x_mid, image_y / 2), x_mid, image_y / 2));

	for (register int y = 1; y <= image_y / 2; y++) {
		line_y1.push_back(setMyGrayPoint(Getgrayscale(gray_image, x1, (image_y / 2) + y), x1, (image_y / 2) + y));
		line_y1.push_back(setMyGrayPoint(Getgrayscale(gray_image, x1, (image_y / 2) - y), x1, (image_y / 2) - y));

		line_y2.push_back(setMyGrayPoint(Getgrayscale(gray_image, x2, (image_y / 2) + y), x2, (image_y / 2) + y));
		line_y2.push_back(setMyGrayPoint(Getgrayscale(gray_image, x2, (image_y / 2) - y), x2, (image_y / 2) - y));

		line_y_mid.push_back(setMyGrayPoint(Getgrayscale(gray_image, x_mid, (image_y / 2) + y), x_mid, (image_y / 2) + y));
		line_y_mid.push_back(setMyGrayPoint(Getgrayscale(gray_image, x_mid, (image_y / 2) - y), x_mid, (image_y / 2) - y));
	}
}

void ChessRecognition::GetgraySidelinesPoint(IplImage *chess_image) {
	int line_count_x1, line_count_x2, line_count_x_mid, line_count_y1, line_count_y2, line_count_y_mid;
	int jump_count_p1, jump_count_m1, jump_count_p2, jump_count_m2, jump_count_p3, jump_count_m3;
	int jump_count_x = chess_image->width / 10;
	int jump_count_y = chess_image->height / 10;

	line_count_x1 = line_count_x2 = line_count_x_mid = line_count_y1 = line_count_y2 = line_count_y_mid = 0;
	jump_count_p1 = jump_count_p2 = jump_count_p3 = jump_count_m1 = jump_count_m2 = jump_count_m3 = 0;

	line_point_x1.x1 = line_point_x1.x2 = line_point_x2.x1 = line_point_x2.x2 = line_point_x_mid.x1 = line_point_x_mid.x2 = chess_image->width / 2;
	line_point_y1.y1 = line_point_y1.y2 = line_point_y2.y1 = line_point_y2.y2 = line_point_y_mid.y1 = line_point_y_mid.y1 = chess_image->height / 2;

	bool change_flag_line_x1_t, change_flag_line_x1_t1, change_flag_line_x1_t2;

	change_flag_line_x1_t1 = 127 > line_x1[0].grayscale ? true : false;
	change_flag_line_x1_t2 = 127 > line_x1[1].grayscale ? true : false;

	for (register int i = 0; i < line_x1.size() - 10; i++) {
		if ((i % 2 == 1) && (jump_count_p1 > 0)) {
			jump_count_p1--;
		}
		else if ((i % 2 == 0) && (jump_count_m1 > 0)) {
			jump_count_m1--;
		}
		else {
			bool change_flag_t;

			change_flag_t = 127 > line_x1[i].grayscale ? true : false;

			if (i % 2 == 0)
				change_flag_line_x1_t = change_flag_line_x1_t1;
			else
				change_flag_line_x1_t = change_flag_line_x1_t2;

			if (line_x1[i].grayscale != line_x1[i + 2].grayscale) {
				int flag = true;

				for (register int j = 1; j <= 3; j++) {
					if (line_x1[i].grayscale == line_x1[i + (j * 2)].grayscale && change_flag_t == change_flag_line_x1_t) {
						flag = false;
						break;
					}
				}
				if (flag) {
					if (line_count_x1 < 9){
						if (line_point_x1.x1 > line_x1[i].x) {
							line_point_x1.x1 = line_x1[i].x;
							line_point_x1.y1 = line_x1[i].y;
						}

						if (line_point_x1.x2 < line_x1[i].x) {
							line_point_x1.x2 = line_x1[i].x;
							line_point_x1.y2 = line_x1[i].y;
						}

						in_line_point_x1.push_back(setMyPoint(line_x1[i].x, line_x1[i].y));

						line_count_x1++;
						//cvCircle(chess_image, cvPoint(line_x1[i].x, line_x1[i].y), 5, cvScalar(255,255,255));

						if (i % 2 == 0)
							change_flag_line_x1_t1 = !change_flag_t;
						else
							change_flag_line_x1_t2 = !change_flag_t;
					}
					if (i % 2 == 1)
						jump_count_p1 = 30;
					else
						jump_count_m1 = 30;	
				}
			}
		}

		bool change_flag_line_x2_t, change_flag_line_x2_t1, change_flag_line_x2_t2;

		change_flag_line_x2_t1 = 127 > line_x2[0].grayscale ? true : false;
		change_flag_line_x2_t2 = 127 > line_x2[1].grayscale ? true : false;

		if ((i % 2 == 1) && (jump_count_p2 > 0)) {
			jump_count_p2--;
		}
		else if ((i%2 == 0) && (jump_count_m2 > 0)) {
			jump_count_m2--;
		}
		else {
			bool change_flag_t;
			change_flag_t = 127 > line_x2[i].grayscale ? true : false;

			if (i % 2 == 0)
				change_flag_line_x2_t = change_flag_line_x2_t1;
			else
				change_flag_line_x2_t = change_flag_line_x2_t2;

			if (line_x2[i].grayscale != line_x2[i + 2].grayscale) {
				int flag = true;

				for (register int j = 1; j <= 3; j++) {
					if (line_x2[i].grayscale == line_x2[i + (j * 2)].grayscale && change_flag_t == change_flag_line_x2_t){
						flag = false;
						break;
					}
				}
				if (flag) {
					if (line_count_x2 < 9) {
						if (line_point_x2.x1 > line_x2[i].x) {
							line_point_x2.x1 = line_x2[i].x;
							line_point_x2.y1 = line_x2[i].y;
						}

						if (line_point_x2.x2 < line_x2[i].x) {
							line_point_x2.x2 = line_x2[i].x;
							line_point_x2.y2 = line_x2[i].y;
						}

						in_line_point_x2.push_back(setMyPoint(line_x2[i].x, line_x2[i].y));

						line_count_x2++;
						//cvCircle(chess_image, cvPoint(line_x2[i].x, line_x2[i].y), 5, cvScalar(255, 255, 255));

						if (i % 2 == 0)
							change_flag_line_x2_t1 = !change_flag_t;
						else
							change_flag_line_x2_t2 = !change_flag_t;
					}
					if (i % 2 == 1)
						jump_count_p2 = 30;
					else
						jump_count_m2 = 30;
				}
			}
		}

		if (line_count_x1 == line_count_x2 /*== line_count_x_mid*/ == 9)
			break;
	}

	jump_count_p1 = jump_count_p2 = jump_count_p3 = jump_count_m1 = jump_count_m2 = jump_count_m3 = 0;

	bool change_flag_line_y1_t, change_flag_line_y1_t1, change_flag_line_y1_t2;

	change_flag_line_y1_t1 = 127 > line_y1[0].grayscale ? true : false;
	change_flag_line_y1_t2 = 127 > line_y1[1].grayscale ? true : false;

	for (register int i=0; i < line_y1.size() - 10; i++) {
		if ((i % 2 == 1) && (jump_count_p1 > 0)) {
			jump_count_p1--;
		}
		else if ((i % 2 == 0) && (jump_count_m1 > 0)) {
			jump_count_m1--;
		}
		else {
			bool change_flag_t;
			change_flag_t = 127 > line_y1[i].grayscale ? true : false;

			if (i % 2 == 0)
				change_flag_line_y1_t = change_flag_line_y1_t1;
			else
				change_flag_line_y1_t = change_flag_line_y1_t2;

			if (line_y1[i].grayscale != line_y1[i + 2].grayscale) {
				int flag = true;

				for (register int j = 1; j <= 3; j++) {
					if (line_y1[i].grayscale == line_y1[i + (j * 2)].grayscale && change_flag_t == change_flag_line_y1_t) {
						flag = false;
						break;
					}
				}
				if (flag) {
					if (line_count_y1 < 9) {
						if (line_point_y1.y1 > line_y1[i].y) {
							line_point_y1.x1 = line_y1[i].x;
							line_point_y1.y1 = line_y1[i].y;
						}

						if (line_point_y1.y2 < line_y1[i].y) {
							line_point_y1.x2 = line_y1[i].x;
							line_point_y1.y2 = line_y1[i].y;
						}

						in_line_point_y1.push_back(setMyPoint(line_y1[i].x, line_y1[i].y));

						line_count_y1++;
						//cvCircle(chess_image, cvPoint(line_y1[i].x, line_y1[i].y), 5, cvScalar(255, 255, 255));

						if (i % 2 == 0)
							change_flag_line_y1_t1 = !change_flag_t;
						else
							change_flag_line_y1_t2 = !change_flag_t;
					}
					if (i % 2 == 1)
						jump_count_p1 = 30;
					else
						jump_count_m1 = 30;
				}
			}
		}

		bool change_flag_line_y2_t, change_flag_line_y2_t1, change_flag_line_y2_t2;

		change_flag_line_y2_t1 = 127 > line_y2[0].grayscale ? true : false;
		change_flag_line_y2_t2 = 127 > line_y2[1].grayscale ? true : false;

		if ((i % 2 == 1) && (jump_count_p2 > 0)) {
			jump_count_p2--;
		}
		else if ((i % 2 == 0) && (jump_count_m2 > 0)) {
			jump_count_m2--;
		}
		else {
			bool change_flag_t;
			change_flag_t = 127 > line_y2[i].grayscale ? true : false;

			if (i % 2 == 0)
				change_flag_line_y2_t = change_flag_line_y2_t1;
			else
				change_flag_line_y2_t = change_flag_line_y2_t2;

			if (line_y2[i].grayscale != line_y2[i + 2].grayscale) {
				int flag = true;

				for (register int j = 1; j <= 3; j++) {
					if (line_y2[i].grayscale == line_y2[i + (j * 2)].grayscale && change_flag_t == change_flag_line_y2_t) {
						flag = false;
						break;
					}
				}
				if (flag) {
					if (line_count_y2 < 9) {
						if (line_point_y2.y1 > line_y2[i].y) {
							line_point_y2.x1 = line_y2[i].x;
							line_point_y2.y1 = line_y2[i].y;
						}

						if (line_point_y2.y2 < line_y2[i].y) {
							line_point_y2.x2 = line_y2[i].x;
							line_point_y2.y2 = line_y2[i].y;
						}

						in_line_point_y2.push_back(setMyPoint(line_y2[i].x, line_y2[i].y));

						line_count_y2++;
						//cvCircle(chess_image, cvPoint(line_y2[i].x, line_y2[i].y), 5, cvScalar(255, 255, 255));

						if (i % 2 == 0)
							change_flag_line_y2_t1 = !change_flag_t;
						else
							change_flag_line_y2_t2 = !change_flag_t;
					}

					if (i % 2 == 1)
						jump_count_p2 = 30;
					else
						jump_count_m2 = 30;
				}
			}
		}

		if (line_count_y1 == line_count_y2 /*== line_count_y_mid*/ == 9)
			break;
	}
}

void ChessRecognition::GetSquarePoint(IplImage *chess_image) {
	SetMyLinePoint(line_point_x1.x1, line_point_x1.y1, line_point_x2.x1, line_point_x2.y1, &line_square_left);
	SetMyLinePoint(line_point_y1.x1, line_point_y1.y1, line_point_y2.x1, line_point_y2.y1, &line_square_top);
	SetMyLinePoint(line_point_x1.x2, line_point_x1.y2, line_point_x2.x2, line_point_x2.y2, &line_square_right);
	SetMyLinePoint(line_point_y1.x2, line_point_y1.y2, line_point_y2.x2, line_point_y2.y2, &line_square_bottom);

	MyPoint t_square_lt, t_square_lb, t_square_rt, t_square_rb;

	if (GetCrossPoint(line_square_left, line_square_top, &t_square_lt) && GetCrossPoint(line_square_left, line_square_bottom, &t_square_lb)
		&& GetCrossPoint(line_square_right, line_square_top, &t_square_rt) && GetCrossPoint(line_square_right, line_square_bottom, &t_square_rb)){
		main_square.LeftTop = t_square_lt;
		main_square.LeftBottom = t_square_lb;
		main_square.RightTop = t_square_rt;
		main_square.RightBottom = t_square_rb;

		cvCircle(chess_image, cvPoint(main_square.LeftTop.x, main_square.LeftTop.y), 5, cvScalar(0, 0, 0));
		cvCircle(chess_image, cvPoint(main_square.LeftBottom.x, main_square.LeftBottom.y), 5, cvScalar(0, 0, 0));
		cvCircle(chess_image, cvPoint(main_square.RightTop.x, main_square.RightTop.y), 5, cvScalar(0, 0, 0));
		cvCircle(chess_image, cvPoint(main_square.RightBottom.x, main_square.RightBottom.y), 5, cvScalar(0, 0, 0));
	}
	else
		printf("Get Cross Point error!\n");
}

void ChessRecognition::GetInCrossPoint(IplImage *chess_image, vector<Chess_point> *point) {
	point->clear();

	// in_line_point 오름차순 정렬
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

	MyLinePoint t_in_line_point_x, t_in_line_point_y;
	MyPoint t_in_point;

	for (register int i = 0; i < in_line_point_x1.size(); i++) {
		for (register int j = 0; j < in_line_point_x1.size(); j++) {
			SetMyLinePoint(in_line_point_x1[i].x, in_line_point_x1[i].y, in_line_point_x2[i].x, in_line_point_x2[i].y, &t_in_line_point_x);
			SetMyLinePoint(in_line_point_y1[j].x, in_line_point_y1[j].y, in_line_point_y2[j].x, in_line_point_y2[j].y, &t_in_line_point_y);

			GetCrossPoint(t_in_line_point_x, t_in_line_point_y, &t_in_point);

			Chess_point temp;
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
	MyGrayPoint t_graypoint;

	t_graypoint.grayscale = grayscale;
	t_graypoint.x = x;
	t_graypoint.y = y;

	return t_graypoint;
}

ChessRecognition::MyPoint ChessRecognition::setMyPoint(int x, int y) {
	MyPoint t_point;
	t_point.x = x;
	t_point.y = y;

	return t_point;
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
	float hist[256]={0,};
	int temp[256];

	memset(temp, 0, sizeof(int) * 256);

	bool flag = true;

	for (register int i = 0; i < gray_image->width; i++) {
		for (register int j = 0; j < gray_image->height; j++) {
			temp[Getgrayscale(gray_image, i, j)]++;
			if (Getgrayscale(gray_image, i, j) != 0)
				flag = false;
		}
	}

	if (flag)
		return;

	float area = (float)gray_image->width * gray_image->height;

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

	uchar *data = (uchar *)gray_image->imageData;

	for (register int i=0; i < gray_image->width; i++) {
		for (register int j=0; j < gray_image->height; j++) {
			int index = i + (j * gray_image->widthStep);

			gray_image->imageData[index] = Getgrayscale(gray_image, i, j) > T ? 255 : 0;
		}
	}
}


void ChessRecognition::MemoryClear() {
	line_x1.clear(), line_x2.clear(), line_x_mid.clear(), line_y1.clear(), line_y2.clear(), line_y_mid.clear();

	in_line_point_x1.clear(), in_line_point_x2.clear(), in_line_point_y1.clear(), in_line_point_y2.clear();
}

void ChessRecognition::Chess_recog_wrapper(IplImage *src, vector<Chess_point> *point) {
	vector<std::pair<float, float>> CH_LineX, CH_LineY;
	point->clear();

	if (MODE == 1) {
		Get_Line(&CH_LineX, &CH_LineY);
		drawLines(CH_LineX, src);
		drawLines(CH_LineY, src);
		findIntersections(CH_LineX, CH_LineY, point);
		Refine_CrossPoint(point);
		drawPoint(src, *point);
	}
	else if (MODE == 2) {
		EnterCriticalSection(&vec_cs);
		copy(CP.begin(), CP.end(), back_inserter(*point));
		LeaveCriticalSection(&vec_cs);
		//Chess_recognition_process(point);
		Refine_CrossPoint(point);
		drawPoint(src, *point);
	}
}