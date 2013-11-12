﻿//////////////////////////////////////////////////////////////////////////////////////////////
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

#include "ChessLineSearchAlg.hpp"

ChessLineSearchAlg::ChessLineSearchAlg() {
	Linefindcount_x1 = 0, Linefindcount_y1 = 0, Linefindcount_x2 = 0, Linefindcount_y2 = 0;
	Linefindcount_x11 = 0, Linefindcount_y11 = 0, Linefindcount_x22 = 0, Linefindcount_y22 = 0;

	refine == false;
}

ChessLineSearchAlg::~ChessLineSearchAlg() {
}

void ChessLineSearchAlg::GetLinegrayScale(IplImage *gray_image, int linefindcount_x1, int linefindcount_y1, int linefindcount_x2, int linefindcount_y2, int linefindcount_x11, int linefindcount_y11, int linefindcount_x22, int linefindcount_y22) {

	int image_y = gray_image->height, image_x = gray_image->width;
	int x1, x2, y1, y2, x11, x22, y11, y22;

	// x축의 grayscale을 얻기 위해 y축의 탐색 위치를 결정.

	y1 = linefindcount_x1;
	y11 = (image_y / 5) + linefindcount_x11;
	y2 = ((image_y / 5) * 4) - linefindcount_x2;
	y22 = image_y - linefindcount_x22;

	// 처음에는 각  vector 배열에 중심이 되는 값을 넣어주고,
	// 그 이후에 짝수는 오른쪽, 홀수는 왼쪽의 수치를 넣어준다.

	// line vector에 push하기 위해 해당 위치의 grayscale을 구해 x축과 y축의 값들을 MyGrayPoint형식으로 반환하여 push해준다

	line_x1.push_back(setMyGrayPoint(Getgrayscale(gray_image, image_x / 2, y1), image_x / 2, y1));
	line_x11.push_back(setMyGrayPoint(Getgrayscale(gray_image, image_x / 2, y11), image_x / 2, y11));
	line_x2.push_back(setMyGrayPoint(Getgrayscale(gray_image, image_x / 2, y2), image_x / 2, y2));
	line_x22.push_back(setMyGrayPoint(Getgrayscale(gray_image, image_x / 2, y22), image_x / 2, y22));

	for (register int x = 1; x <= image_x / 2; x++) {

		// for문에서 도는 x값을 기준으로 +는 오른쪽 -는 왼쪽으로 뻗어 나가 영상의 끝까지 탐색을 해준다

		line_x1.push_back(setMyGrayPoint(Getgrayscale(gray_image, (image_x / 2) + x, y1), (image_x / 2) + x, y1));
		line_x1.push_back(setMyGrayPoint(Getgrayscale(gray_image, (image_x / 2) - x, y1), (image_x / 2) - x, y1));
		line_x11.push_back(setMyGrayPoint(Getgrayscale(gray_image, (image_x / 2) + x, y11), (image_x / 2) + x, y11));
		line_x11.push_back(setMyGrayPoint(Getgrayscale(gray_image, (image_x / 2) - x, y11), (image_x / 2) - x, y11));

		// 일부러 for문을 두개를 쓸 필요는 없으므로 한번에 양쪽과 같은 축을 가지는 라인을 함께 처리해준다

		line_x2.push_back(setMyGrayPoint(Getgrayscale(gray_image, (image_x / 2) + x, y2) ,(image_x / 2) + x, y2));
		line_x2.push_back(setMyGrayPoint(Getgrayscale(gray_image, (image_x / 2) - x, y2) ,(image_x / 2) - x, y2));
		line_x22.push_back(setMyGrayPoint(Getgrayscale(gray_image, (image_x / 2) + x, y22) ,(image_x / 2) + x, y22));
		line_x22.push_back(setMyGrayPoint(Getgrayscale(gray_image, (image_x / 2) - x, y22) ,(image_x / 2) - x, y22));
	}

	// y축의 grayscale을 얻기위해 x축의 탐색위치를 결정.
	x1 = linefindcount_y1;
	x11 = (image_x / 5) + linefindcount_y11;
	x2 = ((image_x / 5) * 4) - linefindcount_y2;
	x22 = image_x - linefindcount_y22;

	// 처음 vector 배열에 중심이 되는 값을 넣어주고, 
	// 그 이후에 짝수는 아래쪽, 홀수는 위쪽의 수치를 넣어준다.
	line_y1.push_back(setMyGrayPoint(Getgrayscale(gray_image, x1, image_y / 2), x1, image_y / 2));
	line_y11.push_back(setMyGrayPoint(Getgrayscale(gray_image, x11, image_y / 2), x11, image_y / 2));
	line_y2.push_back(setMyGrayPoint(Getgrayscale(gray_image, x2, image_y / 2), x2, image_y / 2));
	line_y22.push_back(setMyGrayPoint(Getgrayscale(gray_image, x22, image_y / 2), x22, image_y / 2));

	for (register int y = 1; y <= image_y / 2; y++) {
		line_y1.push_back(setMyGrayPoint(Getgrayscale(gray_image, x1, (image_y / 2) + y), x1, (image_y / 2) + y));
		line_y1.push_back(setMyGrayPoint(Getgrayscale(gray_image, x1, (image_y / 2) - y), x1, (image_y / 2) - y));
		line_y11.push_back(setMyGrayPoint(Getgrayscale(gray_image, x11, (image_y / 2) + y), x11, (image_y / 2) + y));
		line_y11.push_back(setMyGrayPoint(Getgrayscale(gray_image, x11, (image_y / 2) - y), x11, (image_y / 2) - y));

		line_y2.push_back(setMyGrayPoint(Getgrayscale(gray_image, x2, (image_y / 2) + y), x2, (image_y / 2) + y));
		line_y2.push_back(setMyGrayPoint(Getgrayscale(gray_image, x2, (image_y / 2) - y), x2, (image_y / 2) - y));
		line_y22.push_back(setMyGrayPoint(Getgrayscale(gray_image, x22, (image_y / 2) + y), x22, (image_y / 2) + y));
		line_y22.push_back(setMyGrayPoint(Getgrayscale(gray_image, x22, (image_y / 2) - y), x22, (image_y / 2) - y));
	}
}

#if WINDOWS_SYS
UINT WINAPI
	//DWORD WINAPI
#elif POSIX_SYS
// using pthread
void *
#endif
	ChessLineSearchAlg::GraySideLinesPointThread(
#if WINDOWS_SYS
	LPVOID
#elif POSIX_SYS
	void *
#endif
	Param) {

	GraySideLinesPointStruct *_TGraySideLinesPointStruct = (GraySideLinesPointStruct *)Param;
	_TGraySideLinesPointStruct->T_ChessLineSearchAlg->GetgraySidelines(_TGraySideLinesPointStruct->chess_image, _TGraySideLinesPointStruct->Lines, _TGraySideLinesPointStruct->LinePoint, _TGraySideLinesPointStruct->InLinePoints, _TGraySideLinesPointStruct->XYflag);
	//_endthread();
	return 0;
}

void ChessLineSearchAlg::GetgraySidelinesPoint(IplImage *chess_image) {
	// GetLinegrayScale에서 얻은 4개의 라인에서 경계점을 탐색한다

	// 각 grayscale이 저장되어 있는 vector 배열에서 해당 라인의 교차점을 구한다.
	HANDLE _THandleArr[8];
	_THandleArr[0] = (HANDLE)_beginthreadex(NULL, 0, 
		GraySideLinesPointThread, 
		(LPVOID)&GraySideLinesPointStruct(this, chess_image, &line_x1, &line_point_x1, &in_line_point_x1, true), 
		0, NULL);
	_THandleArr[1] = (HANDLE)_beginthreadex(NULL, 0, 
		GraySideLinesPointThread,
		(LPVOID)&GraySideLinesPointStruct(this, chess_image, &line_x2, &line_point_x2, &in_line_point_x2, true),
		0, NULL);
	_THandleArr[2] = (HANDLE)_beginthreadex(NULL, 0,
		GraySideLinesPointThread,
		(LPVOID)&GraySideLinesPointStruct(this, chess_image, &line_y1, &line_point_y1, &in_line_point_y1, false),
		0, NULL);
	_THandleArr[3] = (HANDLE)_beginthreadex(NULL, 0,
		GraySideLinesPointThread,
		(LPVOID)&GraySideLinesPointStruct(this, chess_image, &line_y2, &line_point_y2, &in_line_point_y2, false),
		0, NULL);
	_THandleArr[4] = (HANDLE)_beginthreadex(NULL, 0, 
		GraySideLinesPointThread, 
		(LPVOID)&GraySideLinesPointStruct(this, chess_image, &line_x11, &line_point_x11, &in_line_point_x11, true), 
		0, NULL);
	_THandleArr[5] = (HANDLE)_beginthreadex(NULL, 0, 
		GraySideLinesPointThread,
		(LPVOID)&GraySideLinesPointStruct(this, chess_image, &line_x22, &line_point_x22, &in_line_point_x22, true),
		0, NULL);
	_THandleArr[6] = (HANDLE)_beginthreadex(NULL, 0,
		GraySideLinesPointThread,
		(LPVOID)&GraySideLinesPointStruct(this, chess_image, &line_y11, &line_point_y11, &in_line_point_y11, false),
		0, NULL);
	_THandleArr[7] = (HANDLE)_beginthreadex(NULL, 0,
		GraySideLinesPointThread,
		(LPVOID)&GraySideLinesPointStruct(this, chess_image, &line_y22, &line_point_y22, &in_line_point_y22, false),
		0, NULL);

	// 이 함수 4개를 스레드로
	WaitForMultipleObjects(8, _THandleArr, TRUE, INFINITE);

	CloseHandle(_THandleArr[0]);
	CloseHandle(_THandleArr[1]);
	CloseHandle(_THandleArr[2]);
	CloseHandle(_THandleArr[3]);
	CloseHandle(_THandleArr[4]);
	CloseHandle(_THandleArr[5]);
	CloseHandle(_THandleArr[6]);
	CloseHandle(_THandleArr[7]);

// 	GetgraySidelines(chess_image, &line_x1, &line_point_x1, &in_line_point_x1, true);
// 	GetgraySidelines(chess_image, &line_x2, &line_point_x2, &in_line_point_x2, true);
// 	GetgraySidelines(chess_image, &line_y1, &line_point_y1, &in_line_point_y1, false);
// 	GetgraySidelines(chess_image, &line_y2, &line_point_y2, &in_line_point_y2, false);
// 	GetgraySidelines(chess_image, &line_x11, &line_point_x11, &in_line_point_x11, true);
// 	GetgraySidelines(chess_image, &line_x22, &line_point_x22, &in_line_point_x22, true);
// 	GetgraySidelines(chess_image, &line_y11, &line_point_y11, &in_line_point_y11, false);
// 	GetgraySidelines(chess_image, &line_y22, &line_point_y22, &in_line_point_y22, false);

	GetTrueLines(in_line_point_x1, in_line_point_x11, &true_line_point_x1);
	GetTrueLines(in_line_point_x22, in_line_point_x2, &true_line_point_x2);
	GetTrueLines(in_line_point_y1, in_line_point_y11, &true_line_point_y1);
	GetTrueLines(in_line_point_y22, in_line_point_y2, &true_line_point_y2);
}

void ChessLineSearchAlg::GetInCrossPoint(IplImage *chess_image, vector<ChessPoint> *point) {
	point->clear();

	// in_line_point 오름차순 정렬.
	// 찾은 경계점들을 x또는 y를 중심으로 재정렬한다.
	for (register int i = 0; i < true_line_point_x1.size(); i++) {
		for (register int j = i + 1; j < true_line_point_x1.size(); j++) {

			// in_line_point 4개의 vector를 간단한 버블 정렬로 정렬을 한다
			// 각 축의 탐색 방향과 대비되는 x 또는 y를 기준으로 삼는다

			if (true_line_point_x1[i].x > true_line_point_x1[j].x) {
				MyPoint t_point = true_line_point_x1[i];

				true_line_point_x1[i] = true_line_point_x1[j];
				true_line_point_x1[j] = t_point;
			}
			if (true_line_point_x2[i].x > true_line_point_x2[j].x) {
				MyPoint t_point = true_line_point_x2[i];

				true_line_point_x2[i] = true_line_point_x2[j];
				true_line_point_x2[j] = t_point;
			}
			if (true_line_point_y1[i].y > true_line_point_y1[j].y) {
				MyPoint t_point = true_line_point_y1[i];

				true_line_point_y1[i] = true_line_point_y1[j];
				true_line_point_y1[j] = t_point;
			}
			if (true_line_point_y2[i].y > true_line_point_y2[j].y) {
				MyPoint t_point = true_line_point_y2[i];

				true_line_point_y2[i] = true_line_point_y2[j];
				true_line_point_y2[j] = t_point;
			}
		}
	}

	// 찾은 모든 경계점들을 수직이 되는 라인의 경계점들과의 모든 교차점을 찾는다.
	// 9 * 9 = 81
	// 그후 모든 교차점을 point 변수에 넣는다.

	for (register int i = 0; i < true_line_point_x1.size(); i++) {
		for (register int j = 0; j < true_line_point_x1.size(); j++) {

			MyLinePoint t_in_line_point_x, t_in_line_point_y;
			MyPoint t_in_point;

			// 같은 축의 라인의 양끝을 이은 직선을 구해 수직이 되는 라인과의 교차점을 찾아
			// 반환해 주는 point에 push한다

			SetMyLinePoint(true_line_point_x1[i].x, true_line_point_x1[i].y, true_line_point_x2[i].x, true_line_point_x2[i].y, &t_in_line_point_x);
			SetMyLinePoint(true_line_point_y1[j].x, true_line_point_y1[j].y, true_line_point_y2[j].x, true_line_point_y2[j].y, &t_in_line_point_y);

			// in_line_point 4개의 변수 모두 9개의 경계점을 가지고 있으므로
			// 각 수직이 되는 직선 9*9로 총 81개의 교차점이 생기게 된다
			// 그것이 체스판의 모든 정점의 위치로 인식을 하고 넘겨준다

			GetCrossPoint(t_in_line_point_x, t_in_line_point_y, &t_in_point);

			ChessPoint temp;
			temp.Cordinate = cvPoint(t_in_point.x, t_in_point.y);
			temp.index = cvPoint(i, j);
			point->push_back(temp);
		}
	}
}

void ChessLineSearchAlg::SetMyLinePoint(int x1, int y1, int x2, int y2, MyLinePoint *setLinePoint) {

	// 2개의 x,y 쌍을 받아 setLinePoint에 반환해준다 

	setLinePoint->x1 = x1;
	setLinePoint->x2 = x2;
	setLinePoint->y1 = y1;
	setLinePoint->y2 = y2;
}

int ChessLineSearchAlg::Getgrayscale(IplImage *gray_image, int x, int y) {

	// 해당 grayscale의 위치를 반환해 주기위해 x,y 해당 위치를 영상에서 찾아 grayscale의 데이터를 받아 value에 저장해준다

	int index = x + y*gray_image->widthStep ;
	unsigned char value = gray_image->imageData[index];

	// grayscale 정수형으로 이루어져 있으므로 int형으로 반환해준다

	return (int)value;
}

MyGrayPoint ChessLineSearchAlg::setMyGrayPoint(int grayscale, int x, int y) {

	// grayscale과 해당하는 위치를 모두 저장하기 위한 반환함수로 함수내에 임시 MyGrayPoint 변수를 만들어 
	// 각 멤버변수에 입력받은 값들을 저장해 반환해준다

	MyGrayPoint t_graypoint;

	t_graypoint.grayscale = grayscale;
	t_graypoint.x = x;
	t_graypoint.y = y;

	return t_graypoint;
}

MyPoint ChessLineSearchAlg::setMyPoint(int x, int y) {

	// 위치값 x,y를 저장하기 위한 반환함수로 함수내에 임시 MyPoint 변수를 만들어 
	// 각 멤버변수에 입력받은 값들을 저장해 반환해준다

	MyPoint t_point;
	t_point.x = x;
	t_point.y = y;

	return t_point;
}

bool ChessLineSearchAlg::GetCrossPoint(MyLinePoint line1, MyLinePoint line2, MyPoint *out) {
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

void ChessLineSearchAlg::GrayImageBinarization(IplImage *gray_image) {
	// 이미지의 grayscale을 저장할 변수.
	float hist[256] = {0, };
	int temp[256];

	memset(temp, 0, sizeof(int) * 256);

	bool flag = true;

	// 영상의 grayscale을 저장한다.
	for (register int i = 0; i < gray_image->width; i++) {
		for (register int j = 0; j < gray_image->height; j++) {
			temp[Getgrayscale(gray_image, i, j)]++;

			// 맨 처음 영상이 단일색으로 나올 경우가 있기 때문에 예외 처리를 해준다.
			if (Getgrayscale(gray_image, i, j) != 0)
				flag = false;
		}
	}

	if (flag)
		return;

	float area = (float)gray_image->width * gray_image->height;

	// grayscale의 평균값.
	for (register int i = 1; i < 256; i++)
		hist[i] = temp[i] / area;

	int T, Told;

	float sum = 0.f;
	for (register int i = 1; i < 256; i++)
		sum += (i * hist[i]);

	T = (int)sum;

	// grayscale의 기준 T를 찾는 과정으로 
	// 간단히 얻을 수있는 모든 grayscale의 평균을 구한다고 보면 된다

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
	} while (T != Told);

	for (register int i = 0; i < gray_image->width; i++) {
		for (register int j = 0; j < gray_image->height; j++) {
			int index = i + (j * gray_image->widthStep);

			// 해당 위치의 grayscale을 T값을 기준으로 이진화를 결정한다 
			gray_image->imageData[index] = Getgrayscale(gray_image, i, j) > T - 20 ? 255 : 0;
		}
	}
}

void ChessLineSearchAlg::GetgraySidelines(IplImage *image, vector<MyGrayPoint> *line, MyLinePoint *line_point, vector<MyPoint> *in_line_point, bool XYFlag) {
	// 경계를 찾은 후 어느 정도의 경계에는 계산을 하지 않는다.
	int line_count = 0, jump_count_p = 0, jump_count_m = 0, jump_count = 0;

	// 기본적으로 계산을 해줄 필요가 없는 최소 픽셀을 jump_count에 저장한다 
	// line_point 에는 해당 축라인에서만 위치가 바뀌니 고정 값을 저장한다

	if (XYFlag) {
		jump_count = 30;/*image->width / 12;*/
		line_point->x1 = image->width / 2;
	}
	else {
		jump_count = 30;/*image->height / 12;*/
		line_point->y1 = image->height / 2;
	}

	// 교차되는 체스판의 경계를 검출 할 때 체스 말이 판과 대비가 될 경우,
	// 경계선으로 인식 되는 경우를 막는다.
	bool change_flag_line_t, change_flag_line_t1, change_flag_line_t2;

	// line vector함수 차수 변환.
	vector<MyGrayPoint> _TT = *((vector<MyGrayPoint> *)line);
	vector<MyPoint> _TT_in = *((vector<MyPoint> *)in_line_point);
	vector<MyPoint> _TT_in1,_TT_in2;

	// 처음 중심이 되는 위치를 저장해준다 짝수는 오른쪽 홀수는 왼쪽으로 나가는 기준점이다

	_TT[0].grayscale;
	_TT[1].grayscale;

	// grayscale의 교차를 탐색에 도움을 주기 위해 양쪽 방향의 기준 grayscale을 잡아준다

	change_flag_line_t1 = 255 >= _TT[0].grayscale ? true : false;
	change_flag_line_t2 = 255 >= _TT[1].grayscale ? true : false;


	// 여기선 추가로 탐색되지 말아야할 라인을 걸러준다.
	// 홀수면 오른쪽 짝수면 왼쪽으로 판단하여 계산해준다

	for (register int i = 0; i < line->size() - 10; i++) {

		// jump_count 가 유효할 경우 탐색을 하지 않아도 될 영역으로 판단 하여 넘긴다
		// 해당되는 jump_count를 건너 뛰며 홀수와 짝수로 판단하여 변수를 분리하였으므로 양쪽 탐색에 문제가 되지 않는다

		if ((i % 2 == 1) && (jump_count_p > 0)) {
			jump_count_p--;
		}
		else if ((i % 2 == 0) && (jump_count_m > 0)) {
			jump_count_m--;
		}
		else {
			bool change_flag_t;

			// 기존에 저장되어있는 grayscale과 비교를 위해 해당 위치의 grayscale을 저장한다 

			change_flag_t = 255 >= _TT[i].grayscale ? true : false;

			// 전에 기준이 된 grayscale을 비교하기 위해 직접 비교를 하는 변수에 저장한다

			if (i % 2 == 0)
				change_flag_line_t = change_flag_line_t1;
			else
				change_flag_line_t = change_flag_line_t2;

			// vector에 교차적으로 저장이 되었기 때문에 2씩 증가해 비교를한다

			if (_TT[i].grayscale != _TT[i + 2].grayscale ) {
				int flag = true;

				// 해당 위치에서 그다음 픽셀이 대비가 된다면 경계선으로 인식.
				// 이 부분에서 대각선 방향을 처리해 준다 XYFlag가 ture 면 x축, false이면 y축.
				// 해당 방향으로 뻗어있는 두 대각선 방향의 색을 비교하여 차이가나면 경계선으로 인식한다.
				// 기준점에서 왼쪽과 오른쪽을 비교해야 하기 때문에 양 쪽으로 1픽셀씩 비교를 해주기 위해 +- 2를 비교한다

				if (XYFlag) {
					if (i % 2 == 1 && (Getgrayscale(image, _TT[i].x + 2, _TT[i].y - 2) != Getgrayscale(image, _TT[i].x + 2, _TT[i].y + 2)))
						return;
					else if (i % 2 == 0 && (Getgrayscale(image, _TT[i].x - 2, _TT[i].y - 2) != Getgrayscale(image, _TT[i].x - 2, _TT[i].y + 2)))
						return;
				}
				else {
					if (i % 2 == 1 && (Getgrayscale(image, _TT[i].x + 2, _TT[i].y + 2) != Getgrayscale(image, _TT[i].x - 2, _TT[i].y + 2))) 
						return;
					else if (i % 2 == 0 && (Getgrayscale(image, _TT[i].x - 2, _TT[i].y - 2) != Getgrayscale(image, _TT[i].x + 2, _TT[i].y - 2)))
						return;
				}

				// 확실히 하기위해 최소 2픽셀 까지 대비가 되면 경계선으로 인식한다.
				for (register int j = 1; j <= 2; j++) {
					if(i + (j * 2) > _TT.size())
						continue;

					// 만약 기준이 된 grayscale에서 정확한 판단을 위한 검사에 grayscale이 같게 되면 가면 break를 해준다
					if (_TT[i].grayscale == _TT[i + (j * 2)].grayscale && change_flag_t == change_flag_line_t) {
						flag = false;
						break;
					}
				}
				if (flag) {

					// 체스판 경계에 필요한 경계는 9개 이므로 그 이상은 받지 않는다

					if (line_count < 9){
						if (line_point->x1 > _TT[i].x) {
							line_point->x1 = _TT[i].x;
							line_point->y1 = _TT[i].y;
						}

						if (line_point->x2 < _TT[i].x) {
							line_point->x2 = _TT[i].x;
							line_point->y2 = _TT[i].y;
						}

						// 왼쪽과 오른쪽을 구분하여 따로 저장해준다

						if(i%2 == 1)
							_TT_in1.push_back(setMyPoint(_TT[i].x, _TT[i].y));
						else
							_TT_in2.push_back(setMyPoint(_TT[i].x, _TT[i].y));
						
						if(XYFlag)
							cvCircle(image, cvPoint(_TT[i].x, _TT[i].y), 5, cvScalar(0, 0, 0));
						else
							cvCircle(image, cvPoint(_TT[i].x, _TT[i].y), 5, cvScalar(0, 0, 0));

						line_count++;

						// 위의 조건을 통과 하였으면 경계가 되는 면 다음의 grayscale을 적용해준다

						if (i % 2 == 0)
							change_flag_line_t1 = !change_flag_t;
						else
							change_flag_line_t2 = !change_flag_t;
					}

					// 경계점을 찾으면 최소한의 범위는 탐색할 필요가 없기 때문에 넘겨준다

					if (i % 2 == 1)
						jump_count_p = jump_count;
					else
						jump_count_m = jump_count;	
				}
			}
		}

		// 경계선을 9개 다 찾으면 더 이상 찾을 필요가 없으므로 캔슬한다.
		if (line_count == 9){
			break;
		}
	}

	// 점들 사이의 거리가 일정부분이상 떨어져있어야 적합한 라인이라고 인식한다.
	// 35px ~ 50px
	// 단 경계점이 두개 이상 탐색이 된 상태여야 하기 때문에 조건을 걸어준다

	int SumFlag = true;
	int _TT_in1_avg = 0, _TT_in2_avg = 0;

	if (XYFlag && (_TT_in1.size() >= 2 && _TT_in2.size() >= 2)) {
		for (register int i = 0; i < _TT_in1.size() - 1; i++) {

			// _TT는 각 라인의 중심에서 양쪽으로 뻗어나가며 찾은 경계점들이 순차적으로 push가 되어있으므로 자신과 그 이후에 탐색된 점과 비교를 한다

			if (abs(_TT_in1[i].x - _TT_in1[i + 1].x) < 30 || abs(_TT_in1[i].x - _TT_in1[i + 1].x) > 50) {
				SumFlag = false;
			}
		}
		for (register int i = 0; i < _TT_in2.size() - 1; i++) {
			if (abs(_TT_in2[i].x - _TT_in2[i + 1].x) < 30 || abs(_TT_in2[i].x - _TT_in2[i + 1].x) > 50) {
				SumFlag = false;
			}
		}
		if (abs(_TT_in1[0].x - _TT_in2[0].x) < 30 || abs(_TT_in1[0].x - _TT_in2[0].x) > 50) {
			SumFlag = false;
		}
	}
	else if (!XYFlag && (_TT_in1.size() >= 2 && _TT_in2.size() >= 2)) {
		for (register int i = 0; i < _TT_in1.size() - 1; i++) {
			if (abs(_TT_in1[i].y - _TT_in1[i + 1].y) < 30 || abs(_TT_in1[i].y - _TT_in1[i + 1].y) > 50) {
				SumFlag = false;
			}
		}
		for (register int i = 0; i <_TT_in2.size() - 1; i++) {
			if (abs(_TT_in2[i].y - _TT_in2[i + 1].y) < 30 || abs(_TT_in2[i].y - _TT_in2[i + 1].y) > 50) {
				SumFlag = false;
			}
		}
		if (abs(_TT_in1[0].y - _TT_in2[0].y) < 30 || abs(_TT_in1[0].y - _TT_in2[0].y) > 50) {
			SumFlag = false;
		}
	}

	//

	int _TT_in1_dist = 0, _TT_in2_dist = 0;

	if (XYFlag && (_TT_in1.size() >= 2 && _TT_in2.size() >= 2)) {
		for (register int i = 0; i < _TT_in1.size() - 1; i++) {

			if(_TT_in1_avg != 0 && abs(_TT_in1_avg - abs(_TT_in1[i].x - _TT_in1[i + 1].x)) > 5){
				_TT_in1[i + 1].x = _TT_in1[i].x + _TT_in1_avg;
			}
			else if(_TT_in1_avg == 0){
				_TT_in1_avg += abs(_TT_in1[i].x - _TT_in1[i + 1].x);
			}
			else if(_TT_in1_avg != 0){
				_TT_in1_avg = abs(_TT_in1[i].x - _TT_in1[i + 1].x);
				
			}
		}
		for (register int i = 0; i < _TT_in2.size() - 1; i++) {

			if(_TT_in2_avg != 0 && abs(_TT_in2_avg - abs(_TT_in2[i].x - _TT_in2[i + 1].x)) > 5){
				_TT_in2[i + 1].x = _TT_in2[i].x + _TT_in2_avg;
			}
			else if(_TT_in2_avg == 0){
				_TT_in2_avg -= abs(_TT_in2[i].x - _TT_in2[i + 1].x);
			}
			else if(_TT_in2_avg != 0){
				_TT_in2_avg = abs(_TT_in2[i].x - _TT_in2[i + 1].x);
				
			}
		}
	}
	else if (!XYFlag && (_TT_in1.size() >= 2 && _TT_in2.size() >= 2)) {
		for (register int i = 0; i < _TT_in1.size() - 1; i++) {

			if(_TT_in1_avg != 0 && abs(_TT_in1_avg - abs(_TT_in1[i].y - _TT_in1[i + 1].y)) > 10){
				_TT_in1[i + 1].y = _TT_in1[i].y + _TT_in1_avg;
			}
			else if(_TT_in1_avg == 0){
				_TT_in1_avg += abs(_TT_in1[i].y - _TT_in1[i + 1].y);
			}
			else if(_TT_in1_avg != 0){
				_TT_in1_avg = abs(_TT_in1[i].y - _TT_in1[i + 1].y) - abs(_TT_in1_avg - abs(_TT_in1[i].y - _TT_in1[i + 1].y));
				
			}
		}
		for (register int i = 0; i <_TT_in2.size() - 1; i++) {

			if(_TT_in2_avg != 0 && abs(_TT_in2_avg - abs(_TT_in2[i].y - _TT_in2[i + 1].y)) > 10){
				_TT_in2[i + 1].y = _TT_in2[i].y + _TT_in2_avg;
			}
			else if(_TT_in2_avg == 0){
				_TT_in2_avg -= abs(_TT_in2[i].y - _TT_in2[i + 1].y);
			}
			else if(_TT_in2_avg != 0){
				_TT_in2_avg = abs(_TT_in2[i].y - _TT_in2[i + 1].y) + abs(_TT_in2_avg - abs(_TT_in2[i].y - _TT_in2[i + 1].y));
				
			}
		}
	}

	// 만약 위의 조건에 벗어나는 경계점을 찾지 않았을 경우 return 해주는 in_line_point 변수에 합하여 return 해준다

	if (SumFlag) {
		if (_TT_in1.size() != 0) {
			for (register int i = 0; i < _TT_in1.size(); i++)
				in_line_point->push_back(_TT_in1[i]);
		}
		if (_TT_in2.size() != 0){
			for (register int i = 0; i < _TT_in2.size(); i++)
				in_line_point->push_back(_TT_in2[i]);
		}
	}
}

void ChessLineSearchAlg::MemoryClear() {

	// 전역으로 쓰인 모든 변수들을 clear해준다

	line_x1.clear(), line_x2.clear(), line_y1.clear(), line_y2.clear(), line_x11.clear(), line_x22.clear(), line_y11.clear(), line_y22.clear();

	in_line_point_x1.clear(), in_line_point_x2.clear(), in_line_point_y1.clear(), in_line_point_y2.clear(), in_line_point_x11.clear(), in_line_point_x22.clear(), in_line_point_y11.clear(), in_line_point_y22.clear();

	true_line_point_x1.clear(), true_line_point_x2.clear(), true_line_point_y1.clear(), true_line_point_y2.clear();
}


void ChessLineSearchAlg::GetTrueLines(vector<MyPoint> in_line_point1, vector<MyPoint> in_line_point2, vector<MyPoint> *Ture_in_line_point){
	
	if(in_line_point1.size() == 9){
		for(int i=0;i<in_line_point1.size();i++)
			Ture_in_line_point->push_back(in_line_point1[i]);
	}
	else if(in_line_point2.size() == 9){
		for(int i=0;i<in_line_point2.size();i++)
			Ture_in_line_point->push_back(in_line_point2[i]);
	}
}
