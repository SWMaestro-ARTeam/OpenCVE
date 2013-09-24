#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include <Windows.h>
#include <time.h>

#include "Hand_recognition.hpp"
#include "Chess_recognition.hpp"
#include "BlobLabeling.hpp"
#include "chess_game.hpp"
#include "Img_Process.hpp"

#define WIDTH 640
#define HEIGHT 480
#define ROI_WIDTH	480
#define ROI_HEIGHT	480
#define SUB_THRESHOLD 15
#define RECOGNITION_MODE 2

//
IplImage *img_Cam;								//원본 이미지
IplImage *img_Chess;							//처리할 관심영역 속 RGB 이미지
IplImage *prev_img;
IplImage *img_sub;

float area_tri(CvPoint p, CvPoint q, CvPoint r){
	return (float)abs((p.x*q.y+q.x*r.y+r.x*p.y)-(p.y*q.x+q.y*r.x+r.y*p.x)) / 2.0;
}
void Sub_image(IplImage *src1, IplImage *src2, IplImage *dst){
	cvZero(dst);

	for(int i = 0; i < src1->width; i++){
		for(int j = 0; j < src1->height; j++){
			unsigned char SUB_B = abs((unsigned char)src1->imageData[(i * 3) + j * src1->widthStep] - (unsigned char)src2->imageData[(i * 3) + j * src2->widthStep]);
			unsigned char SUB_G = abs((unsigned char)src1->imageData[(i * 3) + j * src1->widthStep + 1] - (unsigned char)src2->imageData[(i * 3) + j * src2->widthStep + 1]);
			unsigned char SUB_R = abs((unsigned char)src1->imageData[(i * 3) + j * src1->widthStep + 2] - (unsigned char)src2->imageData[(i * 3) + j * src2->widthStep + 2]);

			if(SUB_B > SUB_THRESHOLD || SUB_G > SUB_THRESHOLD || SUB_R > SUB_THRESHOLD){
				dst->imageData[i + j * dst->widthStep] = (unsigned char)255;
				/*UCHAR value = SUB_B + SUB_G + SUB_R;
				if(value > 255)		value = 255;
				dst->imageData[i + j * dst->widthStep] = (unsigned char)value;*/
			}
		}
	}
}
bool Check_InChessboard(IplImage *img, vector<Chess_point> point){
	CvPoint LH, LL, RH, RL;			//왼쪽 위, 왼쪽 아래, 오른쪽 위 오른쪽 아래
	float tArea,t1Area, t2Area, t3Area, t4Area;
	
	LH = point.at(0).Cordinate;
	RH = point.at(8).Cordinate;
	LL = point.at(72).Cordinate;
	RL = point.at(80).Cordinate;
	tArea = area_tri(LH, LL, RH) + area_tri(RH, RL, LL);

	for(int i = 0; i < img->width; i++){
		for(int j = 0; j < img->height; j++){
			UCHAR pixel_value = (UCHAR)img->imageData[i + j * img->widthStep];
			
			if(pixel_value == 255){

				t1Area = area_tri(cvPoint(i,j), LH, RH);
				t2Area = area_tri(cvPoint(i,j), RH, RL);
				t3Area = area_tri(cvPoint(i,j), RL, LL);
				t4Area = area_tri(cvPoint(i,j), LL, LH);

				float totalArea = t1Area+t2Area+t3Area+t4Area;
				if(fabs(tArea-totalArea) < 5)
					return true;
			}
		}
	}

	return false;
}
void Compose_diffImage(IplImage *rgb, IplImage *bin, CvScalar RGB){
	unsigned char bin_value;
	for(int i = 0; i < rgb->width; i++)
		for(int j = 0; j < rgb->height; j++){
			bin_value = (unsigned char)bin->imageData[i + j * bin->widthStep];
			if(bin_value == 255){
				rgb->imageData[i*3 + j * rgb->widthStep] = RGB.val[0];
				rgb->imageData[i*3 + j * rgb->widthStep + 1] = RGB.val[1];
				rgb->imageData[i*3 + j * rgb->widthStep + 2] = RGB.val[2];
			}
		}
}
bool Check_imgZero(IplImage *img){
	unsigned char pixel_value;
	for(int i = 0; i < img->width; i++)
		for(int j = 0; j < img->height; j++){
			pixel_value = (unsigned char)img->imageData[i + j * img->widthStep];
			if(pixel_value != 0)
				return false;
		}

	return true;
}
CvPoint Get_Chessidx(CvPoint point, vector<Chess_point> CP){
	for(int i = 0; i < CP.size() - 10; i++){
		if(CP.at(i).Cordinate.x <= point.x && CP.at(i).Cordinate.y <= point.y){
			if(CP.at(i+10).Cordinate.x > point.x && CP.at(i+10).Cordinate.y > point.y)
				return CP.at(i).index;
		}
	}
	return cvPoint(-1,-1);
}

int main(){

	Img_Process IP;
	IP.Init_process();
	while(1){
		IP.Do_imgprocess();

		if(IP.Check_Exit())	break;
	}

	IP.Exit_imgProcess();

	return 0;
}