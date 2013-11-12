#include "ChessObjDetection.h"


ChessObjDetection::ChessObjDetection(void)
{
	_Canny_HighThreshold = 250;
	_Canny_LowThreshold = 240;

	_low_bright = 135;
	_high_bright = 0;

	_thickness = 20;

	_score_threshold = 0.01;
	_sub_threshold = 0.01;
}


ChessObjDetection::~ChessObjDetection(void)
{
}

void ChessObjDetection::SetCannyThreshold( int Low, int High )
{
	_Canny_LowThreshold = Low;
	_Canny_HighThreshold = High;
}

void ChessObjDetection::DetectScore( IplImage *src, vector<_ChessPoint> _cross_point, float score_out[][8] )
{
	if(_cross_point.size() != 0){
		IplImage *brightHigh_Canny = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
		IplImage *brightLow_Canny = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1); 
		IplImage *Add_Canny = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
		IplImage *high_bright_img = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);
		IplImage *low_bright_img = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);

		//밝은 영상 어두운 영상 생성
		cvAddS(src, CV_RGB(_high_bright,_high_bright,_high_bright), high_bright_img);
		cvSubS(src, CV_RGB(_low_bright, _low_bright, _low_bright), low_bright_img);

		// 각 밝기에서 H, S Edge detection
		Get_Edgeimg(high_bright_img, brightHigh_Canny);
		Get_Edgeimg(low_bright_img, brightLow_Canny);

		cvAdd(brightHigh_Canny, brightLow_Canny, Add_Canny);
		// 체스보드의 라인영향을 최소화시킴
		Delete_ChessLine(Add_Canny, _cross_point);

		// 오브젝트 유무를 확인하기 위해서
		// 각 체스보드 그리드 안에 엣지가 존재하는 면적비를 연산
		float score_board[8][8]; // 엣지 / 체스그리드 면적 => 스코어
		_CheckChessboard->Cal_BoardScore(Add_Canny, _cross_point, score_board);

		// 스코어 thresholding
		Thresholding_score(score_board, _score_threshold);

		// 출력으로 복사
		for(int i = 0; i < 8; i++)
			for(int j = 0; j < 8; j++)
				score_out[i][j] = score_board[i][j];

#ifdef DEBUG_MODE
		cvShowImage("Add_Canny", Add_Canny);
#endif

		cvReleaseImage(&Add_Canny);
		cvReleaseImage(&brightHigh_Canny);
		cvReleaseImage(&brightLow_Canny);

		cvReleaseImage(&high_bright_img);
		cvReleaseImage(&low_bright_img);
	}
}

void ChessObjDetection::Delete_ChessLine( IplImage *edge, vector<_ChessPoint> _cross_point )
{
	if(_cross_point.size() == 0) // 체스보드 인식 좌표가 없다면 return error
		return;

	for(int i = 0; i < _cross_point.size() - 1; i++){
		_ChessPoint _temp_CP = _cross_point.at(i);
		if(_temp_CP.index.y != 8){
			cvDrawLine(edge, _temp_CP.Cordinate, _cross_point.at(i+1).Cordinate, cvScalarAll(0), _thickness);
		}

		if(_temp_CP.index.x != 8){
			cvDrawLine(edge, _temp_CP.Cordinate, _cross_point.at(i+9).Cordinate, cvScalarAll(0), _thickness);
		}
	}
}

void ChessObjDetection::SetBright( int Low, int High )
{
	_high_bright = High;
	_low_bright = Low;
}

void ChessObjDetection::Get_Edgeimg( IplImage *src, IplImage *dst )
{
	IplImage *HSV = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);
	IplImage *H = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	IplImage *S = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);

	cvZero(dst);

	cvCvtColor(src, HSV, CV_BGR2HSV);

	cvSetImageCOI(HSV, 1);
	cvCopy(HSV, H);
	cvSetImageCOI(HSV, 2);
	cvCopy(HSV, S);

	add_Canny(H,S, dst);
	//cvCanny(H, dst, threshold1, threshold2);

	cvReleaseImage(&HSV);
	cvReleaseImage(&H);
	cvReleaseImage(&S);
}

void ChessObjDetection::add_Canny( IplImage *H, IplImage *S, IplImage *dst )
{
	IplImage *H_canny = cvCreateImage(cvGetSize(H), IPL_DEPTH_8U, 1);
	IplImage *S_canny = cvCreateImage(cvGetSize(S), IPL_DEPTH_8U, 1);

	cvZero(dst);

	cvCanny(H, H_canny, _Canny_LowThreshold, _Canny_HighThreshold);
	cvCanny(S, S_canny, _Canny_LowThreshold, _Canny_HighThreshold);


	for(int i = 0; i < dst->width; i++){
		for(int j = 0; j < dst->height; j++){
			unsigned char H_Value = (unsigned char)H_canny->imageData[i + j * H_canny->widthStep];
			unsigned char S_Value = (unsigned char)S_canny->imageData[i + j * S_canny->widthStep];

			if(H_Value != 0 || S_Value != 0)
				dst->imageData[i + j * dst->widthStep] = 255;
		}
	}

	cvReleaseImage(&H_canny);
	cvReleaseImage(&S_canny);
}

void ChessObjDetection::Thresholding_score( float score[][8], float threshold )
{
	for(int i = 0; i < 8; i++){
		for(int j = 0; j < 8; j++){
			if(score[i][j] < threshold)
				score[i][j] = 0.0f;
		}
	}
}

void ChessObjDetection::SetThickness( int thickness )
{
	_thickness = thickness;
}

void ChessObjDetection::SetScoreThreshold( int threshold )
{
	_score_threshold = threshold;
}

void ChessObjDetection::DetectMovement( float score_before[][8], float score_after[][8], CvPoint out[] )
{
	float sub_score[8][8];

	// 스코어를 면적으로 나눠줘서 비율을 구함.
	// 가장 비율이 큰 두 좌표를 리턴. -> num 개의 좌표를 리턴
	float temp_max[4];
	CvPoint p_max[4];

	// 초기화
	for (register int i = 0; i < 4; i++){
		p_max[i] = cvPoint(-1,-1);
		temp_max[i] = -1.0;
	}

	// 스코어 차이값 연산부
	for(int i = 0; i < 8; i++)
		for(int j = 0; j < 8; j++)
			sub_score[i][j] = fabs(score_before[i][j] - score_after[i][j]);

	//0번지부터 큰순으로 연산한 결과 저장
	for (register int i = 0 ; i < 8; i++) {
		for (register int j = 0; j < 8; j++) {

			if(_sub_threshold <= sub_score[i][j]){
				if(temp_max[0] <= sub_score[i][j]){
					temp_max[3] = temp_max[2];
					temp_max[2] = temp_max[1];
					temp_max[1] = temp_max[0];
					temp_max[0] = sub_score[i][j];

					p_max[3] = p_max[2];
					p_max[2] = p_max[1];
					p_max[1] = p_max[0];
					p_max[0] = cvPoint(i,j);
				}
				else if(temp_max[0] > sub_score[i][j] && sub_score[i][j] >= temp_max[1]) {
					temp_max[3] = temp_max[2];
					temp_max[2] = temp_max[1];
					temp_max[1] = sub_score[i][j];

					p_max[3] = p_max[2];
					p_max[2] = p_max[1];
					p_max[1] = cvPoint(i,j);
				}
				else if(temp_max[1] > sub_score[i][j] && sub_score[i][j] >= temp_max[2]) {
					temp_max[3] = temp_max[2];
					temp_max[2] = sub_score[i][j];

					p_max[3] = p_max[2];
					p_max[2] = cvPoint(i,j);
				}
				else if(temp_max[2] > sub_score[i][j] && sub_score[i][j] >= temp_max[3]) {
					temp_max[3] = sub_score[i][j];

					p_max[3] = cvPoint(i,j);
				}
			}
		}
	}

	//4개에 할당
	for (register int i = 0; i < 4; i++){
		out[i] = p_max[i];
#ifdef DEBUG_MODE
		printf("(%d, %d) - %f\n", p_max[i].x, p_max[i].y, temp_max[i]);
#endif
	}
#ifdef DEBUG_MODE
	printf("\n");
#endif
}

void ChessObjDetection::SetSubThreshold( int threshold )
{
	_sub_threshold = threshold;
}

void ChessObjDetection::Get_Movement( IplImage *before, IplImage *after, vector<_ChessPoint> _cross_point, CvPoint out[] )
{
	float before_score[8][8], after_score[8][8];

	//이전 영상과 이후 영상의 스코어를 계산
	/*DetectScore(before, _cross_point, before_score);
	DetectScore(after, _cross_point, after_score);*/
	Detect_SobelCannyScore(before, _cross_point, before_score);
	Detect_SobelCannyScore(after, _cross_point, after_score);

	//움직임 계산
	DetectMovement(before_score, after_score, out);
}

void ChessObjDetection::Detect_SobelCannyScore( IplImage *src, vector<_ChessPoint> _cross_point, float score_out[][8] )
{
	if(_cross_point.size() != 0){
		IplImage *gray = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
		IplImage *sobel = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
		IplImage *Add_Canny = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);

		cvCvtColor(src, gray, CV_BGR2GRAY);
		//sobel->canny
		cvSobel(gray, sobel, 1, 1, 5);
		cvCanny(sobel, Add_Canny, 100, 150);

		// 체스보드의 라인영향을 최소화시킴
		Delete_ChessLine(Add_Canny, _cross_point);

		// 오브젝트 유무를 확인하기 위해서
		// 각 체스보드 그리드 안에 엣지가 존재하는 면적비를 연산
		float score_board[8][8]; // 엣지 / 체스그리드 면적 => 스코어
		_CheckChessboard->Cal_BoardScore(Add_Canny, _cross_point, score_board);

		// 스코어 thresholding
		Thresholding_score(score_board, _score_threshold);

		// 출력으로 복사
		for(int i = 0; i < 8; i++)
			for(int j = 0; j < 8; j++)
				score_out[i][j] = score_board[i][j];

#ifdef DEBUG_MODE
		cvShowImage("Add_Canny", Add_Canny);
#endif

		cvReleaseImage(&gray);
		cvReleaseImage(&sobel);
		cvReleaseImage(&Add_Canny);
	}
}

