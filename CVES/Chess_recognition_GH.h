#include <cv.h>
#include <highgui.h>
#include "GlobalVariables.hpp"

using namespace std;

class Chess_recognition_GH
{
	public:
	//////////////////////////////////////////////////////////////////////////////규홍규홍////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 점의 위치를 저장하기 위한 구조체
	typedef struct {
		int x, y;
	} MyPoint;

	// 라인의 양끝 점을 저장하기 위한 구조체
	typedef struct {
		int x1, y1, x2, y2;
	} MyLinePoint;

	// 해당 위치와 함께 grayscale 의 값을 저장하기 위한 구조체
	typedef struct {
		int grayscale, x, y;
	} MyGrayPoint;

	// 사각형의 네 꼭지점을 저장하기 위한 구조체
	typedef struct {
		MyPoint LeftTop, LeftBottom, RightTop, RightBottom;
	} MySquarePoint;

	// 모든 체스판 라인이 검출되기 위한 위치는 찾는데 사용되는 변수
	int Linefindcount_x1, Linefindcount_y1, Linefindcount_x2, Linefindcount_y2;

	// Linefindcount 함수들의 증감을 결정하는 변수
	bool flag_x1, flag_y1, flag_x2, flag_y2;

	vector<MyPoint> in_line_point_x1, in_line_point_x2, in_line_point_y1, in_line_point_y2;
	vector<MyGrayPoint> line_x1, line_x2, line_x_mid, line_y1, line_y2, line_y_mid;

	MyLinePoint line_point_x1, line_point_x2, line_point_x_mid, line_point_y1, line_point_y2, line_point_y_mid;

	MySquarePoint main_square;

	// 영상에서 해당 부분의 grayscale 추출
	void GetLinegrayScale(IplImage *gray_image, int linefindcount_x1, int linefindcount_y1, int linefindcount_x2, int linefindcount_y2);
	// 체스판에서 각 체스판의 경계를 추출
	void GetgraySidelinesPoint(IplImage *chess_image);
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

	void MemoryClear();

	MyGrayPoint setMyGrayPoint(int grayscale, int x, int y);
	MyPoint setMyPoint(int x, int y);
	
	Chess_recognition_GH(void);
	~Chess_recognition_GH(void);
};

