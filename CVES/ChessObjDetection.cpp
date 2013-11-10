#include "ChessObjDetection.h"


ChessObjDetection::ChessObjDetection(void)
{
	_Canny_HighThreshold = 250;
	_Canny_LowThreshold = 240;
}


ChessObjDetection::~ChessObjDetection(void)
{
}

void ChessObjDetection::SetCannyThreshold( int Low, int High )
{
	_Canny_LowThreshold = Low;
	_Canny_HighThreshold = High;
}

void ChessObjDetection::ConvertHplane( IplImage *src )
{
	IplImage *temp_src = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);

	cvCvtColor(src, temp_src, CV_BGR2HSV);
	cvSetImageCOI(temp_src, 1);
	cvCopy(temp_src, _H_Plane);

	cvReleaseImage(&temp_src);
}

void ChessObjDetection::ConvertSplane( IplImage *src )
{
	IplImage *temp_src = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);

	cvCvtColor(src, temp_src, CV_BGR2HSV);
	cvSetImageCOI(temp_src, 2);
	cvCopy(temp_src, _S_Plane);

	cvReleaseImage(&temp_src);
}

void ChessObjDetection::add_CannyImg( IplImage *H_canny, IplImage *S_canny, IplImage *dst )
{
	cvZero(dst);

	for(int i = 0; i < dst->width; i++){
		for(int j = 0; j < dst->height; j++){
			unsigned char H_Value = (unsigned char)H_canny->imageData[i + j * H_canny->widthStep];
			unsigned char S_Value = (unsigned char)S_canny->imageData[i + j * S_canny->widthStep];

			if(H_Value != 0 || S_Value != 0)
				dst->imageData[i + j * dst->widthStep] = 255;
		}
	}
}

void ChessObjDetection::DetectObj( IplImage *src, vector<_ChessPoint> _cross_point, bool *board[] )
{
	IplImage *H_Canny = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	IplImage *S_Canny = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1); 
	IplImage *Add_Canny = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);

	ConvertHplane(src);
	ConvertSplane(src);

	cvCanny(_H_Plane, H_Canny, _Canny_LowThreshold, _Canny_HighThreshold);
	cvCanny(_S_Plane, S_Canny, _Canny_LowThreshold, _Canny_HighThreshold);

	add_CannyImg(H_Canny, S_Canny, Add_Canny);

	// 체스보드의 라인영향을 최소화시킴
	Delete_ChessLine(Add_Canny, _cross_point);

	// 오브젝트 유무를 확인하기 위해서
	// 각 체스보드 그리드 안에 엣지가 존재하는 면적비를 연산
	float score_board[8][8]; // 엣지 / 체스그리드 면적 => 스코어
	_CheckChessboard->Cal_BoardScore(Add_Canny, _cross_point, score_board);


	cvReleaseImage(&Add_Canny);
	cvReleaseImage(&H_Canny);
	cvReleaseImage(&S_Canny);
}

void ChessObjDetection::Delete_ChessLine( IplImage *edge, vector<_ChessPoint> _cross_point )
{

}
