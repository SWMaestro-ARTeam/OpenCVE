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

	IplImage *img_YCrCb, *img_HSV;				//YCrCb, HSV 색상계
	IplImage *prev_ground, *present_ground;			//차영상을 통해 차이를 구할 예정

public:
	Hand_recognition(void);
	~Hand_recognition(void);

	//초기화
	void Init(int width, int height);
	//피부색 Detection
	void Detect_Skin(IplImage *src, IplImage *dst);
	//손검출 판단 - 면적 기반
	bool is_Hand(IplImage *src);
	//이전 프레임과의 차영상
	void Sub_prevFrame(IplImage *src, IplImage *dst);
	//차영상 초기화
	void Init_diff();
};

