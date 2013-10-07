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

#include "EngineS.hpp"

EngineS::EngineS() {
}

EngineS::~EngineS() {
}

void EngineS::Engine_Initializing() {

}

void EngineS::Engine_DeInitializing() {

}

void EngineS::MouseCallback_SetROI(int event, int x, int y, int flags, void *param) {
	EngineS* p = (EngineS*)param;
	static bool down_check = false;

	if (p->ImgProcess_Mode == 0) {
		switch (event) {
			case CV_EVENT_LBUTTONDOWN:
				p->ImgProcess_ROI.x = x;
				p->ImgProcess_ROI.y = y;
				down_check = true;
				break;
			case CV_EVENT_LBUTTONUP:
				if (abs(p->ImgProcess_ROI.height * p->ImgProcess_ROI.width) > 200) {
					p->ImgProcess_Mode++;

					if (p->ImgProcess_ROI.width < 0) {
						p->ImgProcess_ROI.x += p->ImgProcess_ROI.width;
						p->ImgProcess_ROI.width *= -1;
					}
					if (p->ImgProcess_ROI.height < 0) {
						p->ImgProcess_ROI.y += p->ImgProcess_ROI.height;
						p->ImgProcess_ROI.height *= -1;
					}
				}

				down_check = false;
				break;
			case CV_EVENT_MOUSEMOVE:
				if (down_check == true) {
					p->ImgProcess_ROI.width = x - p->ImgProcess_ROI.x;
					p->ImgProcess_ROI.height = y - p->ImgProcess_ROI.y;
				}
				break;
		}
	}
}

bool EngineS::Check_Exit() {
	if (ImgProcess_Mode == 3)
		return true;
	else
		return false;
}

void EngineS::Init_process() {
	//Cam init
	Cam = cvCaptureFromCAM(0);
	if (Cam != NULL) {
		cvSetCaptureProperty(Cam, CV_CAP_PROP_FRAME_WIDTH, WIDTH);
		cvSetCaptureProperty(Cam, CV_CAP_PROP_FRAME_HEIGHT, HEIGHT);
	}
	
	//모드 초기화
	ImgProcess_Mode = 0;

	//window 생성 & 마우스 콜백 붙이기
	cvNamedWindow("CVES");
	cvSetMouseCallback("CVES", MouseCallback_SetROI, this);

	ImgProcess_ROI = cvRect(0, 0, 0, 0);

	Sub_check	= false;
	InHand_Check = false;
	BeforeHand_first = false;
}

void EngineS::Do_imgprocess(){
	char key_wait, buf[32];
	static bool img_createCheck = false;

	img_Cam = cvQueryFrame(Cam);

	switch (ImgProcess_Mode) {
		case 0:	
			//관심영역 설정부
			if (img_createCheck == true) {
				cvReleaseImage(&img_Chess);
				img_createCheck = false;
			}
			cvDrawRect(img_Cam, cvPoint(ImgProcess_ROI.x, ImgProcess_ROI.y), cvPoint(ImgProcess_ROI.x + ImgProcess_ROI.width, ImgProcess_ROI.y + ImgProcess_ROI.height), cvScalar(255), 2);
			cvShowImage("CVES", img_Cam);
			cvWaitKey(33);
			break;
		case 1:							//관심영역 재설정 선택 OR 체스보드 인식 확인부
			if (img_createCheck == false) {
				img_Chess = cvCreateImage(cvSize(ImgProcess_ROI.width, ImgProcess_ROI.height), IPL_DEPTH_8U, 3);
				temp_prev = cvCreateImage(cvSize(ImgProcess_ROI.width, ImgProcess_ROI.height), IPL_DEPTH_8U, 3);
				temp_prev2 = cvCreateImage(cvSize(ImgProcess_ROI.width, ImgProcess_ROI.height), IPL_DEPTH_8U, 3);
				other = cvCreateImage(cvSize(ImgProcess_ROI.width, ImgProcess_ROI.height), IPL_DEPTH_8U, 1);
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
			Find_Chess.Chess_recog_wrapper(img_Cam, &cross_point);
			cvResetImageROI(img_Cam);

			key_wait = cvWaitKey(10);
			if (key_wait == 27)
				ImgProcess_Mode++;
			else if (key_wait != -1)
				ImgProcess_Mode+=2;

			cvDrawRect(img_Cam, cvPoint(ImgProcess_ROI.x, ImgProcess_ROI.y), cvPoint(ImgProcess_ROI.x + ImgProcess_ROI.width, ImgProcess_ROI.y + ImgProcess_ROI.height), cvScalar(255), 2);
			cvPutText(img_Cam, "If U want start, Press 'ESC' else Press Any Key", cvPoint(30, 30), &cvFont(1.0), cvScalar(0, 100, 100));
			cvShowImage("CVES", img_Cam);
			break;
		case 2:							
			//실제 이미지 처리 실행부
			int tick = GetTickCount();

			cvSetImageROI(img_Cam, ImgProcess_ROI);
			cvCopy(img_Cam, img_Chess);

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////차영상 및 조건판정 부분///////////////////////////////////////////////
			if (cross_point.size() != 81) {
				//초기화			
				//Find_Chess.Chess_recog_wrapper(img_Cam, &cross_point);
			}
			else if (cross_point.size() == 81) {
				if (Sub_check == false) {
					//Chessboard recognition;
					Find_Chess.Copy_Img(img_Chess);
					Find_Chess.Chess_recog_wrapper(img_Cam, &cross_point);

					//손이 들어오기 직전 영상을 촬영
					Find_Hand.Sub_prevFrame(img_Chess, img_Skin, BeforeHand_first);					//실시간 차영상->턴별 차영상
					if (BeforeHand_first)
						BeforeHand_first = false;

					cvDilate(img_Skin, img_Skin, 0, 10);
#ifdef DEBUG
					cvShowImage("img_Skin", img_Skin);
#endif
					if (Check_InChessboard(img_Skin, cross_point)){
						//물체가 체스보드 위로 들어옴
						cvCopy(temp_prev2, prev_img);
#ifdef DEBUG
						cvShowImage("PREV", prev_img);
						printf("PREV Catch\n");
#endif
						Sub_check = true;
					}
					else {
						//Find_Chess.Chess_recog_wrapper(img_Cam, &cross_point);
					}
				}
				else {
					//추후 해야할 작업 : 빠질때 어떻게 작업할 것인가
					//손이 들어옴 판정 이후 작업
					/*Find_Hand.Sub_prevFrame(img_Chess, img_sub);*/
#ifdef DEBUG
					cvShowImage("유레카1", img_Chess);
#endif
					Sub_image(prev_img, img_Chess, img_Skin);
					/*cvErode(img_Skin, img_Skin, 0, 1);
					cvDilate(img_Skin, img_Skin, 0, 2);
					cvErode(img_Skin, img_Skin, 0, 1);*/
					Compose_diffImage(img_Chess, img_Skin, cvScalar(0, 255, 255));

					//BlobLabeling
					CBlob.SetParam(img_Skin, 1);
					CBlob.DoLabeling();
					CBlob.DrawLabel(img_Chess, cvScalar(255,0,255));

					//손판정
					CBlob.GetSideBlob(img_Skin, &piece_idx, other);
					Compose_diffImage(img_Chess, img_Skin, cvScalar(100, 100, 255));
					cvDilate(img_Skin, img_Skin, 0, 5);
					cvShowImage("sibal skin!!", img_Skin);
					Find_Chess.drawPoint(img_Chess, cross_point);
					if (Check_InChessboard(img_Skin, cross_point)) {
						//img_Skin은 손 추정물체만 남긴 이미지
						InHand_Check = true;
					}
					else if (InHand_Check == true) {
						//이동 처리부

						//다음턴 준비부 -> 조건을 추가로 줘서 예외를 막아야함
						//if(Check_imgZero(img_Skin)) {								//전제 조건은 손이 빠질때는 중간에 멈추지 않음
							printf("Hand escape!\n");
							InHand_Check = false;
							Sub_check = false;
							BeforeHand_first = true;

							//좌표 계산부

							///////////////////////////////말 좌표 반환//////////////////////////////////////////////////////
							//CvPoint Left, Right;
							//CvRect temp;
							//if (piece_idx.size() > 2) {							//노이즈가 잡혔을때 -> 크기 큰걸로 정렬
							//	int first_idx, second_inx;
							//	int MAX_SIZE;
							//	for (register int j = 0; j < 2; j++) {
							//		MAX_SIZE = -1;
							//		for (register int k = 0; k < piece_idx.size(); k++){
							//			temp = CBlob.m_recBlobs[piece_idx.at(k)];
							//			if (MAX_SIZE < temp.height * temp.width){
							//				MAX_SIZE = temp.height * temp.width;

							//				if (j == 0)		first_idx = k;
							//				else if (j == 1 && k != first_idx)		second_inx = k;
							//			}
							//		}
							//	}

							//	temp = CBlob.m_recBlobs[piece_idx.at(0)];
							//	Left = Get_Chessidx(cvPoint(temp.x+temp.width/2,temp.y+temp.height/3*2), cross_point);
							//	cvCircle(img_Chess, cvPoint(temp.x+temp.width/2,temp.y+temp.height/3*2), 3, cvScalar(0,0,255));
							//	temp = CBlob.m_recBlobs[piece_idx.at(1)];
							//	Right = Get_Chessidx(cvPoint(temp.x+temp.width/2,temp.y+temp.height/3*2), cross_point);
							//	cvCircle(img_Chess, cvPoint(temp.x+temp.width/2,temp.y+temp.height/3*2), 3, cvScalar(0,0,255));
							//	printf("(%d,%d), (%d,%d)\n",Left.x, Left.y, Right.x, Right.y);

							//	CHESS_GAME.Chess_process(Left, Right);
							//	CHESS_GAME.Show_chess_board();
							//}
							//else if(piece_idx.size() == 1) {
							//	//겹쳐버렸을때
							//	temp = CBlob.m_recBlobs[piece_idx.at(0)];
							//	Left = Get_Chessidx(cvPoint(temp.x+temp.width/2, temp.y+temp.height/2+temp.height/3*2), cross_point);
							//	cvCircle(img_Chess, cvPoint(temp.x+temp.width/2, temp.y+temp.height/2+temp.height/3*2), 3, cvScalar(0,0,255)); 
							//	Right = Get_Chessidx(cvPoint(temp.x+temp.width/2, temp.y+temp.height/3), cross_point);
							//	cvCircle(img_Chess, cvPoint(temp.x+temp.width/2, temp.y+temp.height/3), 3, cvScalar(0,0,255));
							//	printf("(%d,%d), (%d,%d)\n",Left.x, Left.y, Right.x, Right.y);

							//	CHESS_GAME.Chess_process(Left, Right);
							//	CHESS_GAME.Show_chess_board();
							//}
							//else if(piece_idx.size() == 2) {						//딱 두개 추적
							//	temp = CBlob.m_recBlobs[piece_idx.at(0)];
							//	Left = Get_Chessidx(cvPoint(temp.x+temp.width/2,temp.y+temp.height/3*2), cross_point);
							//	cvCircle(img_Chess, cvPoint(temp.x+temp.width/2,temp.y+temp.height/3*2), 3, cvScalar(0,0,255));
							//	temp = CBlob.m_recBlobs[piece_idx.at(1)];
							//	Right = Get_Chessidx(cvPoint(temp.x+temp.width/2,temp.y+temp.height/3*2), cross_point);
							//	cvCircle(img_Chess, cvPoint(temp.x+temp.width/2,temp.y+temp.height/3*2), 3, cvScalar(0,0,255));
							//	printf("(%d,%d), (%d,%d)\n",Left.x, Left.y, Right.x, Right.y);

							//	CHESS_GAME.Chess_process(Left, Right);
							//	CHESS_GAME.Show_chess_board();
							//}
						//}
					}
#ifdef DEBUG
					cvShowImage("compose_diff", img_Chess);
					cvShowImage("other", other);
#endif
				}
			}
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			cvCopy(temp_prev, temp_prev2);
			cvCopy(img_Chess, temp_prev);
			cvResetImageROI(img_Cam);

			if (cvWaitKey(10) == 27)
				ImgProcess_Mode++;
			cvDrawRect(img_Cam, cvPoint(ImgProcess_ROI.x, ImgProcess_ROI.y), cvPoint(ImgProcess_ROI.x + ImgProcess_ROI.width, ImgProcess_ROI.y + ImgProcess_ROI.height), cvScalar(255), 2);
			tick = GetTickCount() - tick;
			sprintf(buf, "%.2f fps", 1000.f/ (float)tick);
			cvPutText(img_Cam, buf, cvPoint(30, 30), &cvFont(1.0), cvScalar(0, 0, 255));
			cvShowImage("CVES", img_Cam);
			break;
	}
}

void EngineS::Exit_imgProcess(){
	cvDestroyAllWindows();

	cvReleaseCapture(&Cam);

	cvReleaseImage(&temp_prev);
	cvReleaseImage(&temp_prev2);

	cvReleaseImage(&img_Chess);
	cvReleaseImage(&img_Skin);
	cvReleaseImage(&prev_img);
	cvReleaseImage(&img_sub);
}

bool EngineS::Check_InChessboard(IplImage *img, vector<Chess_point> point){
	CvPoint LH, LL, RH, RL;			//왼쪽 위, 왼쪽 아래, 오른쪽 위 오른쪽 아래
	float tArea, t1Area, t2Area, t3Area, t4Area;
	
	LH = point.at(0).Cordinate;
	RH = point.at(8).Cordinate;
	LL = point.at(72).Cordinate;
	RL = point.at(80).Cordinate;
	tArea = area_tri(LH, LL, RH) + area_tri(RH, RL, LL);

	for (register int i = 0; i < img->width; i++) {
		for (register int j = 0; j < img->height; j++) {
			UCHAR pixel_value = (UCHAR)img->imageData[i + j * img->widthStep];
			
			if (pixel_value == 255) {
				t1Area = area_tri(cvPoint(i,j), LH, RH);
				t2Area = area_tri(cvPoint(i,j), RH, RL);
				t3Area = area_tri(cvPoint(i,j), RL, LL);
				t4Area = area_tri(cvPoint(i,j), LL, LH);

				float totalArea = t1Area + t2Area + t3Area + t4Area;
				if (fabs(tArea - totalArea) < 2)
					return true;
			}
		}
	}

	return false;
}

float EngineS::area_tri(CvPoint p, CvPoint q, CvPoint r) {
	return (float)abs(((p.x * q.y) + (q.x * r.y) + (r.x * p.y))-((p.y * q.x) + (q.y * r.x) + (r.y * p.x))) / 2.0;
}

void EngineS::Sub_image(IplImage *src1, IplImage *src2, IplImage *dst) {
	cvZero(dst);

	for (register int i = 0; i < src1->width; i++) {
		for (register int j = 0; j < src1->height; j++) {
			unsigned char SUB_B = abs((unsigned char)src1->imageData[(i * 3) + (j * src1->widthStep)] - (unsigned char)src2->imageData[(i * 3) + (j * src2->widthStep)]);
			unsigned char SUB_G = abs((unsigned char)src1->imageData[(i * 3) + (j * src1->widthStep) + 1] - (unsigned char)src2->imageData[(i * 3) + (j * src2->widthStep) + 1]);
			unsigned char SUB_R = abs((unsigned char)src1->imageData[(i * 3) + (j * src1->widthStep) + 2] - (unsigned char)src2->imageData[(i * 3) + (j * src2->widthStep) + 2]);

			if (SUB_B > SUB_THRESHOLD || SUB_G > SUB_THRESHOLD || SUB_R > SUB_THRESHOLD) {
				dst->imageData[i + (j * dst->widthStep)] = (unsigned char)255;
			}
		}
	}

	cvSmooth(dst, dst, CV_MEDIAN, 3, 3);
	cvErode(dst, dst, 0, 2);
	cvDilate(dst, dst, 0, 2);
}

void EngineS::Compose_diffImage(IplImage *rgb, IplImage *bin, CvScalar RGB) {
	unsigned char bin_value;

	for (register int i = 0; i < rgb->width; i++)
		for (register int j = 0; j < rgb->height; j++) {
			bin_value = (unsigned char)bin->imageData[i + j * bin->widthStep];

			if (bin_value == 255) {
				rgb->imageData[(i * 3) + (j * rgb->widthStep)] = RGB.val[0];
				rgb->imageData[(i * 3) + (j * rgb->widthStep) + 1] = RGB.val[1];
				rgb->imageData[(i * 3) + (j * rgb->widthStep) + 2] = RGB.val[2];
			}
		}
}

bool EngineS::Check_imgZero(IplImage *img){
	unsigned char pixel_value;
	for (register int i = 0; i < img->width; i++)
		for (register int j = 0; j < img->height; j++) {

			pixel_value = (unsigned char)img->imageData[i + (j * img->widthStep)];

			if (pixel_value != 0)
				return false;
		}

		return true;
}

CvPoint EngineS::Get_Chessidx(CvPoint point, vector<Chess_point> cross_point){
	for (register int i = 0; i < cross_point.size() - 10; i++) {
		if (cross_point.at(i).Cordinate.x <= point.x && cross_point.at(i).Cordinate.y <= point.y) {
			if (cross_point.at(i + 10).Cordinate.x > point.x && cross_point.at(i + 10).Cordinate.y > point.y)
				return cross_point.at(i).index;
		}
	}
	return cvPoint(-1, -1);
}

void EngineS::Inter_imageCraete(int roi_width, int roi_height){
	img_Skin = cvCreateImage(cvSize(roi_width, roi_height), IPL_DEPTH_8U, 1);
	prev_img = cvCreateImage(cvSize(roi_width, roi_height), IPL_DEPTH_8U, 3);
	img_sub = cvCreateImage(cvSize(roi_width, roi_height), IPL_DEPTH_8U, 1);
}

void EngineS::EngineS_Start() {
	//EngineS IP;
	Init_process();
	while (EngineEnable) {
		Do_imgprocess();

		if (Check_Exit()) break;
	}

	Exit_imgProcess();
}