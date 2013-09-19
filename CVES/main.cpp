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

#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include <Windows.h>
#include <time.h>

#include "Hand_recognition.hpp"
#include "Chess_recognition.hpp"
#include "BlobLabeling.hpp"
#include "chess_game.hpp"

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
	CvCapture *Cam;
	IplImage *img_Skin = cvCreateImage(cvSize(ROI_WIDTH, ROI_HEIGHT), IPL_DEPTH_8U, 1);
	IplImage *prev_img = cvCreateImage(cvSize(ROI_WIDTH, ROI_HEIGHT), IPL_DEPTH_8U, 3);
	IplImage *img_sub = cvCreateImage(cvSize(ROI_WIDTH, ROI_HEIGHT), IPL_DEPTH_8U, 1);
	IplImage *img_canny = cvCreateImage(cvSize(ROI_WIDTH, ROI_HEIGHT), IPL_DEPTH_8U, 1);

	char buf[32];
	//bool Hand_check[2];
	bool Sub_check = false;
	bool InHand_Check = false;
	time_t sec_time = 1;

	vector<std::pair<float, float>> CH_LineX, CH_LineY;
	vector<Chess_point> CP;
	vector<int> piece_idx;

	Hand_recognition Find_Hand;
	Chess_recognition Find_Chess;
	CBlobLabeling CBlob;
	chess_game CHESS_GAME;

	cvNamedWindow("RGB");

	//Cam init
	Cam = cvCaptureFromCAM(0);
	if(Cam != NULL){
		cvSetCaptureProperty(Cam, CV_CAP_PROP_FRAME_WIDTH, WIDTH);
		cvSetCaptureProperty(Cam, CV_CAP_PROP_FRAME_HEIGHT, HEIGHT);
	}

	//Image alloc
	img_Chess = cvCreateImage(cvSize(ROI_WIDTH, ROI_HEIGHT), IPL_DEPTH_8U, 3);

	Find_Hand.Init(ROI_WIDTH, ROI_HEIGHT);
	Find_Chess.Init(ROI_WIDTH, ROI_HEIGHT, RECOGNITION_MODE);

	///판 자리잡기
	while(1){
		img_Cam = cvQueryFrame(Cam);
		if(img_Cam == NULL){
			printf("Camera Disconnect!\n");
			break;
		}
		//cvFlip(img_Cam, img_Cam, -1);

		cvSetImageROI(img_Cam, cvRect(80,0,480,480));
		cvCopy(img_Cam, img_Chess);
		Find_Chess.Copy_Img(img_Chess);

		//Find_Chess.Get_Line(&CH_LineX, &CH_LineY);
		//Find_Chess.drawLines(CH_LineX, img_Cam);
		//Find_Chess.drawLines(CH_LineY, img_Cam);
		//Find_Chess.findIntersections(CH_LineX, CH_LineY, &CP);
		//Find_Chess.Refine_CrossPoint(&CP);
		//Find_Chess.drawPoint(img_Cam, CP);

		///*Find_Chess.Chess_recognition_process(&CP);
		//Find_Chess.Refine_CrossPoint(&CP);
		//Find_Chess.drawPoint(img_Cam, CP);*/
		Find_Chess.Chess_recog_wrapper(img_Cam, &CP);

		cvResetImageROI(img_Cam);
		cvDrawRect(img_Cam, cvPoint(80,0), cvPoint(560, 480), cvScalar(255));
		cvPutText(img_Cam, "If U want start, Press 'ESC'", cvPoint(30, 30), &cvFont(2.0), cvScalar(0,0,255));
		cvShowImage("RGB", img_Cam);

		if(cvWaitKey(10) == 27)		break;
	}

	while(1){
		int tick = GetTickCount();

		//캠으로부터 이미지 받아오기
		img_Cam = cvQueryFrame(Cam);
		if(img_Cam == NULL){
			printf("Camera Disconnect!\n");
			break;
		}

		//관심영역 설정
		cvSetImageROI(img_Cam, cvRect(80,0,480,480));
		cvCopy(img_Cam, img_Chess);
		Find_Chess.Copy_Img(img_Chess);

///////////////////////////////////////////////////////////////////////////////////////체스판 동작 여부 체크/////////////////////////////////////////////////////////////////////////////////////////////
		
		/*//손이 있을 경우
		Find_Hand.Detect_Skin(img_Chess, img_Skin);
		//Find_Hand.Sub_prevFrame(img_Chess, img_Skin);
		Hand_check[0] = Find_Hand.is_Hand(img_Skin);

		//손이 있을 경우
		if(Hand_check[0] == true){
			printf("Hand found!\n");
			//이전 프레임에서는 손이 발견되지 않았을 경우
			if(Hand_check[1] == false){
				cvCopy(img_Cam, prev_img);
				cvShowImage("PREV", prev_img);
			}
			Hand_check[1] = true;
		}
		//손이 없을 경우
		else{
			printf("XX\n");
			//이전 프레임에서 손이 발견됬을때
			if(Hand_check[1] == true){
				//차영상 진행
				Sub_image(img_Chess, prev_img, img_sub);
				cvShowImage("after", img_Chess);
				cvShowImage("SUB", img_sub);
			}
			//이전 프레임도 손이 미발견
			else{
				Find_Chess.Get_Line(&CH_LineX, &CH_LineY);
				//Find_Chess.drawLines(CH_LineX, img_Cam);
				//Find_Chess.drawLines(CH_LineY, img_Cam);
				Find_Chess.findIntersections(CH_LineX, CH_LineY, &CP);
				Find_Chess.Refine_CrossPoint(&CP);
				cvShowImage("LINE", img_Chess);
			}
			cvCopy(img_Cam, prev_img);
			Hand_check[1] = false;
		}*/
		if(CP.size() != 81){								//초기화
			///*Find_Chess.Get_Line(&CH_LineX, &CH_LineY);
			//Find_Chess.drawLines(CH_LineX, img_Cam);
			//Find_Chess.drawLines(CH_LineY, img_Cam);
			//Find_Chess.findIntersections(CH_LineX, CH_LineY, &CP);
			//Find_Chess.Refine_CrossPoint(&CP);*/
			//Find_Chess.Chess_recognition_process(&CP);
			//Find_Chess.Refine_CrossPoint(&CP);
			Find_Chess.Chess_recog_wrapper(img_Cam, &CP);
		}else if(CP.size() == 81){						//초기화 이후
			//time_t time_sub;
			//time(&time_sub);
			//time_sub -= sec_time;
			if(Sub_check == false /*&& time_sub >= 1*/){
				Find_Hand.Sub_prevFrame(img_Chess, img_Skin);
				if(Check_InChessboard(img_Skin, CP)){
					cvCopy(img_Cam, prev_img);
					cvShowImage("PREV", prev_img);
					//cvShowImage("PREV_SUB", img_Skin);
					printf("PREV Catch\n");
					Sub_check = true;
				}else{
					/*Find_Chess.Get_Line(&CH_LineX, &CH_LineY);
					Find_Chess.drawLines(CH_LineX, img_Cam);
					Find_Chess.drawLines(CH_LineY, img_Cam);
					Find_Chess.findIntersections(CH_LineX, CH_LineY, &CP);
					Find_Chess.Refine_CrossPoint(&CP);*/
					Find_Chess.Chess_recog_wrapper(img_Cam, &CP);
				}
			}else{ //추후 해야할 작업 : 빠질때 어떻게 작업할 것인가
				Find_Hand.Sub_prevFrame(img_Chess, img_sub);

				cvShowImage("유레카1", img_Chess);
				Sub_image(prev_img, img_Chess, img_Skin);
				//모폴로지 연산에 의한 노이즈 제거
				/*cvSmooth(img_Skin, img_Skin, CV_MEDIAN, 3, 3);*/
				//cvDilate(img_Skin, img_Skin, 0, 4);
				//cvErode(img_Skin, img_Skin, 0, 5);
				///*cvDilate(img_Skin, img_Skin, 0, 2);*/
				cvErode(img_Skin, img_Skin, 0, 6);
				cvDilate(img_Skin, img_Skin, 0, 6);
				cvErode(img_Skin, img_Skin, 0, 3);
				Compose_diffImage(img_Chess, img_Skin, cvScalar(0,255,255));
				
				//BlobLabeling
				CBlob.SetParam(img_Skin, 1);
				CBlob.DoLabeling();
				CBlob.DrawLabel(img_Chess, cvScalar(255,0,255));

				//손판정
				CBlob.GetSideBlob(img_Skin, &piece_idx);
				Compose_diffImage(img_Chess, img_Skin, cvScalar(100,100,255));
				cvDilate(img_Skin, img_Skin, 0, 5);
				if(Check_InChessboard(img_Skin, CP)){						//img_Skin은 손 추정물체만 남긴 이미지
					//printf("IN HAND!\n");
					InHand_Check = true;
				}else if(InHand_Check == true){                             //손이 없어졌을 때의 처리
					//이동 처리부
					printf("이동 ");

					//다음턴 준비부 -> 조건을 추가로 줘서 예외를 막아야함
					if(Check_imgZero(img_sub)){								//전제 조건은 손이 빠질때는 중간에 멈추지 않음
						printf("ZERO\n");
						InHand_Check = false;
						Sub_check = false;

						///////////////////////////////말 좌표 반환//////////////////////////////////////////////////////
						CvPoint Left, Right;
						CvRect temp;
						if(piece_idx.size() > 2){							//노이즈가 잡혔을때 -> 크기 큰걸로 정렬
							int first_idx, second_inx;
							int MAX_SIZE;
							for(int j = 0; j < 2; j++){
								MAX_SIZE = -1;
								for(int k = 0; k < piece_idx.size(); k++){
									temp = CBlob.m_recBlobs[piece_idx.at(k)];
									if(MAX_SIZE < temp.height * temp.width){
										MAX_SIZE = temp.height * temp.width;

										if(j == 0)		first_idx = k;
										else if(j == 1 && k != first_idx)		second_inx = k;
									}
								}
							}

							temp = CBlob.m_recBlobs[piece_idx.at(0)];
							Left = Get_Chessidx(cvPoint(temp.x+temp.width/2,temp.y+temp.height/3*2), CP);
							cvCircle(img_Chess, cvPoint(temp.x+temp.width/2,temp.y+temp.height/3*2), 3, cvScalar(0,0,255));
							temp = CBlob.m_recBlobs[piece_idx.at(1)];
							Right = Get_Chessidx(cvPoint(temp.x+temp.width/2,temp.y+temp.height/3*2), CP);
							cvCircle(img_Chess, cvPoint(temp.x+temp.width/2,temp.y+temp.height/3*2), 3, cvScalar(0,0,255));
							printf("(%d,%d), (%d,%d)\n",Left.x, Left.y, Right.x, Right.y);
							
							CHESS_GAME.Chess_process(Left, Right);
							CHESS_GAME.Show_chess_board();
						}
						else if(piece_idx.size() == 1){						//겹쳐버렸을때
							temp = CBlob.m_recBlobs[piece_idx.at(0)];
							Left = Get_Chessidx(cvPoint(temp.x+temp.width/2, temp.y+temp.height/2+temp.height/3*2), CP);
							cvCircle(img_Chess, cvPoint(temp.x+temp.width/2, temp.y+temp.height/2+temp.height/3*2), 3, cvScalar(0,0,255)); 
							Right = Get_Chessidx(cvPoint(temp.x+temp.width/2, temp.y+temp.height/3), CP);
							cvCircle(img_Chess, cvPoint(temp.x+temp.width/2, temp.y+temp.height/3), 3, cvScalar(0,0,255));
							printf("(%d,%d), (%d,%d)\n",Left.x, Left.y, Right.x, Right.y);
							
							CHESS_GAME.Chess_process(Left, Right);
							CHESS_GAME.Show_chess_board();
						}
						else if(piece_idx.size() == 2){						//딱 두개 추적
							temp = CBlob.m_recBlobs[piece_idx.at(0)];
							Left = Get_Chessidx(cvPoint(temp.x+temp.width/2,temp.y+temp.height/3*2), CP);
							cvCircle(img_Chess, cvPoint(temp.x+temp.width/2,temp.y+temp.height/3*2), 3, cvScalar(0,0,255));
							temp = CBlob.m_recBlobs[piece_idx.at(1)];
							Right = Get_Chessidx(cvPoint(temp.x+temp.width/2,temp.y+temp.height/3*2), CP);
							cvCircle(img_Chess, cvPoint(temp.x+temp.width/2,temp.y+temp.height/3*2), 3, cvScalar(0,0,255));
							printf("(%d,%d), (%d,%d)\n",Left.x, Left.y, Right.x, Right.y);
							
							CHESS_GAME.Chess_process(Left, Right);
							CHESS_GAME.Show_chess_board();
						}
						/////////////////////////////////////////////////////////////////////////////////////////////////
					}
				}
				cvShowImage("compose_diff", img_Chess);
			}
		}

		//좌표그리기
		Find_Chess.drawPoint(img_Cam, CP);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if(cvWaitKey(10) == 27)		break;

		tick = GetTickCount() - tick;
		sprintf(buf, "%.2f fps", 1000.f/ (float)tick);
		cvPutText(img_Cam, buf, cvPoint(30, 30), &cvFont(1.0), cvScalar(0,0,255));

		cvResetImageROI(img_Cam);
		cvDrawRect(img_Cam, cvPoint(80,0), cvPoint(560, 480), cvScalar(255));

		cvShowImage("RGB", img_Cam);
		cvShowImage("SKin", img_Skin);
	}

	cvReleaseCapture(&Cam);

	cvReleaseImage(&img_sub);
	cvReleaseImage(&prev_img);
	cvReleaseImage(&img_Skin);
	cvReleaseImage(&img_Chess);

	cvDestroyAllWindows();

	return 0;
}