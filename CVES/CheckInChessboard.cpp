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

#include "CheckInChessboard.hpp"

CheckInChessboard::CheckInChessboard() {
}

CheckInChessboard::~CheckInChessboard() {
}

float CheckInChessboard::area_tri(CvPoint p, CvPoint q, CvPoint r) {
	// 2차원 좌표 p,q,r로 생성되는 삼각형의 넓이를 구함.
	return (float)abs(((p.x * q.y) + (q.x * r.y) + (r.x * p.y)) - ((p.y * q.x) + (q.y * r.x) + (r.y * p.x))) / 2.0;
}

bool CheckInChessboard::Check_InChessboard(IplImage *img, vector<ChessPoint> point) {
	// Chess_point를 통하여 binary image의 픽셀이 chess board 내부에 존재하는지를 확인.
	// 1) 체스판 사각형의 크기를 연산하여 _TTriangleArea에 저장
	// 2) 차영상 결과 Detection된 픽셀의 좌표와 체스판 사각형의 끝점들로 이루어진 삼각형의 넓이를 각각 계산
	// 3) 2번 과정에서 연산된 삼각형 네개의 합이 체스판 사각형의 크기와 차이가 적을 경우 픽셀의 좌표가 사각형 내부에 있음으로 판정
	// 4) 사각형 내부의 픽셀 좌표가 있다는 판정이 되면 return true;

	CvPoint _T_LH, _T_LL, _T_RH, _T_RL; // 왼쪽 위, 왼쪽 아래, 오른쪽 위 오른쪽 아래.

	// tArea : chessboard 전체 넓이, 나머지 : 삼각형 넓이.
	float _TTriangleArea, _TTriangle1Area, _TTriangle2Area, _TTriangle3Area, _TTriangle4Area;

	// chessboard의 코너점을 구함.
	_T_LH = point.at(0).Cordinate;
	_T_RH = point.at(8).Cordinate;
	_T_LL = point.at(72).Cordinate;
	_T_RL = point.at(80).Cordinate;

	// 코너점을 이용하여 chessboard의 넓이 연산.
	_TTriangleArea = area_tri(_T_LH, _T_LL, _T_RH) + area_tri(_T_RH, _T_RL, _T_LL);

	// binary image에 존재하는 모든 픽셀을 대상으로 각 코너점과 이루는 삼각형의 넓이를 연산.
	for (register int i = 0; i < img->width; i++) {
		for (register int j = 0; j < img->height; j++) {
			unsigned char pixel_value = (unsigned char)img->imageData[i + j * img->widthStep];

			if (pixel_value == 255) {
				// Detection된 임의의 픽셀 좌표와 체스판의 코너점으로 이루어진 삼각형 네개의 넓이를 연산.
				_TTriangle1Area = area_tri(cvPoint(i,j), _T_LH, _T_RH);
				_TTriangle2Area = area_tri(cvPoint(i,j), _T_RH, _T_RL);
				_TTriangle3Area = area_tri(cvPoint(i,j), _T_RL, _T_LL);
				_TTriangle4Area = area_tri(cvPoint(i,j), _T_LL, _T_LH);

				float totalArea = _TTriangle1Area + _TTriangle2Area + _TTriangle3Area + _TTriangle4Area;

				// 각 삼각형의 합이 chessboard의 넓이와 같다면 return true; => 픽셀이 chessboard 내부에 존재함.
				if (fabs(_TTriangleArea - totalArea) < 2)
					return true;
			}
		}
	}

	// binary image의 픽셀이 chessboard 내부에 존재하지 않음.
	return false;
}

bool CheckInChessboard::Check_imgZero(IplImage *img) {
	// binary image에 픽셀값이 모드 0인지를 확인
	unsigned char _TPixelValue;

	for (register int i = 0; i < img->width; i++)
		for (register int j = 0; j < img->height; j++) {
			_TPixelValue = (unsigned char)img->imageData[i + (j * img->widthStep)];

			if (_TPixelValue != 0)
				return false;
		}

		return true;
}

CvPoint CheckInChessboard::Get_Chessidx(CvPoint point, vector<ChessPoint> cross_point) {
	// chessboard의 교점의 index를 부여.
	// left-top이 원점인 좌표계로 index를 부여
	for (register int i = 0; i < cross_point.size() - 10; i++) {
		if (cross_point.at(i).Cordinate.x <= point.x && cross_point.at(i).Cordinate.y <= point.y) {
			if (cross_point.at(i + 10).Cordinate.x > point.x && cross_point.at(i + 10).Cordinate.y > point.y)
				return cross_point.at(i).index;
		}
	}
	return cvPoint(-1, -1);
}

CvPoint CheckInChessboard::Get_ChessboxPos(int width, int height, vector<ChessPoint> cross_point) {
	// width, height가 가리키는 픽셀이 어느 체스보드 인덱스를 가지는지를 계산하여 반환.
	// 차영상 결과로 이진화된 이미지의 true 값을 가지는 픽셀의 좌표로 체스판의 인덱스 반환.
	for (register int i = 0; i < 8; i++) {
		for (register int j = 0; j < 8; j++) {
			CvPoint top_left = cross_point.at((i * 9) + j).Cordinate;
			CvPoint bot_right = cross_point.at((i * 9) + j + 10).Cordinate;

			if (top_left.x <= width && top_left.y <= height)
				if (bot_right.x > width && bot_right.y > height)
					return cvPoint(j, i);
		}
	}

	//error return;
	return cvPoint(-1,-1);
}

void CheckInChessboard::Calculate_Movement(IplImage *bin, vector<ChessPoint> cross_point, CvPoint out[]) {
	// 차영상의 결과 이미지를 이용하여 체스보드의 score를 부여.
	// score / 면적 을 이용하여 가장 많이 변한 두 좌표를 반환.
	float score_box[8][8]; // 면적비율 저장 배열.
	float chess_area[8][8]; // 체스 영역 저장 배열.
	const float score_threshold = 0.1; // 면적 비율 threshold.

	// 각 체스 영역 면적 계산부
	for (register int i = 0; i < 8; i++) {
		for (register int j = 0; j < 8; j++) {
			int cross_idx = i + (j * 9);
			CvPoint Head_point = cross_point.at(cross_idx).Cordinate;
			CvPoint Head_right = cross_point.at(cross_idx + 1).Cordinate;
			CvPoint Head_down = cross_point.at(cross_idx + 9).Cordinate;
			CvPoint right_down = cross_point.at(cross_idx + 10).Cordinate;
			score_box[i][j] = 0;
			chess_area[i][j] = area_tri(Head_point, Head_right, Head_down) + area_tri(Head_right, right_down, Head_down);
		}
	}

	// 차영상 면적 계산부.
	for (register int i = 0; i < bin->width; i++) {
		for (register int j = 0; j < bin->height; j++) {
			unsigned char pixel_value = bin->imageData[i + (j * bin->widthStep)];

			// 어느 좌표에 위치하는지 확인.
			if (pixel_value != 0) {
				CvPoint chessbox_pos = Get_ChessboxPos(i, j, cross_point);
				if (chessbox_pos.x != -1 || chessbox_pos.y != -1)
					score_box[chessbox_pos.x][chessbox_pos.y]++;
			}
		}
	}

	// 스코어를 면적으로 나눠줘서 비율을 구함.
	// 가장 비율이 큰 두 좌표를 리턴. -> num 개의 좌표를 리턴
	float temp_max[4];
	CvPoint p_max[4];

	//초기화
	for(int i = 0; i < 4; i++){
		p_max[i] = cvPoint(-1,-1);
		temp_max[i] = -1.0;
	}

	//0번지부터 큰순으로 연산한 결과 저장
	for (register int i = 0 ; i < 8; i++) {
		for (register int j = 0; j < 8; j++) {
			score_box[i][j] /= chess_area[i][j];

			if (score_box[i][j] >= score_threshold) {
				if(temp_max[0] <= score_box[i][j]){
					temp_max[3] = temp_max[2];
					temp_max[2] = temp_max[1];
					temp_max[1] = temp_max[0];
					temp_max[0] = score_box[i][j];

					p_max[3] = p_max[2];
					p_max[2] = p_max[1];
					p_max[1] = p_max[0];
					p_max[0] = cvPoint(i,j);
				}else if(temp_max[0] > score_box[i][j] && score_box[i][i] >= temp_max[1]){
					temp_max[3] = temp_max[2];
					temp_max[2] = temp_max[1];
					temp_max[1] = score_box[i][j];

					p_max[3] = p_max[2];
					p_max[2] = p_max[1];
					p_max[1] = cvPoint(i,j);
				}else if(temp_max[1] > score_box[i][j] && score_box[i][i] >= temp_max[2]){
					temp_max[3] = temp_max[2];
					temp_max[2] = score_box[i][j];

					p_max[3] = p_max[2];
					p_max[2] = cvPoint(i,j);
				}else if(temp_max[2] > score_box[i][j] && score_box[i][i] >= temp_max[3]){
					temp_max[3] = score_box[i][j];

					p_max[3] = cvPoint(i,j);
				}
			}
		}
	}

	//4개에 할당
	for(int i = 0; i < 4; i++){
		out[i] = p_max[i];
	}
}