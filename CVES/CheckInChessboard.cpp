﻿//////////////////////////////////////////////////////////////////////////////////////////////
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

#include "CheckInChessboard.hpp"

#pragma region Constructor & Destructor
CheckInChessboard::CheckInChessboard() {
	_Blob.Initialize_BlobLabeling();
}

CheckInChessboard::~CheckInChessboard() {
	_Blob.Deinitialize_BlobLabeling();
}
#pragma endregion Constructor & Destructor

#pragma region Private Functions
float CheckInChessboard::Get_TriangleArea(CvPoint Value_P, CvPoint Value_Q, CvPoint Value_R) {
	// 2차원 좌표 p,q,r로 생성되는 삼각형의 넓이를 구함.
	return (float)abs(((Value_P.x * Value_Q.y) + (Value_Q.x * Value_R.y) + (Value_R.x * Value_P.y))
		- ((Value_P.y * Value_Q.x) + (Value_Q.y * Value_R.x) + (Value_R.y * Value_P.x))) / 2.0;
}

CvPoint CheckInChessboard::Get_ChessIndex(CvPoint Point, vector<ChessPoint> CrossPoint) {
	// chessboard의 교점의 index를 부여.
	// left-top이 원점인 좌표계로 index를 부여
	for (register int i = 0; i < CrossPoint.size() - 10; i++) {
		if (CrossPoint.at(i).Cordinate.x <= Point.x && CrossPoint.at(i).Cordinate.y <= Point.y) {
			if (CrossPoint.at(i + 10).Cordinate.x > Point.x && CrossPoint.at(i + 10).Cordinate.y > Point.y)
				return CrossPoint.at(i).Index;
		}
	}

	return cvPoint(-1, -1);
}

CvPoint CheckInChessboard::Get_ChessBoxPosition(int Width, int Height, vector<ChessPoint> CrossPoint) {
	// width, height가 가리키는 픽셀이 어느 체스보드 인덱스를 가지는지를 계산하여 반환.
	// 차영상 결과로 이진화된 이미지의 true 값을 가지는 픽셀의 좌표로 체스판의 인덱스 반환.
	for (register int i = 0; i < 8; i++) {
		for (register int j = 0; j < 8; j++) {
			CvPoint top_left = CrossPoint.at((i * 9) + j).Cordinate;
			CvPoint bot_right = CrossPoint.at((i * 9) + j + 10).Cordinate;

			if (top_left.x <= Width && top_left.y <= Height)
				if (bot_right.x > Width && bot_right.y > Height)
					return cvPoint(j, i);
		}
	}

	//error return;
	return cvPoint(-1,-1);
}

unsigned char CheckInChessboard::Get_MedianVaul_Inkernel(unsigned char _kernel[][PIXEL_PICK_KERNEL_SIZE]) {
	std::vector<unsigned char> _Median_value;
	for(register int i = 0; i < PIXEL_PICK_KERNEL_SIZE; i++){
		for(register int j = 0; j < PIXEL_PICK_KERNEL_SIZE; j++ ){
			_Median_value.push_back(_kernel[i][j]);
		}
	}

	std::sort(_Median_value.begin(), _Median_value.end());

	return _Median_value.at((PIXEL_PICK_KERNEL_SIZE*PIXEL_PICK_KERNEL_SIZE)/2 + 1);
}

float CheckInChessboard::Get_GridPixelvalue(IplImage *gray, CvPoint Headpoint, CvPoint Head_right, CvPoint Head_down, CvPoint right_down) {
	IplImage *temp_src = cvCreateImage(cvSize(40, 40), IPL_DEPTH_8U, 1);
	CvMat* warp_mat = cvCreateMat(3, 3, CV_32FC1);
	int thickness = 3;

	CvPoint2D32f srcTri[4], dstTri[4];

	srcTri[0].x = Headpoint.x + thickness;              // X1
	srcTri[0].y = Headpoint.y + thickness;
	srcTri[1].x = Head_right.x + thickness;    // Y1
	srcTri[1].y = Head_right.y - thickness;
	srcTri[2].x = Head_down.x - thickness;              // Z1
	srcTri[2].y = Head_down.y + thickness;
	srcTri[3].x = right_down.x - thickness;
	srcTri[3].y = right_down.y - thickness;

	dstTri[0].x = 0;
	dstTri[0].y = 0;
	dstTri[1].x = 39;   // Y2
	dstTri[1].y = 0;
	dstTri[2].x = 0;   // Z2
	dstTri[2].y = 39;
	dstTri[3].x = 39;   // Z2
	dstTri[3].y = 39;

	cvGetPerspectiveTransform(srcTri, dstTri, warp_mat);
	cvWarpPerspective(gray, temp_src, warp_mat);

	//char buf[32];
	//sprintf(buf, "%d_%d.jpg", Headpoint.x, Headpoint.y);
	//cvSaveImage(buf, temp_src);
	// 픽셀 picking
	unsigned char _kernel[PIXEL_PICK_KERNEL_SIZE][PIXEL_PICK_KERNEL_SIZE];
	for(int i = 0; i < PIXEL_PICK_KERNEL_SIZE; i++){
		for(int j = 0; j < PIXEL_PICK_KERNEL_SIZE; j++){
			_kernel[i][j] = temp_src->imageData[(20- PIXEL_PICK_KERNEL_SIZE/2 + i) + (20 - PIXEL_PICK_KERNEL_SIZE/2 + j) * temp_src->widthStep];
		}
	}

	cvReleaseImage(&temp_src);
	cvReleaseMat(&warp_mat);

	return (float)	Get_MedianVaul_Inkernel(_kernel);
}

float CheckInChessboard::Get_AvgRect(IplImage *GrayImage, IplImage *edge, CvRect ROI) {
	int count = 0;
	long total = 0;

	for(register int i = 0; i < ROI.width; i++){
		for(register int j = 0; j < ROI.height; j++){
			unsigned char _edgeValue = (unsigned char)edge->imageData[(ROI.x + i) + (ROI.y + j) * edge->widthStep];
			unsigned char _pixelValue = (unsigned char)GrayImage->imageData[(ROI.x + i) + (ROI.y + j) * GrayImage->widthStep];


			if(_edgeValue == 255){
				count++;
				total += _pixelValue;
			}
		}
	}

	return total / count;
}

unsigned char CheckInChessboard::Get_MedianRect(IplImage *Gray, CvRect ROI) {
	IplImage *ROI_Image = cvCreateImage(cvSize(ROI.width, ROI.height), IPL_DEPTH_8U, 1);

	cvSetImageROI(Gray, ROI);
	cvCopy(Gray, ROI_Image);
	cvResetImageROI(Gray);

	// 속력 문제 개선 여지
	vector<unsigned char> _temp_vector;

	for (register int i = 0; i < ROI_Image->width; i++) {
		for (register int j = 0; j < ROI_Image->height; j++) {
			_temp_vector.push_back(ROI_Image->imageData[i + j * ROI_Image->widthStep]);
		}
	}

	sort(_temp_vector.begin(), _temp_vector.end());

	unsigned char return_value = (unsigned char)_temp_vector.at((ROI_Image->width-1)*(ROI_Image->height-1) / 2);
	cvReleaseImage(&ROI_Image);

	return return_value;
}
#pragma endregion Private Functions

#pragma region Public Functions
bool CheckInChessboard::Check_InChessboard(IplImage *Image, vector<ChessPoint> Point) {
	// Chess_point를 통하여 binary image의 픽셀이 chess board 내부에 존재하는지를 확인.
	// 1) 체스판 사각형의 크기를 연산하여 _TTriangleArea에 저장
	// 2) 차영상 결과 Detection된 픽셀의 좌표와 체스판 사각형의 끝점들로 이루어진 삼각형의 넓이를 각각 계산
	// 3) 2번 과정에서 연산된 삼각형 네개의 합이 체스판 사각형의 크기와 차이가 적을 경우 픽셀의 좌표가 사각형 내부에 있음으로 판정
	// 4) 사각형 내부의 픽셀 좌표가 있다는 판정이 되면 return true;
	//_InChessBoardCalculateProtectMutex.lock();
	CvPoint _T_LeftTop, _T_LeftBottom, _T_RightTop, _T_RightBottom; // 왼쪽 위, 왼쪽 아래, 오른쪽 위 오른쪽 아래.

	// tArea : chessboard 전체 넓이, 나머지 : 삼각형 넓이.
	float _TTriangleArea, _TTriangle1Area, _TTriangle2Area, _TTriangle3Area, _TTriangle4Area;

	// chessboard의 코너점을 구함.
	_T_LeftTop = Point.at(0).Cordinate;
	_T_RightTop = Point.at(8).Cordinate;
	_T_LeftBottom = Point.at(72).Cordinate;
	_T_RightBottom = Point.at(80).Cordinate;

	// 코너점을 이용하여 chessboard의 넓이 연산.
	_TTriangleArea = Get_TriangleArea(_T_LeftTop, _T_LeftBottom, _T_RightTop) + Get_TriangleArea(_T_RightTop, _T_RightBottom, _T_LeftBottom);

	// binary image에 존재하는 모든 픽셀을 대상으로 각 코너점과 이루는 삼각형의 넓이를 연산.
	for (register int i = 0; i < Image->width; i++) {
		for (register int j = 0; j < Image->height; j++) {
			unsigned char pixel_value = (unsigned char)Image->imageData[i + j * Image->widthStep];

			if (pixel_value == 255) {
				// Detection된 임의의 픽셀 좌표와 체스판의 코너점으로 이루어진 삼각형 네개의 넓이를 연산.
				_TTriangle1Area = Get_TriangleArea(cvPoint(i,j), _T_LeftTop, _T_RightTop);
				_TTriangle2Area = Get_TriangleArea(cvPoint(i,j), _T_RightTop, _T_RightBottom);
				_TTriangle3Area = Get_TriangleArea(cvPoint(i,j), _T_RightBottom, _T_LeftBottom);
				_TTriangle4Area = Get_TriangleArea(cvPoint(i,j), _T_LeftBottom, _T_LeftTop);

				float _TTotalArea = _TTriangle1Area + _TTriangle2Area + _TTriangle3Area + _TTriangle4Area;

				// 각 삼각형의 합이 chessboard의 넓이와 같다면 return true; => 픽셀이 chessboard 내부에 존재함.
				if (fabs(_TTriangleArea - _TTotalArea) < 2)
					return true;
			}
		}
	}
	//_InChessBoardCalculateProtectMutex.unlock();
	// binary image의 픽셀이 chessboard 내부에 존재하지 않음.
	return false;
}

bool CheckInChessboard::Check_ImageZero(IplImage *Image) {
	// binary image에 픽셀값이 모드 0인지를 확인
	unsigned char _TPixelValue;

	for (register int i = 0; i < Image->width; i++)
		for (register int j = 0; j < Image->height; j++) {
			_TPixelValue = (unsigned char)Image->imageData[i + (j * Image->widthStep)];

			if (_TPixelValue != 0)
				return false;
		}

		return true;
}

void CheckInChessboard::Calculate_Movement(IplImage *BinaryImage, vector<ChessPoint> CrossPoint, CvPoint Result[], float ScoreThreshold) {
	// 차영상의 결과 이미지를 이용하여 체스보드의 score를 부여.
	// score / 면적 을 이용하여 가장 많이 변한 두 좌표를 반환.
	float _TScoreBox[8][8]; // 면적비율 저장 배열.

	// 스코어를 면적으로 나눠줘서 비율을 구함.
	// 가장 비율이 큰 두 좌표를 리턴. -> num 개의 좌표를 리턴
	float _TTempMax[4];
	CvPoint _TPMax[4];

	// 초기화
	for (register int i = 0; i < 4; i++){
		_TPMax[i] = cvPoint(-1,-1);
		_TTempMax[i] = -1.0;
	}

	// 0번지부터 큰순으로 연산한 결과 저장.
	for (register int i = 0 ; i < 8; i++) {
		for (register int j = 0; j < 8; j++) {

			if (_TScoreBox[i][j] >= ScoreThreshold) {
				if (_TTempMax[0] <= _TScoreBox[i][j]) {
					_TTempMax[3] = _TTempMax[2];
					_TTempMax[2] = _TTempMax[1];
					_TTempMax[1] = _TTempMax[0];
					_TTempMax[0] = _TScoreBox[i][j];

					_TPMax[3] = _TPMax[2];
					_TPMax[2] = _TPMax[1];
					_TPMax[1] = _TPMax[0];
					_TPMax[0] = cvPoint(i,j);
				}
				else if (_TTempMax[0] > _TScoreBox[i][j] && _TScoreBox[i][j] >= _TTempMax[1]) {
					_TTempMax[3] = _TTempMax[2];
					_TTempMax[2] = _TTempMax[1];
					_TTempMax[1] = _TScoreBox[i][j];

					_TPMax[3] = _TPMax[2];
					_TPMax[2] = _TPMax[1];
					_TPMax[1] = cvPoint(i,j);
				}
				else if (_TTempMax[1] > _TScoreBox[i][j] && _TScoreBox[i][j] >= _TTempMax[2]) {
					_TTempMax[3] = _TTempMax[2];
					_TTempMax[2] = _TScoreBox[i][j];

					_TPMax[3] = _TPMax[2];
					_TPMax[2] = cvPoint(i,j);
				}
				else if (_TTempMax[2] > _TScoreBox[i][j] && _TScoreBox[i][j] >= _TTempMax[3]) {
					_TTempMax[3] = _TScoreBox[i][j];

					_TPMax[3] = cvPoint(i,j);
				}
			}
		}
	}

	// 4개에 할당.
	for (register int i = 0; i < 4; i++) {
		Result[i] = _TPMax[i];
	}
}

void CheckInChessboard::Calculate_BoardScore(IplImage *BinaryImage, IplImage *GrayImage, vector<ChessPoint> CrossPoint, float ScoreBox[][8]) {
	float _TChess_Area[8][8]; // 체스 영역 저장 배열.
	float _TChess_gray[8][8]; // 체스 그리드의 평균 색상값 저장
	int _TChess_Blob[8][8];		// 체스 내부 blob의 갯수를 잡음
	unsigned char _kernel[PIXEL_PICK_KERNEL_SIZE][PIXEL_PICK_KERNEL_SIZE]; // 픽셀값을 취할 커널

	// 각 체스 영역 면적 계산부
	for (register int i = 0; i < 8; i++) {
		for (register int j = 0; j < 8; j++) {
			int cross_idx = i + (j * 9);
			CvPoint Head_point = CrossPoint.at(cross_idx).Cordinate;
			CvPoint Head_right = CrossPoint.at(cross_idx + 1).Cordinate;
			CvPoint Head_down = CrossPoint.at(cross_idx + 9).Cordinate;
			CvPoint right_down = CrossPoint.at(cross_idx + 10).Cordinate;
			ScoreBox[i][j] = 0;
			_TChess_Area[i][j] = Get_TriangleArea(Head_point, Head_right, Head_down) + Get_TriangleArea(Head_right, right_down, Head_down);
			_TChess_gray[i][j] = 0;
			_TChess_Blob[i][j] = 0;
		}
	}

	// 체스보드 내부 전체 평균을 계산
	float Chess_avg_pixvalue; 

	int count = 0;
	long pix_total = 0;
	for(register int i = 0; i < GrayImage->width; i++){
		for(register int j = 0; j < GrayImage->height; j++){
			unsigned char pixel_value = (unsigned char)GrayImage->imageData[i + j * GrayImage->widthStep];

			if(pixel_value != 0){
				count++;
				pix_total += pixel_value;
			}
		}
	}

	//CvScalar _Avg_scalar = cvAvg(GrayImage);
	Chess_avg_pixvalue = /*_Avg_scalar.val[0]*/pix_total / count;

	//BlobLabeling
	//_Blob.SetParam(BinaryImage, 1);
	_Blob.Set_Parameter(BinaryImage, 1);
	//_Blob.DoLabeling();
	_Blob.Go_Labeling();

	CvPoint _center, _idx;
	float _AvgPixValue;
	for (register int i = 0; i < _Blob._LabelingQty; i++) {
		_center = cvPoint(_Blob._LabelingInfomation[i].x + _Blob._LabelingInfomation[i].width/2, _Blob._LabelingInfomation[i].y + _Blob._LabelingInfomation[i].height/2);
		//_idx = Get_ChessboxPos(_center.x, _center.y, CrossPoint);
		_idx = Get_ChessBoxPosition(_center.x, _center.y, CrossPoint);

		_AvgPixValue = /*Get_AvgRect(GrayImage, BinaryImage, _Blob._LabelingInfomation[i])*/Get_MedianRect(GrayImage, _Blob._LabelingInfomation[i]);
		_TChess_Blob[_idx.x][_idx.y]++;
		_TChess_gray[_idx.x][_idx.y] += _AvgPixValue;

		if (_AvgPixValue > Chess_avg_pixvalue) {
			cvDrawRect(GrayImage, cvPoint(_Blob._LabelingInfomation[i].x, _Blob._LabelingInfomation[i].y), cvPoint(_Blob._LabelingInfomation[i].x + _Blob._LabelingInfomation[i].width, _Blob._LabelingInfomation[i].y + _Blob._LabelingInfomation[i].height), cvScalarAll(255), -1);
		}
		else {
			cvDrawRect(GrayImage, cvPoint(_Blob._LabelingInfomation[i].x, _Blob._LabelingInfomation[i].y), cvPoint(_Blob._LabelingInfomation[i].x + _Blob._LabelingInfomation[i].width, _Blob._LabelingInfomation[i].y + _Blob._LabelingInfomation[i].height), cvScalarAll(0), -1);
		}		
	}

	for(register int i = 0; i < 8; i++){
		for(register int j = 0; j < 8; j++){
			_TChess_gray[i][j] /= _TChess_Blob[i][j];
		}
	}

	// 차영상 면적 계산부.
	for (register int i = 0; i < BinaryImage->width; i++) {
		for (register int j = 0; j < BinaryImage->height; j++) {
			unsigned char pixel_value = BinaryImage->imageData[i + (j * BinaryImage->widthStep)];

			// 어느 좌표에 위치하는지 확인.
			if (pixel_value != 0) {

				CvPoint chessbox_pos = Get_ChessBoxPosition(i, j, CrossPoint);
				if (chessbox_pos.x != -1 || chessbox_pos.y != -1){
					if(_TChess_gray[chessbox_pos.x][chessbox_pos.y] > Chess_avg_pixvalue)
						ScoreBox[chessbox_pos.x][chessbox_pos.y] += 1;
					else
						ScoreBox[chessbox_pos.x][chessbox_pos.y] -= 1;
				}
			}
		}
	}

	for (register int i = 0 ; i < 8; i++) {
		for (register int j = 0; j < 8; j++) {
			ScoreBox[i][j] /= _TChess_Area[i][j];
		}
	}
}

void CheckInChessboard::Delete_Chessboard(IplImage *Image, vector<ChessPoint> Point) {
	CvPoint _T_LeftTop, _T_LeftBottom, _T_RightTop, _T_RightBottom; // 왼쪽 위, 왼쪽 아래, 오른쪽 위 오른쪽 아래.

	// tArea : chessboard 전체 넓이, 나머지 : 삼각형 넓이.
	float _TTriangleArea, _TTriangle1Area, _TTriangle2Area, _TTriangle3Area, _TTriangle4Area;

	// chessboard의 코너점을 구함.
	_T_LeftTop = Point.at(0).Cordinate;
	_T_RightTop = Point.at(8).Cordinate;
	_T_LeftBottom = Point.at(72).Cordinate;
	_T_RightBottom = Point.at(80).Cordinate;

	// 코너점을 이용하여 chessboard의 넓이 연산.
	_TTriangleArea = Get_TriangleArea(_T_LeftTop, _T_LeftBottom, _T_RightTop) + Get_TriangleArea(_T_RightTop, _T_RightBottom, _T_LeftBottom);

	// binary image에 존재하는 모든 픽셀을 대상으로 각 코너점과 이루는 삼각형의 넓이를 연산.
	for (register int i = 0; i < Image->width; i++) {
		for (register int j = 0; j < Image->height; j++) {
			unsigned char pixel_value = (unsigned char)Image->imageData[i + j * Image->widthStep];

			if (pixel_value != 0) {
				// Detection된 임의의 픽셀 좌표와 체스판의 코너점으로 이루어진 삼각형 네개의 넓이를 연산.
				_TTriangle1Area = Get_TriangleArea(cvPoint(i,j), _T_LeftTop, _T_RightTop);
				_TTriangle2Area = Get_TriangleArea(cvPoint(i,j), _T_RightTop, _T_RightBottom);
				_TTriangle3Area = Get_TriangleArea(cvPoint(i,j), _T_RightBottom, _T_LeftBottom);
				_TTriangle4Area = Get_TriangleArea(cvPoint(i,j), _T_LeftBottom, _T_LeftTop);

				float _TTotalArea = _TTriangle1Area + _TTriangle2Area + _TTriangle3Area + _TTriangle4Area;

				// 각 삼각형의 합이 chessboard의 넓이와 같다면 return true; => 픽셀이 chessboard 내부에 존재함.
				if (fabs(_TTriangleArea - _TTotalArea) >= 2)
					Image->imageData[i + j * Image->widthStep] = 0;
			}
		}
	}
}
#pragma endregion Public Functions