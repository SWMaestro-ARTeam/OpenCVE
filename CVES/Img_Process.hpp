#pragma once
#include <stdio.h>
#include <highgui.h>
#include <cv.h>
#include <Windows.h>

#include "Hand_recognition.hpp"
#include "Chess_recognition.hpp"
#include "BlobLabeling.hpp"
#include "chess_game.hpp"

#define WIDTH 640
#define HEIGHT 480
#define RECOGNITION_MODE 2				//1 : hough Line, 2 : ��ȫ recognition

#define DEBUG

class Img_Process
{
public:
	Img_Process(void);
	~Img_Process(void);

	void Init_process();			//image process �ʱ�ȭ
	void Get_History();				//���� �̵��� ��� ť�� ������ -- �̱���
	void Exit_imgProcess();			//�̹��� ���μ����� ������
	void Do_imgprocess();			//�� �������� ȣ��Ǵ� image process �Լ�
	bool Check_Exit();

private:
	Hand_recognition Find_Hand;
	Chess_recognition Find_Chess;
	CBlobLabeling CBlob;
	chess_game CHESS_GAME;

	int ImgProcess_Mode;				//��� ����
	bool Sub_check;
	bool InHand_Check;
	bool BeforeHand_first;

	CvCapture *Cam;										//ķ
	IplImage *img_Cam;								//���� �̹���
	IplImage *img_Chess;							//ó���� ���ɿ��� �� RGB �̹���
	IplImage *img_Skin;								//������ ��� �̹���
	IplImage *prev_img;								//�������� ���� �ʿ��� �̹���
	IplImage *img_sub;								//������ �Կ����� 
	CvRect ImgProcess_ROI;						//���ɿ��� ũ��

	vector<Chess_point> cross_point;
	vector<int> piece_idx;

	static void MouseCallback_SetROI(int event, int x, int y, int flags, void *param);
	
	void Inter_imageCraete(int roi_width, int roi_height);													//���꿡 �ʿ��� �̹��� �Ҵ�
	void Sub_image(IplImage *src1, IplImage *src2, IplImage *dst);									//������ ����
	void Compose_diffImage(IplImage *rgb, IplImage *bin, CvScalar RGB);							//������ ��� �̹����� RGB �� �����
	bool Check_InChessboard(IplImage *img, vector<Chess_point> point);							//binary image�� ü������ �ȿ� �ȼ��� �������� �˻�
	bool Check_imgZero(IplImage *img);																							//img�� �ȼ����� �ƹ��͵� ������ �ʴ��� üũ
	CvPoint	Get_Chessidx(CvPoint point, vector<Chess_point> cross_point);										//���� ��� ü���ǿ� �ִ����� üũ
	float area_tri(CvPoint p, CvPoint q, CvPoint r);
};

