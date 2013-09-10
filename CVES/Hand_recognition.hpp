#pragma once
#include <iostream>
#include <cv.h>
#include <Windows.h>

#define MOP_NUM 2
#define T_SKIN_NUM 10
#define SUB_THRESHOLD 5

class Hand_recognition
{
private:
	int img_width, img_height;

	unsigned char B,G,R;
	unsigned char Y,Cr,Cb;
	unsigned char H,S,V;

	bool R1(int R, int G, int B);
	bool R2(float Y, float Cr, float Cb);
	bool R3(float H, float S, float V);

	IplImage *img_YCrCb, *img_HSV;				//YCrCb, HSV �����
	IplImage *prev_ground, *present_ground;			//�������� ���� ���̸� ���� ����

public:
	Hand_recognition(void);
	~Hand_recognition(void);

	//�ʱ�ȭ
	void Init(int width, int height);
	//�Ǻλ� Detection
	void Detect_Skin(IplImage *src, IplImage *dst);
	//�հ��� �Ǵ� - ���� ���
	bool is_Hand(IplImage *src);
	//���� �����Ӱ��� ������
	void Sub_prevFrame(IplImage *src, IplImage *dst);
	//������ �ʱ�ȭ
	void Init_diff();
};

