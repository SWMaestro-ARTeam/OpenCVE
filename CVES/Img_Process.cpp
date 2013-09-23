#include "Img_Process.hpp"


Img_Process::Img_Process(void)
{
}


Img_Process::~Img_Process(void)
{
}

void Img_Process::MouseCallback_SetROI(int event, int x, int y, int flags, void *param){
	Img_Process* p = (Img_Process*)param;
	static bool down_check = false;

	if(p->ImgProcess_Mode == 0){
		switch(event){
		case CV_EVENT_LBUTTONDOWN:
			p->ImgProcess_ROI.x = x;
			p->ImgProcess_ROI.y = y;
			down_check = true;
			break;
		case CV_EVENT_LBUTTONUP:
			p->ImgProcess_Mode++;
			down_check = false;
			break;
		case CV_EVENT_MOUSEMOVE:
			if(down_check == true){
				p->ImgProcess_ROI.width = x - p->ImgProcess_ROI.x;
				p->ImgProcess_ROI.height = y - p->ImgProcess_ROI.y;
			}
			break;
		}
	}
}

bool Img_Process::Check_Exit(){
	if(ImgProcess_Mode == 3)		return true;
	else							return false;
}

void Img_Process::Init_process(){
	//Cam init
	Cam = cvCaptureFromCAM(0);
	if(Cam != NULL){
		cvSetCaptureProperty(Cam, CV_CAP_PROP_FRAME_WIDTH, WIDTH);
		cvSetCaptureProperty(Cam, CV_CAP_PROP_FRAME_HEIGHT, HEIGHT);
	}
	
	//모드 초기화
	ImgProcess_Mode = 0;

	//window 생성 & 마우스 콜백 붙이기
	cvNamedWindow("CVES");
	cvSetMouseCallback("CVES", MouseCallback_SetROI, this);

	ImgProcess_ROI = cvRect(0,0,0,0);

	Sub_check				= false;
	InHand_Check		= false;
}

void Img_Process::Do_imgprocess(){
	char key_wait, buf[32];
	static bool img_createCheck = false;

	img_Cam = cvQueryFrame(Cam);

	switch(ImgProcess_Mode){
	case 0:							//관심영역 설정부
		if(img_createCheck == true){ cvReleaseImage(&img_Chess), img_createCheck = false;}
		cvDrawRect(img_Cam, cvPoint(ImgProcess_ROI.x, ImgProcess_ROI.y), cvPoint(ImgProcess_ROI.x + ImgProcess_ROI.width, ImgProcess_ROI.y + ImgProcess_ROI.height), cvScalar(255), 2);
		cvShowImage("CVES", img_Cam);
		cvWaitKey(33);
		break;
	case 1:							//관심영역 재설정 선택 OR 체스보드 인식 확인부
		if(img_createCheck == false){
			img_Chess = cvCreateImage(cvSize(ImgProcess_ROI.width, ImgProcess_ROI.height), IPL_DEPTH_8U, 3);
			img_createCheck = true;
			Find_Chess.Init(ImgProcess_ROI.width, ImgProcess_ROI.height, RECOGNITION_MODE);
			Find_Hand.Init(ImgProcess_ROI.width, ImgProcess_ROI.height);

			//연산에 필요한 이미지 할당
			Inter_imageCraete(ImgProcess_ROI.width, ImgProcess_ROI.height);
		}
		cvSetImageROI(img_Cam, ImgProcess_ROI);
		cvCopy(img_Cam, img_Chess);

		//Chessboard recognition;
		Find_Chess.Copy_Img(img_Chess);
		Find_Chess.Chess_recog_wrapper(img_Cam, &CP);

		key_wait = cvWaitKey(10);
		printf("%d, %d\n", key_wait, ImgProcess_Mode);
		if(key_wait == 27)					ImgProcess_Mode++;
		else if(key_wait != -1)				ImgProcess_Mode+=2;

		cvResetImageROI(img_Cam);
		cvDrawRect(img_Cam, cvPoint(ImgProcess_ROI.x, ImgProcess_ROI.y), cvPoint(ImgProcess_ROI.x + ImgProcess_ROI.width, ImgProcess_ROI.y + ImgProcess_ROI.height), cvScalar(255), 2);
		cvPutText(img_Cam, "If U want start, Press 'ESC' else Press Any Key", cvPoint(30, 30), &cvFont(1.0), cvScalar(0,100,100));
		cvShowImage("CVES", img_Cam);
		break;
	case 2:							//실제 이미지 처리 실행부
		{
			int tick = GetTickCount();

			cvSetImageROI(img_Cam, ImgProcess_ROI);
			cvCopy(img_Cam, img_Chess);

			//Chessboard recognition;
			Find_Chess.Copy_Img(img_Chess);

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////차영상 및 조건판정 부분///////////////////////////////////////////////
			if(CP.size() != 81){								//초기화			
				Find_Chess.Chess_recog_wrapper(img_Cam, &CP);
			}else if(CP.size() == 81){
				if(Sub_check == false /*&& time_sub >= 1*/){
					Find_Hand.Sub_prevFrame(img_Chess, img_Skin);
					if(Check_InChessboard(img_Skin, CP)){
						cvCopy(img_Cam, prev_img);
#ifdef DEBUG
						cvShowImage("PREV", prev_img);
						printf("PREV Catch\n");
#endif
						Sub_check = true;
					}else{
						Find_Chess.Chess_recog_wrapper(img_Cam, &CP);
					}
				}else{ //추후 해야할 작업 : 빠질때 어떻게 작업할 것인가
					Find_Hand.Sub_prevFrame(img_Chess, img_sub);

#ifdef DEBUG
					cvShowImage("유레카1", img_Chess);
#endif
					Sub_image(prev_img, img_Chess, img_Skin);
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
						InHand_Check = true;
					}else if(InHand_Check == true){
						//이동 처리부
						printf("이동 ");

						//다음턴 준비부 -> 조건을 추가로 줘서 예외를 막아야함
						//if(Check_imgZero(img_sub)){								//전제 조건은 손이 빠질때는 중간에 멈추지 않음
						//	printf("ZERO\n");
						//	InHand_Check = false;
						//	Sub_check = false;

						//	///////////////////////////////말 좌표 반환//////////////////////////////////////////////////////
						//	CvPoint Left, Right;
						//	CvRect temp;
						//	if(piece_idx.size() > 2){							//노이즈가 잡혔을때 -> 크기 큰걸로 정렬
						//		int first_idx, second_inx;
						//		int MAX_SIZE;
						//		for(int j = 0; j < 2; j++){
						//			MAX_SIZE = -1;
						//			for(int k = 0; k < piece_idx.size(); k++){
						//				temp = CBlob.m_recBlobs[piece_idx.at(k)];
						//				if(MAX_SIZE < temp.height * temp.width){
						//					MAX_SIZE = temp.height * temp.width;

						//					if(j == 0)		first_idx = k;
						//					else if(j == 1 && k != first_idx)		second_inx = k;
						//				}
						//			}
						//		}

						//		temp = CBlob.m_recBlobs[piece_idx.at(0)];
						//		Left = Get_Chessidx(cvPoint(temp.x+temp.width/2,temp.y+temp.height/3*2), CP);
						//		cvCircle(img_Chess, cvPoint(temp.x+temp.width/2,temp.y+temp.height/3*2), 3, cvScalar(0,0,255));
						//		temp = CBlob.m_recBlobs[piece_idx.at(1)];
						//		Right = Get_Chessidx(cvPoint(temp.x+temp.width/2,temp.y+temp.height/3*2), CP);
						//		cvCircle(img_Chess, cvPoint(temp.x+temp.width/2,temp.y+temp.height/3*2), 3, cvScalar(0,0,255));
						//		printf("(%d,%d), (%d,%d)\n",Left.x, Left.y, Right.x, Right.y);

						//		CHESS_GAME.Chess_process(Left, Right);
						//		CHESS_GAME.Show_chess_board();
						//	}
						//	else if(piece_idx.size() == 1){						//겹쳐버렸을때
						//		temp = CBlob.m_recBlobs[piece_idx.at(0)];
						//		Left = Get_Chessidx(cvPoint(temp.x+temp.width/2, temp.y+temp.height/2+temp.height/3*2), CP);
						//		cvCircle(img_Chess, cvPoint(temp.x+temp.width/2, temp.y+temp.height/2+temp.height/3*2), 3, cvScalar(0,0,255)); 
						//		Right = Get_Chessidx(cvPoint(temp.x+temp.width/2, temp.y+temp.height/3), CP);
						//		cvCircle(img_Chess, cvPoint(temp.x+temp.width/2, temp.y+temp.height/3), 3, cvScalar(0,0,255));
						//		printf("(%d,%d), (%d,%d)\n",Left.x, Left.y, Right.x, Right.y);

						//		CHESS_GAME.Chess_process(Left, Right);
						//		CHESS_GAME.Show_chess_board();
						//	}
						//	else if(piece_idx.size() == 2){						//딱 두개 추적
						//		temp = CBlob.m_recBlobs[piece_idx.at(0)];
						//		Left = Get_Chessidx(cvPoint(temp.x+temp.width/2,temp.y+temp.height/3*2), CP);
						//		cvCircle(img_Chess, cvPoint(temp.x+temp.width/2,temp.y+temp.height/3*2), 3, cvScalar(0,0,255));
						//		temp = CBlob.m_recBlobs[piece_idx.at(1)];
						//		Right = Get_Chessidx(cvPoint(temp.x+temp.width/2,temp.y+temp.height/3*2), CP);
						//		cvCircle(img_Chess, cvPoint(temp.x+temp.width/2,temp.y+temp.height/3*2), 3, cvScalar(0,0,255));
						//		printf("(%d,%d), (%d,%d)\n",Left.x, Left.y, Right.x, Right.y);

						//		CHESS_GAME.Chess_process(Left, Right);
						//		CHESS_GAME.Show_chess_board();
						//	}
					}
				}
			}
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			key_wait = cvWaitKey(10);
			printf("%d, %d\n", key_wait, ImgProcess_Mode);
			if(key_wait == 27)					ImgProcess_Mode++;
			else if(key_wait != -1)				ImgProcess_Mode+=2;

			cvResetImageROI(img_Cam);

			cvDrawRect(img_Cam, cvPoint(ImgProcess_ROI.x, ImgProcess_ROI.y), cvPoint(ImgProcess_ROI.x + ImgProcess_ROI.width, ImgProcess_ROI.y + ImgProcess_ROI.height), cvScalar(255), 2);
			tick = GetTickCount() - tick;
			sprintf(buf, "%.2f fps", 1000.f/ (float)tick);
			cvPutText(img_Cam, buf, cvPoint(30, 30), &cvFont(1.0), cvScalar(0,0,255));

			if(cvWaitKey(10) == 27)	ImgProcess_Mode++;
			cvShowImage("CVES", img_Cam);
			break;
		}
	}
}

void Img_Process::Exit_imgProcess(){
	cvDestroyAllWindows();

	cvReleaseCapture(&Cam);

	cvReleaseImage(&img_Chess);
	cvReleaseImage(&img_Skin);
	cvReleaseImage(&prev_img);
	cvReleaseImage(&img_sub);
}

bool Img_Process::Check_InChessboard(IplImage *img, vector<Chess_point> point){
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

float Img_Process::area_tri(CvPoint p, CvPoint q, CvPoint r){
	return (float)abs((p.x*q.y+q.x*r.y+r.x*p.y)-(p.y*q.x+q.y*r.x+r.y*p.x)) / 2.0;
}

void Img_Process::Sub_image(IplImage *src1, IplImage *src2, IplImage *dst){
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

void Img_Process::Compose_diffImage(IplImage *rgb, IplImage *bin, CvScalar RGB){
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

void Img_Process::Inter_imageCraete(int roi_width, int roi_height){
	img_Skin = cvCreateImage(cvSize(roi_width, roi_height), IPL_DEPTH_8U, 1);
	prev_img = cvCreateImage(cvSize(roi_width, roi_height), IPL_DEPTH_8U, 3);
	img_sub = cvCreateImage(cvSize(roi_width, roi_height), IPL_DEPTH_8U, 1);
}