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

#include "ChessObjectDetection.hpp"

#pragma region Constructor & Destructor
ChessObjectDetection::ChessObjectDetection() {
	_Canny_HighThreshold = CANNY_EDGE_THRESHOLD_HIGH;
	_Canny_LowThreshold = CANNY_EDGE_THRESHOLD_LOW;

	_Low_Bright = 135;
	_High_Bright = 0;

	_Thickness = 15;

	_ScoreThreshold = 0.01;
	_SubThreshold = 0.01;
}

ChessObjectDetection::~ChessObjectDetection() {
}
#pragma endregion Constructor & Destructor

#pragma region Private Functions
// 이상 없음.
//void ChessObjectDetection::ConvertHplane(IplImage *Source) {
//	IplImage *_TTemp_Source = cvCreateImage(cvGetSize(Source), IPL_DEPTH_8U, 3);
//
//	cvCvtColor(Source, _TTemp_Source, CV_BGR2HSV);
//	cvSetImageCOI(_TTemp_Source, 1);
//	cvCopy(_TTemp_Source, _H_Plane);
//
//	cvReleaseImage(&_TTemp_Source);
//}

// 이상 없음.
//void ChessObjectDetection::ConvertSplane(IplImage *Source) {
//	IplImage *_TTemp_Source = cvCreateImage(cvGetSize(Source), IPL_DEPTH_8U, 3);
//
//	cvCvtColor(Source, _TTemp_Source, CV_BGR2HSV);
//	cvSetImageCOI(_TTemp_Source, 2);
//	cvCopy(_TTemp_Source, _S_Plane);
//
//	cvReleaseImage(&_TTemp_Source);
//}

// 이상 없음.
//void ChessObjectDetection::Add_CannyImage(IplImage *H_Canny, IplImage *S_Canny, IplImage *Destination) {
//	IplImage *_TH_Canny = cvCreateImage(cvGetSize(H_Canny), IPL_DEPTH_8U, 1);
//	IplImage *_TS_Canny = cvCreateImage(cvGetSize(S_Canny), IPL_DEPTH_8U, 1);
//
//	cvZero(Destination);
//
//	for (register int i = 0; i < Destination->width; i++) {
//		for (register int j = 0; j < Destination->height; j++) {
//			unsigned char H_Value = (unsigned char)_TH_Canny->imageData[i + j * _TH_Canny->widthStep];
//			unsigned char S_Value = (unsigned char)_TS_Canny->imageData[i + j * _TS_Canny->widthStep];
//
//			if (H_Value != 0 || S_Value != 0)
//				Destination->imageData[i + j * Destination->widthStep] = 255;
//		}
//	}
//
//	cvReleaseImage(&_TH_Canny);
//	cvReleaseImage(&_TS_Canny);
//}

void ChessObjectDetection::Delete_ChessLine(IplImage *Edge, vector<_ChessPoint> CrossPoint) {
	if (CrossPoint.size() == 0) // 체스보드 인식 좌표가 없다면 return error
		return;

	for (register int i = 0; i < CrossPoint.size() - 1; i++) {
		ChessPoint _temp_CP = CrossPoint.at(i);
		if (_temp_CP.Index.y != 8) {
			cvDrawLine(Edge, _temp_CP.Cordinate, CrossPoint.at(i+1).Cordinate, cvScalarAll(0), _Thickness);
		}

		if (_temp_CP.Index.x != 8) {
			cvDrawLine(Edge, _temp_CP.Cordinate, CrossPoint.at(i+9).Cordinate, cvScalarAll(0), _Thickness);
		}
	}
}

void ChessObjectDetection::Thresholding_Score(float Score[][8], float Threshold) {
	for (register int i = 0; i < 8; i++) {
		for (register int j = 0; j < 8; j++) {
			if (Score[i][j] < Threshold)
				Score[i][j] = 0.0f;
		}
	}
}

void ChessObjectDetection::Detect_Movement(float BeforeScore[][8], float AfterScore[][8], CvPoint Result[]) {
	float _TSubScore[8][8];

	// 스코어를 면적으로 나눠줘서 비율을 구함.
	// 가장 비율이 큰 두 좌표를 리턴. -> num 개의 좌표를 리턴
	float _TTempMax[4];
	CvPoint _TPointMax[4];

	// 초기화
	for (register int i = 0; i < 4; i++){
		_TPointMax[i] = cvPoint(-1,-1);
		_TTempMax[i] = -1.0;
	}

	// 스코어 차이값 연산부
	for(register int i = 0; i < 8; i++)
		for(register int j = 0; j < 8; j++)
			_TSubScore[i][j] = fabs(BeforeScore[i][j] - AfterScore[i][j]);

	// 0번지부터 큰순으로 연산한 결과 저장
	for (register int i = 0 ; i < 8; i++) {
		for (register int j = 0; j < 8; j++) {

			if(_SubThreshold <= _TSubScore[i][j]){
				if(_TTempMax[0] <= _TSubScore[i][j]){
					_TTempMax[3] = _TTempMax[2];
					_TTempMax[2] = _TTempMax[1];
					_TTempMax[1] = _TTempMax[0];
					_TTempMax[0] = _TSubScore[i][j];

					_TPointMax[3] = _TPointMax[2];
					_TPointMax[2] = _TPointMax[1];
					_TPointMax[1] = _TPointMax[0];
					_TPointMax[0] = cvPoint(i,j);
				}
				else if(_TTempMax[0] > _TSubScore[i][j] && _TSubScore[i][j] >= _TTempMax[1]) {
					_TTempMax[3] = _TTempMax[2];
					_TTempMax[2] = _TTempMax[1];
					_TTempMax[1] = _TSubScore[i][j];

					_TPointMax[3] = _TPointMax[2];
					_TPointMax[2] = _TPointMax[1];
					_TPointMax[1] = cvPoint(i,j);
				}
				else if(_TTempMax[1] > _TSubScore[i][j] && _TSubScore[i][j] >= _TTempMax[2]) {
					_TTempMax[3] = _TTempMax[2];
					_TTempMax[2] = _TSubScore[i][j];

					_TPointMax[3] = _TPointMax[2];
					_TPointMax[2] = cvPoint(i,j);
				}
				else if(_TTempMax[2] > _TSubScore[i][j] && _TSubScore[i][j] >= _TTempMax[3]) {
					_TTempMax[3] = _TSubScore[i][j];

					_TPointMax[3] = cvPoint(i,j);
				}
			}
		}
	}

	// 4개에 할당.
	for (register int i = 0; i < 4; i++){
		Result[i] = _TPointMax[i];
#ifdef DEBUG_MODE
		printf("(%d, %d) - %f\n", _TPointMax[i].x, _TPointMax[i].y, _TTempMax[i]);
#endif
	}
#ifdef DEBUG_MODE
	printf("\n");
#endif
}

void ChessObjectDetection::Detect_SobelCannyScore(IplImage *Source, vector<_ChessPoint> CrossPoint, float ScoreOut[][8]) {
	if (CrossPoint.size() != 0) {
		IplImage *_TGray = cvCreateImage(cvGetSize(Source), IPL_DEPTH_8U, 1);
		IplImage *_TSobel = cvCreateImage(cvGetSize(Source), IPL_DEPTH_8U, 1);
		IplImage *_TAdd_Canny = cvCreateImage(cvGetSize(Source), IPL_DEPTH_8U, 1);

		cvCvtColor(Source, _TGray, CV_BGR2GRAY);
		// sobel->canny.
		cvSobel(_TGray, _TSobel, 1, 1, 5);
		cvCanny(_TSobel, _TAdd_Canny, 100, 150);

		// 체스보드의 라인영향을 최소화시킴.
		Delete_ChessLine(_TAdd_Canny, CrossPoint);

		// 오브젝트 유무를 확인하기 위해서
		// 각 체스보드 그리드 안에 엣지가 존재하는 면적비를 연산.
		float score_board[8][8]; // 엣지 / 체스그리드 면적 => 스코어

		_CheckChessboard.Calculate_BoardScore(_TAdd_Canny, CrossPoint, score_board);

		// 스코어 thresholding
		Thresholding_Score(score_board, _ScoreThreshold);

		// 출력으로 복사
		for (register int i = 0; i < 8; i++)
			for (register int j = 0; j < 8; j++)
				ScoreOut[i][j] = score_board[i][j];

#if defined(DEBUG_MODE)
		cvShowImage("Add_Canny", _TAdd_Canny);
#endif

		cvReleaseImage(&_TGray);
		cvReleaseImage(&_TSobel);
		cvReleaseImage(&_TAdd_Canny);
	}
}

//void ChessObjectDetection::Get_EdgeImage(IplImage *Source, IplImage *Destination) {
//	IplImage *_THSV = cvCreateImage(cvGetSize(Source), IPL_DEPTH_8U, 3);
//	IplImage *_TH = cvCreateImage(cvGetSize(Source), IPL_DEPTH_8U, 1);
//	IplImage *_TS = cvCreateImage(cvGetSize(Source), IPL_DEPTH_8U, 1);
//
//	cvZero(Destination);
//
//	cvCvtColor(Source, _THSV, CV_BGR2HSV);
//
//	cvSetImageCOI(_THSV, 1);
//	cvCopy(_THSV, _TH);
//	cvSetImageCOI(_THSV, 2);
//	cvCopy(_THSV, _TS);
//
//	Add_CannyImage(_TH,_TS, Destination);
//	//cvCanny(H, dst, threshold1, threshold2);
//
//	cvReleaseImage(&_THSV);
//	cvReleaseImage(&_TH);
//	cvReleaseImage(&_TS);
//}

// 이상 없음.
//void ChessObjectDetection::DetectObject(IplImage *Source, vector<_ChessPoint> CrossPoint, bool *Board[]) {
//	IplImage *_T_H_Canny = cvCreateImage(cvGetSize(Source), IPL_DEPTH_8U, 1);
//	IplImage *_T_S_Canny = cvCreateImage(cvGetSize(Source), IPL_DEPTH_8U, 1); 
//	IplImage *_T_Add_Canny = cvCreateImage(cvGetSize(Source), IPL_DEPTH_8U, 1);
//
//	ConvertHplane(Source);
//	ConvertSplane(Source);
//
//	cvCanny(_H_Plane, _T_H_Canny, _Canny_LowThreshold, _Canny_HighThreshold);
//	cvCanny(_S_Plane, _T_S_Canny, _Canny_LowThreshold, _Canny_HighThreshold);
//
//	Add_CannyImage(_T_H_Canny, _T_S_Canny, _T_Add_Canny);
//
//	cvShowImage("Canny", _T_Add_Canny);
//
//	// 체스보드의 라인영향을 최소화시킴.
//	Delete_ChessLine(_T_Add_Canny, CrossPoint);
//
//	// 오브젝트 유무를 확인하기 위해서 각 체스보드 그리드 안에 엣지가 존재하는 면적비를 연산.
//	float ScoreBoard[8][8]; // 엣지 / 체스그리드 면적 => 스코어.
//	_CheckChessboard.Calculate_BoardScore(_T_Add_Canny, CrossPoint, ScoreBoard);
//
//	cvReleaseImage(&_T_Add_Canny);
//	cvReleaseImage(&_T_H_Canny);
//	cvReleaseImage(&_T_S_Canny);
//}
#pragma endregion Private Functions

#pragma region Public Functions
void ChessObjectDetection::Get_Movement(IplImage *Before, IplImage *After, vector<_ChessPoint> _CrossPoint, CvPoint Result[]) {
	float _TBeforeScore[8][8], _TAfterScore[8][8];

	// 이전 영상과 이후 영상의 스코어를 계산.
	/*DetectScore(before, _cross_point, before_score);
	DetectScore(after, _cross_point, after_score);*/
	Detect_SobelCannyScore(Before, _CrossPoint, _TBeforeScore);
	Detect_SobelCannyScore(After, _CrossPoint, _TAfterScore);

	//움직임 계산
	Detect_Movement(_TBeforeScore, _TAfterScore, Result);
}

void ChessObjectDetection::Set_CannyThreshold(int Low, int High) {
	_Canny_LowThreshold = Low;
	_Canny_HighThreshold = High;
}

void ChessObjectDetection::Set_Bright(int Low, int High) {
	_High_Bright = High;
	_Low_Bright = Low;
}

void ChessObjectDetection::Set_Thickness(int Thickness) {
	_Thickness = Thickness;
}

void ChessObjectDetection::Set_SubThreshold(int Threshold) {
	_SubThreshold = Threshold;
}

void ChessObjectDetection::Set_ScoreThreshold(int Threshold) {
	_ScoreThreshold = Threshold;
}

//void ChessObjectDetection::DetectScore(IplImage *src, vector<_ChessPoint> _cross_point, float score_out[][8]) {
//	if(_cross_point.size() != 0){
//		IplImage *brightHigh_Canny = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
//		IplImage *brightLow_Canny = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1); 
//		IplImage *Add_Canny = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
//		IplImage *high_bright_img = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);
//		IplImage *low_bright_img = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);
//
//		//밝은 영상 어두운 영상 생성
//		cvAddS(src, CV_RGB(_high_bright,_high_bright,_high_bright), high_bright_img);
//		cvSubS(src, CV_RGB(_low_bright, _low_bright, _low_bright), low_bright_img);
//
//		// 각 밝기에서 H, S Edge detection
//		Get_EdgeImage(high_bright_img, brightHigh_Canny);
//		Get_EdgeImage(low_bright_img, brightLow_Canny);
//
//		cvAdd(brightHigh_Canny, brightLow_Canny, Add_Canny);
//		// 체스보드의 라인영향을 최소화시킴
//		Delete_ChessLine(Add_Canny, _cross_point);
//
//		// 오브젝트 유무를 확인하기 위해서
//		// 각 체스보드 그리드 안에 엣지가 존재하는 면적비를 연산
//		float score_board[8][8]; // 엣지 / 체스그리드 면적 => 스코어
//		_CheckChessboard.Calculate_BoardScore(Add_Canny, _cross_point, score_board);
//
//		// 스코어 thresholding
//		Thresholding_Score(score_board, _score_threshold);
//
//		// 출력으로 복사
//		for(int i = 0; i < 8; i++)
//			for(int j = 0; j < 8; j++)
//				score_out[i][j] = score_board[i][j];
//
//#ifdef DEBUG_MODE
//		cvShowImage("Add_Canny", Add_Canny);
//#endif
//
//		cvReleaseImage(&Add_Canny);
//		cvReleaseImage(&brightHigh_Canny);
//		cvReleaseImage(&brightLow_Canny);
//
//		cvReleaseImage(&high_bright_img);
//		cvReleaseImage(&low_bright_img);
//	}
//}
#pragma endregion Public Functions