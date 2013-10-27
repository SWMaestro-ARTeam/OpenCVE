#include <cv.h>
#include <highgui.h>
#include "GlobalVariables.hpp"

using namespace std;

class Chess_recognition_GH
{
	public:
	//////////////////////////////////////////////////////////////////////////////��ȫ��ȫ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ���� ��ġ�� �����ϱ� ���� ����ü
	typedef struct {
		int x, y;
	} MyPoint;

	// ������ �糡 ���� �����ϱ� ���� ����ü
	typedef struct {
		int x1, y1, x2, y2;
	} MyLinePoint;

	// �ش� ��ġ�� �Բ� grayscale �� ���� �����ϱ� ���� ����ü
	typedef struct {
		int grayscale, x, y;
	} MyGrayPoint;

	// �簢���� �� �������� �����ϱ� ���� ����ü
	typedef struct {
		MyPoint LeftTop, LeftBottom, RightTop, RightBottom;
	} MySquarePoint;

	// ��� ü���� ������ ����Ǳ� ���� ��ġ�� ã�µ� ���Ǵ� ����
	int Linefindcount_x1, Linefindcount_y1, Linefindcount_x2, Linefindcount_y2;

	// Linefindcount �Լ����� ������ �����ϴ� ����
	bool flag_x1, flag_y1, flag_x2, flag_y2;

	vector<MyPoint> in_line_point_x1, in_line_point_x2, in_line_point_y1, in_line_point_y2;
	vector<MyGrayPoint> line_x1, line_x2, line_x_mid, line_y1, line_y2, line_y_mid;

	MyLinePoint line_point_x1, line_point_x2, line_point_x_mid, line_point_y1, line_point_y2, line_point_y_mid;

	MySquarePoint main_square;

	// ���󿡼� �ش� �κ��� grayscale ����
	void GetLinegrayScale(IplImage *gray_image, int linefindcount_x1, int linefindcount_y1, int linefindcount_x2, int linefindcount_y2);
	// ü���ǿ��� �� ü������ ��踦 ����
	void GetgraySidelinesPoint(IplImage *chess_image);
	// ����� ��� ������ �Ǵ� ��� ������ �������� ����
	void GetInCrossPoint(IplImage *chess_image, vector<Chess_point> *point);
	// �ش� ��ġ�� grayscale�� ��ȯ
	int Getgrayscale(IplImage *gray_image, int x, int y);
	// �� ������ �������� ��ȯ
	bool GetCrossPoint(MyLinePoint line1, MyLinePoint line2, MyPoint *out);
	// �������� MyLinePoint ���� ��ȯ
	void SetMyLinePoint(int x1, int y1, int x2, int y2, MyLinePoint *setLinePoint);
	// ���� ����ȭ �Լ�
	void GrayImageBinarization(IplImage *gray_image);
	// ü���� ��� Ž�� �Լ�
	void GetgraySidelines(IplImage *image, vector<MyGrayPoint> *line, MyLinePoint *line_point, vector<MyPoint> *in_line_point, bool XYFlag);

	void MemoryClear();

	MyGrayPoint setMyGrayPoint(int grayscale, int x, int y);
	MyPoint setMyPoint(int x, int y);
	
	Chess_recognition_GH(void);
	~Chess_recognition_GH(void);
};

