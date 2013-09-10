#pragma once
#include <cv.h>
#include <highgui.h>
#include <Windows.h>
#include <process.h>
#include <thread>

#define H_THREAD 12

using namespace std;

typedef struct{
	CvPoint Cordinate;//��ǥ ��ġ
	CvPoint index;//��ǥ �ε���
}Chess_point;

class Chess_recognition
{
private:
	int WIDTH, HEIGHT;
	int MODE;

	vector<Chess_point> CP;						//����
	vector<std::pair<float, float>> vec_LineX, vec_LineY;		//���� : <rho, theta>
	IplImage *img_process;

	HANDLE hThread;

	void NMS2 ( IplImage* image, IplImage* image2, int kernel);	//
	void cast_seq(CvSeq* linesX, CvSeq* linesY);					//���ͷ� ��ȯ
	//bool sort_first(pair<float, float> a, pair<float, float> b);	//���� ���� ��Ģ
	void mergeLine( vector<std::pair<float, float>> *Lines);

	static UINT WINAPI thread_hough(void *arg);											//������ �Լ�
	static UINT WINAPI thread_GH(void *arg);
	CRITICAL_SECTION cs, vec_cs;														//thread ����ȭ�� ���� cs

	//////////////////////////////////////////////////////////////////////////////��ȫ��ȫ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	typedef struct{
		int x,y;
	} MyPoint;

	typedef struct{
		int x1,y1,x2,y2;
	} MyLinePoint;

	typedef struct{
		int grayscale,x,y;
	}MyGrayPoint;

	typedef struct{
		MyPoint LeftTop, LeftBottom, RightTop, RightBottom;
	}MySquarePoint;

	int line_avg_x1, line_avg_x2, line_avg_y1, line_avg_y2;
	int Linefindcount_x, Linefindcount_y;
	bool flag_x, flag_y;
	
	vector<MyPoint> in_line_point_x1, in_line_point_x2, in_line_point_y1, in_line_point_y2;
	vector<MyGrayPoint> line_x1, line_x2, line_x_mid, line_y1, line_y2, line_y_mid;
	
	MyLinePoint line_point_x1, line_point_x2, line_point_x_mid, line_point_y1, line_point_y2, line_point_y_mid;
	MyLinePoint line_square_left, line_square_top, line_square_right, line_square_bottom;

	MySquarePoint main_square;

	void GetLinegrayScale(IplImage *gray_image, int linefindcount_x, int linefindcount_y);
	void GetgraySidelinesPoint(IplImage *chess_image);
	void GetSquarePoint(IplImage *chess_image);
	void GetInCrossPoint(IplImage *chess_image, vector<Chess_point> *point);
	int Getgrayscale(IplImage *gray_image, int x, int y);
	bool GetCrossPoint(MyLinePoint line1, MyLinePoint line2, MyPoint *out);
	void SetMyLinePoint(int x1, int y1, int x2, int y2, MyLinePoint *setLinePoint);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	Chess_recognition(void);
	~Chess_recognition(void);

	//�ʱ�ȭ
	void Init(int width, int height, int mode);
	//���� �׸���
	void drawLines ( vector<pair<float, float>> lines, IplImage* image);
	void drawPoint ( IplImage *src, vector<Chess_point> point);
	//������ ���ϱ�
	void findIntersections ( vector<pair<float, float>> linesX, vector<pair<float, float>> linesY, vector<Chess_point> *point );
	//���� return
	void Get_Line(vector<pair<float, float>> *linesX, vector<pair<float, float>> *linesY);
	//ó���� �̹��� ����
	void Copy_Img(IplImage *src);
	//������ �����Լ�
	void Refine_CrossPoint(vector<Chess_point> *point);
	//���ɿ��� ����
	void Set_CalculationDomain(CvCapture *Cam, int *ROI_WIDTH, int *ROI_HEIGHT);

	//////////////////////////////////////////////////////////////////////////////��ȫ��ȫ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void Chess_recognition_process(IplImage *src, vector<Chess_point> *point);
	void MemoryClear();

	MyGrayPoint setMyGrayPoint(int grayscale, int x, int y);
	MyPoint setMyPoint(int x, int y);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//wrapper method
	void Chess_recog_wrapper(IplImage *src, vector<Chess_point> *point);
};

