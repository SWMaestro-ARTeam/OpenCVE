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

#ifndef _Chess_recognition_hpp_
#define _Chess_recognition_hpp_

#include <cv.h>
#include <highgui.h>
#include <Windows.h>
#include <process.h>
#include <thread>

#define H_THREAD 12

#define MAX_CORNER 5000

using namespace std;

typedef struct{
	CvPoint Cordinate; // 좌표 위치
	CvPoint index; // 좌표 인덱스
}Chess_point;

class Chess_recognition {
private:
	int _Width, _Height;
	int MODE;

	vector<Chess_point> CP; // 교점
	vector<std::pair<float, float>> vec_LineX, vec_LineY; // 라인 : <rho, theta>
	IplImage *img_process;

	HANDLE hThread;

	void NMS2(IplImage* image, IplImage* image2, int kernel);	//
	void cast_seq(CvSeq* linesX, CvSeq* linesY); // 벡터로 변환
	//bool sort_first(pair<float, float> a, pair<float, float> b);	//벡터 정렬 규칙
	void mergeLine( vector<std::pair<float, float>> *Lines);

	static UINT WINAPI thread_hough(void *arg); //쓰레드 함수
	static UINT WINAPI thread_GH(void *arg);
	CRITICAL_SECTION cs, vec_cs; //thread 동기화를 위한 cs
	bool thread_exit;

	//////////////////////////////////////////////////////////////////////////////규홍규홍////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 점의 위치를 저장하기 위한 구조체
	typedef struct {
		int x, y;
	} MyPoint;

	// 라인의 양끝 점을 저장하기 위한 구조체
	typedef struct{
		int x1, y1, x2, y2;
	} MyLinePoint;

	// 해당 위치와 함께 grayscale 의 값을 저장하기 위한 구조체
	typedef struct{
		int grayscale, x, y;
	} MyGrayPoint;

	// 사각형의 네 꼭지점을 저장하기 위한 구조체
	typedef struct {
		MyPoint LeftTop, LeftBottom, RightTop, RightBottom;
	} MySquarePoint;


	int line_avg_x1, line_avg_x2, line_avg_y1, line_avg_y2;
	
	// 모든 체스판 라인이 검출되기 위한 위치는 찾는데 사용되는 변수
	int Linefindcount_x1, Linefindcount_y1, Linefindcount_x2, Linefindcount_y2;

	// Linefindcount 함수들의 증감을 결정하는 변수
	bool flag_x1, flag_y1, flag_x2, flag_y2;
	
	vector<MyPoint> in_line_point_x1, in_line_point_x2, in_line_point_y1, in_line_point_y2;
	vector<MyGrayPoint> line_x1, line_x2, line_x_mid, line_y1, line_y2, line_y_mid;
	
	MyLinePoint line_point_x1, line_point_x2, line_point_x_mid, line_point_y1, line_point_y2, line_point_y_mid;
	MyLinePoint line_square_left, line_square_top, line_square_right, line_square_bottom;

	MySquarePoint main_square;

	// 영상에서 해당 부분의 grayscale 추출
	void GetLinegrayScale(IplImage *gray_image, int linefindcount_x1, int linefindcount_y1, int linefindcount_x2, int linefindcount_y2);
	// 체스판에서 각 체스판의 경계를 추출
	void GetgraySidelinesPoint(IplImage *chess_image);
	// 양 끝의 점을 이용해 사각형을 구함
	void GetSquarePoint(IplImage *chess_image);
	// 추출된 모든 수직이 되는 모든 선들의 교차점을 구함
	void GetInCrossPoint(IplImage *chess_image, vector<Chess_point> *point);
	// 해당 위치의 grayscale을 반환
	int Getgrayscale(IplImage *gray_image, int x, int y);
	// 두 라인의 교차점을 반환
	bool GetCrossPoint(MyLinePoint line1, MyLinePoint line2, MyPoint *out);
	// 정보들을 MyLinePoint 형식 반환
	void SetMyLinePoint(int x1, int y1, int x2, int y2, MyLinePoint *setLinePoint);
	// 영상 이진화 함수
	void GrayImageBinarization(IplImage *gray_image);
	// 체스판 경계 탐색 함수
	void GetgraySidelines(IplImage *image, vector<MyGrayPoint> *line, MyLinePoint *line_point, vector<MyPoint> *in_line_point, bool XYFlag);
	void GetgraySidelines2(IplImage *image, vector<MyGrayPoint> *line, MyLinePoint *line_point, vector<MyPoint> *in_line_point, bool XYFlag);
	// 헤리스 코너 알고리즘
	void HarrisCorner(IplImage *image);
	// 동적 할당함수
	void MyMalloc(double **t,int ti,int tj);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	Chess_recognition();
	~Chess_recognition();

	//초기화
	void Initialize_ChessRecognition(int width, int height, int mode);
	//라인 그리기
	void drawLines ( vector<pair<float, float>> lines, IplImage* image);
	void drawPoint ( IplImage *src, vector<Chess_point> point);
	//교차점 구하기
	void findIntersections ( vector<pair<float, float>> linesX, vector<pair<float, float>> linesY, vector<Chess_point> *point );
	//라인 return
	void Get_Line(vector<pair<float, float>> *linesX, vector<pair<float, float>> *linesY);
	//처리용 이미지 복사
	void Copy_Img(IplImage *src);
	//교차점 보정함수
	void Refine_CrossPoint(vector<Chess_point> *point);
	//관심영역 설정
	void Set_CalculationDomain(CvCapture *Cam, int *ROI_WIDTH, int *ROI_HEIGHT);
	//자원반환
	void exit();

	//////////////////////////////////////////////////////////////////////////////규홍규홍////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void Chess_recognition_process(IplImage *src, vector<Chess_point> *point);
	void MemoryClear();

	MyGrayPoint setMyGrayPoint(int grayscale, int x, int y);
	MyPoint setMyPoint(int x, int y);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//wrapper method
	void Chess_recog_wrapper(IplImage *src, vector<Chess_point> *point);
};
#endif