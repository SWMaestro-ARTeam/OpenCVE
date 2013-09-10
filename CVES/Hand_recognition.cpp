#include "Hand_recognition.hpp"


Hand_recognition::Hand_recognition(void)
{
}


Hand_recognition::~Hand_recognition(void)
{
	cvReleaseImage(&img_YCrCb);
	cvReleaseImage(&img_HSV);

	if(prev_ground != NULL)
		cvReleaseImage(&prev_ground);
	if(present_ground != NULL);
	cvReleaseImage(&present_ground);
}

void Hand_recognition::Init(int width, int height){
	img_width = width;
	img_height = height;

	img_YCrCb = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
	img_HSV = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);

	prev_ground = NULL;
	present_ground = NULL;
}

bool Hand_recognition::R1(int R, int G, int B) {
    bool e1 = (R>95) && (G>40) && (B>20) && ((max(R,max(G,B)) - min(R, min(G,B)))>15) && (abs(R-G)>15) && (R>G) && (R>B);
    bool e2 = (R>220) && (G>210) && (B>170) && (abs(R-G)<=15) && (R>B) && (G>B);
    return (e1||e2);
}
 
bool Hand_recognition::R2(float Y, float Cr, float Cb) {
    bool e3 = Cr <= 1.5862*Cb+20;
    bool e4 = Cr >= 0.3448*Cb+76.2069;
    bool e5 = Cr >= -4.5652*Cb+234.5652;
    bool e6 = Cr <= -1.15*Cb+301.75;
    bool e7 = Cr <= -2.2857*Cb+432.85;
    return e3 && e4 && e5 && e6 && e7;
}
 
bool Hand_recognition::R3(float H, float S, float V) {
    return (H<25) || (H > 230);
}

void Hand_recognition::Detect_Skin(IplImage *src, IplImage *dst){

	cvCvtColor(src, img_YCrCb, CV_BGR2YCrCb);
	cvCvtColor(src, img_HSV, CV_BGR2HSV);

	cvZero(dst);

	for(int i = 0; i < dst->height; i++){
		for(int j = 0; j < dst->width; j++){

			B = (unsigned char)src->imageData[(j * 3) + i * src->widthStep];
			G = (unsigned char)src->imageData[(j * 3) + i * src->widthStep + 1];
			R = (unsigned char)src->imageData[(j * 3) + i * src->widthStep + 2];

			bool a = R1(R,G,B);

			if(a){
				H = (unsigned char)img_HSV->imageData[(j * 3) + i * img_HSV->widthStep];
				S = (unsigned char)img_HSV->imageData[(j * 3) + i * img_HSV->widthStep + 1];
				V = (unsigned char)img_HSV->imageData[(j * 3) + i * img_HSV->widthStep + 2];

				bool c = R3(H,S,V);;

				if(c){
					Y = (unsigned char)img_YCrCb->imageData[(j * 3) + i * img_YCrCb->widthStep];
					Cr = (unsigned char)img_YCrCb->imageData[(j * 3) + i * img_YCrCb->widthStep + 1];
					Cb = (unsigned char)img_YCrCb->imageData[(j * 3) + i * img_YCrCb->widthStep + 2];

					bool b = R2(Y,Cr,Cb);

					if(b)
						dst->imageData[j + i * dst->widthStep] = (unsigned char) 255;
				}

			}
		}
	}

	cvErode(dst, dst, 0, MOP_NUM);
	cvDilate(dst, dst, 0, MOP_NUM);
}

bool Hand_recognition::is_Hand(IplImage *src){
	int count = 0;

	for(int i = 0; i < src->width; i++){
		for(int j = 0; j < src->height; j++){
			if((unsigned char)src->imageData[i + j * src->widthStep] == 255)
				count++;
		}
	}

	if(count > T_SKIN_NUM)
		return true;

	return false;
}

void Hand_recognition::Sub_prevFrame(IplImage *src, IplImage *dst){
	static int frame_count = 0;

	if(prev_ground == NULL){
		prev_ground = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
		present_ground = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
		cvCvtColor(src, prev_ground, CV_BGR2GRAY);
	}

	cvZero(dst);

	if(frame_count == 1){
		cvCvtColor(src, present_ground, CV_BGR2GRAY);
		for(int i = 0; i < src->width; i++){
			for(int j = 0; j < src->height; j++){
				int pixel_sub = abs((unsigned char)prev_ground->imageData[i + j * prev_ground->widthStep] - (unsigned char)present_ground->imageData[i + j * present_ground->widthStep]);
				if(pixel_sub > SUB_THRESHOLD)
					dst->imageData[i + j * dst->widthStep] = (unsigned char)255;
			}
		}

		//prev_ground = cvCloneImage(present_ground);
		cvCopy(present_ground, prev_ground);

		cvSmooth(dst, dst, CV_MEDIAN,3,3);
		cvErode(dst, dst, 0, 5);
		cvDilate(dst, dst, 0, 20);

		frame_count = 0;
	}

	frame_count++;
}

void Hand_recognition::Init_diff(){
	cvReleaseImage(&prev_ground);
	cvReleaseImage(&present_ground);
}