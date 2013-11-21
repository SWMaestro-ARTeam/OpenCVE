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

	_Thickness = 25;

	_ScoreThreshold = 0.01;
	_SubThreshold = 0.01;
}

ChessObjectDetection::~ChessObjectDetection() {
}
#pragma endregion Constructor & Destructor

#pragma region Private Functions

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
			if (fabs(Score[i][j]) < Threshold)
				Score[i][j] = 0.0f;
		}
	}
}

void ChessObjectDetection::Detect_Movement(float BeforeScore[][8], float AfterScore[][8], CvPoint Result[]) {
	float _TSubScore[8][8];

	// 스코어를 면적으로 나눠줘서 비율을 구함.
	// 가장 비율이 큰 두 좌표를 리턴. -> num 개의 좌표를 리턴

	for (register int i = 0; i < 8; i++){
		for(register int j = 0; j < 8; j++){
			_TSubScore[i][j] = fabs(BeforeScore[i][j] - AfterScore[i][j]);
		}
	}

	for (register int i = 0; i < 8; i++) {
		for (register int j = 0; j < 8; j++) {
			if (BeforeScore[i][j] > 0)
				BeforeScore[i][j] = 1;
			else if (BeforeScore[i][j] < 0)
				BeforeScore[i][j] = -1;
			if (AfterScore[i][j] > 0)
				AfterScore[i][j] = 1;
			else if (AfterScore[i][j] < 0)
				AfterScore[i][j] = -1;
		}
	}

	for (register int i = 0; i < 4; i++)
		Result[i] = cvPoint(-1,-1);

	int count = 0;
	for (register int i = 0; i < 8; i++) {
		for (register int j = 0; j < 8; j++) {
			if (fabs(BeforeScore[i][j] - AfterScore[i][j]) > 0) {
				if (count < 4) {
					Result[count] = cvPoint(i,j);
					count++;
				}
			}
		}
	}

	// 면적 기반 정렬
	float sub_out[4];
	for (register int i = 0; i < count; i++) {
		sub_out[i] = _TSubScore[Result[i].x][Result[i].y];
	}

	float MAX_subscore = -1.0;
	for (register int i = 0; i < count; i++) {
		for (register int j = i; j < count; j++) {
			if (sub_out[i] < sub_out[j]) {
				float t = sub_out[j];
				sub_out[j] = sub_out[i];
				sub_out[i] = t;

				CvPoint t_point = Result[j];
				Result[j] = Result[i];
				Result[i] = t_point;

			}
		}
	}

#if defined(DEBUG_MODE)
	for (int i = 0; i < count; i++)
		printf("%d : (%d, %d) - score %f\n", i, Result[i].x, Result[i].y, sub_out[i]);
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
		for(register int i = 0; i < NUM_GAUSS; i++){
			cvSmooth(_TGray, _TGray, CV_MEDIAN);
		}
		for(register int i = 0; i < NUM_MEDIAN; i++){
			cvSmooth(_TGray, _TGray, CV_GAUSSIAN);
		}
		cvSobel(_TGray, _TSobel, 1, 1, 5);
		cvCanny(_TSobel, _TAdd_Canny, 100, 150);

		// 체스보드의 라인영향을 최소화시킴.
		Delete_ChessLine(_TAdd_Canny, CrossPoint);

		// 체스보드 이외의 영역을 모두 지워냄
		_CheckChessboard.Delete_Chessboard(_TGray, CrossPoint);
		_CheckChessboard.Delete_Chessboard(_TAdd_Canny, CrossPoint);
		cvDilate(_TAdd_Canny, _TAdd_Canny, 0, 7);
		cvErode(_TAdd_Canny, _TAdd_Canny, 0, 6);

		// 오브젝트 유무를 확인하기 위해서
		// 각 체스보드 그리드 안에 엣지가 존재하는 면적비를 연산.
		float score_board[8][8]; // 엣지 / 체스그리드 면적 => 스코어

		_CheckChessboard.Calculate_BoardScore(_TAdd_Canny, _TGray, CrossPoint, score_board);

		// 스코어 thresholding
		Thresholding_Score(score_board, _ScoreThreshold);

		// 출력으로 복사
		for (register int i = 0; i < 8; i++)
			for (register int j = 0; j < 8; j++)
				ScoreOut[i][j] = score_board[i][j];

#if defined(DEBUG_MODE)
		cvShowImage("gray", _TGray);
		cvShowImage("Add_Canny", _TAdd_Canny);
#endif

		cvReleaseImage(&_TGray);
		cvReleaseImage(&_TSobel);
		cvReleaseImage(&_TAdd_Canny);
	}
}
#pragma endregion Private Functions

#pragma region Public Functions
void ChessObjectDetection::Get_Movement(IplImage *Before, IplImage *After, vector<_ChessPoint> _CrossPoint, CvPoint Result[]) {
	float _TBeforeScore[8][8], _TAfterScore[8][8];

	// 이전 영상과 이후 영상의 스코어를 계산.
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
#pragma endregion Public Functions
