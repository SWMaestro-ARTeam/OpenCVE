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
IplImage *img_Cam;								//���� �̹���
IplImage *img_Chess;							//ó���� ���ɿ��� �� RGB �̹���
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
	CvPoint LH, LL, RH, RL;			//���� ��, ���� �Ʒ�, ������ �� ������ �Ʒ�
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

	///�� �ڸ����
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

		//ķ���κ��� �̹��� �޾ƿ���
		img_Cam = cvQueryFrame(Cam);
		if(img_Cam == NULL){
			printf("Camera Disconnect!\n");
			break;
		}

		//���ɿ��� ����
		cvSetImageROI(img_Cam, cvRect(80,0,480,480));
		cvCopy(img_Cam, img_Chess);
		Find_Chess.Copy_Img(img_Chess);

///////////////////////////////////////////////////////////////////////////////////////ü���� ���� ���� üũ/////////////////////////////////////////////////////////////////////////////////////////////
		
		/*//���� ���� ���
		Find_Hand.Detect_Skin(img_Chess, img_Skin);
		//Find_Hand.Sub_prevFrame(img_Chess, img_Skin);
		Hand_check[0] = Find_Hand.is_Hand(img_Skin);

		//���� ���� ���
		if(Hand_check[0] == true){
			printf("Hand found!\n");
			//���� �����ӿ����� ���� �߰ߵ��� �ʾ��� ���
			if(Hand_check[1] == false){
				cvCopy(img_Cam, prev_img);
				cvShowImage("PREV", prev_img);
			}
			Hand_check[1] = true;
		}
		//���� ���� ���
		else{
			printf("XX\n");
			//���� �����ӿ��� ���� �߰߉�����
			if(Hand_check[1] == true){
				//������ ����
				Sub_image(img_Chess, prev_img, img_sub);
				cvShowImage("after", img_Chess);
				cvShowImage("SUB", img_sub);
			}
			//���� �����ӵ� ���� �̹߰�
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
		if(CP.size() != 81){								//�ʱ�ȭ
			///*Find_Chess.Get_Line(&CH_LineX, &CH_LineY);
			//Find_Chess.drawLines(CH_LineX, img_Cam);
			//Find_Chess.drawLines(CH_LineY, img_Cam);
			//Find_Chess.findIntersections(CH_LineX, CH_LineY, &CP);
			//Find_Chess.Refine_CrossPoint(&CP);*/
			//Find_Chess.Chess_recognition_process(&CP);
			//Find_Chess.Refine_CrossPoint(&CP);
			Find_Chess.Chess_recog_wrapper(img_Cam, &CP);
		}else if(CP.size() == 81){						//�ʱ�ȭ ����
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
			}else{ //���� �ؾ��� �۾� : ������ ��� �۾��� ���ΰ�
				Find_Hand.Sub_prevFrame(img_Chess, img_sub);

				cvShowImage("����ī1", img_Chess);
				Sub_image(prev_img, img_Chess, img_Skin);
				//�������� ���꿡 ���� ������ ����
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

				//������
				CBlob.GetSideBlob(img_Skin, &piece_idx);
				Compose_diffImage(img_Chess, img_Skin, cvScalar(100,100,255));
				cvDilate(img_Skin, img_Skin, 0, 5);
				if(Check_InChessboard(img_Skin, CP)){						//img_Skin�� �� ������ü�� ���� �̹���
					//printf("IN HAND!\n");
					InHand_Check = true;
				}else if(InHand_Check == true){                             //���� �������� ���� ó��
					//�̵� ó����
					printf("�̵� ");

					//������ �غ�� -> ������ �߰��� �༭ ���ܸ� ���ƾ���
					if(Check_imgZero(img_sub)){								//���� ������ ���� �������� �߰��� ������ ����
						printf("ZERO\n");
						InHand_Check = false;
						Sub_check = false;

						///////////////////////////////�� ��ǥ ��ȯ//////////////////////////////////////////////////////
						CvPoint Left, Right;
						CvRect temp;
						if(piece_idx.size() > 2){							//����� �������� -> ũ�� ū�ɷ� ����
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
						else if(piece_idx.size() == 1){						//���Ĺ�������
							temp = CBlob.m_recBlobs[piece_idx.at(0)];
							Left = Get_Chessidx(cvPoint(temp.x+temp.width/2, temp.y+temp.height/2+temp.height/3*2), CP);
							cvCircle(img_Chess, cvPoint(temp.x+temp.width/2, temp.y+temp.height/2+temp.height/3*2), 3, cvScalar(0,0,255)); 
							Right = Get_Chessidx(cvPoint(temp.x+temp.width/2, temp.y+temp.height/3), CP);
							cvCircle(img_Chess, cvPoint(temp.x+temp.width/2, temp.y+temp.height/3), 3, cvScalar(0,0,255));
							printf("(%d,%d), (%d,%d)\n",Left.x, Left.y, Right.x, Right.y);
							
							CHESS_GAME.Chess_process(Left, Right);
							CHESS_GAME.Show_chess_board();
						}
						else if(piece_idx.size() == 2){						//�� �ΰ� ����
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

		//��ǥ�׸���
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