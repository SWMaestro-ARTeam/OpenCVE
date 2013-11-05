//////////////////////////////////////////////////////////////////////////////////////////////
//	The OpenCVE Project.
//
//	The MIT License (MIT)
//	Copyright ? 2013 {Doohoon Kim, Sungpil Moon, Kyuhong Choi} at AR Team of SW Maestro 4th
//	{invi.dh.kim, munsp9103, aiaipming} at gmail.com
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy of
//	this software and associated documentation files (the ��Software��), to deal
//	in the Software without restriction, including without limitation the rights to
//	use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
//	the Software, and to permit persons to whom the Software is furnished to do so,
//	subject to the following conditions:
//
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED ��AS IS��, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
//	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
//	PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
//	LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
//	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
//	OR OTHER DEALINGS IN THE SOFTWARE.
//////////////////////////////////////////////////////////////////////////////////////////////
#include "CheckInChessboard.h"

CheckInChessboard::CheckInChessboard(void)
{
}

CheckInChessboard::~CheckInChessboard(void)
{
}

float CheckInChessboard::area_tri(CvPoint p, CvPoint q, CvPoint r) {
	// 2���� ��ǥ p,q,r�� �����Ǵ� �ﰢ���� ���̸� ����.
	return (float)abs(((p.x * q.y) + (q.x * r.y) + (r.x * p.y)) - ((p.y * q.x) + (q.y * r.x) + (r.y * p.x))) / 2.0;
}

bool CheckInChessboard::Check_InChessboard(IplImage *img, vector<ChessPoint> point){
	// Chess_point�� ���Ͽ� binary image�� �ȼ��� chess board ���ο� �����ϴ����� Ȯ��.
	// 1) ü���� �簢���� ũ�⸦ �����Ͽ� _TTriangleArea�� ����
	// 2) ������ ��� Detection�� �ȼ��� ��ǥ�� ü���� �簢���� ������� �̷���� �ﰢ���� ���̸� ���� ���
	// 3) 2�� �������� ����� �ﰢ�� �װ��� ���� ü���� �簢���� ũ��� ���̰� ���� ��� �ȼ��� ��ǥ�� �簢�� ���ο� �������� ����
	// 4) �簢�� ������ �ȼ� ��ǥ�� �ִٴ� ������ �Ǹ� return true;

	CvPoint _T_LH, _T_LL, _T_RH, _T_RL; // ���� ��, ���� �Ʒ�, ������ �� ������ �Ʒ�.

	// tArea : chessboard ��ü ����, ������ : �ﰢ�� ����.
	float _TTriangleArea, _TTriangle1Area, _TTriangle2Area, _TTriangle3Area, _TTriangle4Area;

	// chessboard�� �ڳ����� ����.
	_T_LH = point.at(0).Cordinate;
	_T_RH = point.at(8).Cordinate;
	_T_LL = point.at(72).Cordinate;
	_T_RL = point.at(80).Cordinate;

	// �ڳ����� �̿��Ͽ� chessboard�� ���� ����.
	_TTriangleArea = area_tri(_T_LH, _T_LL, _T_RH) + area_tri(_T_RH, _T_RL, _T_LL);

	// binary image�� �����ϴ� ��� �ȼ��� ������� �� �ڳ����� �̷�� �ﰢ���� ���̸� ����.
	for (register int i = 0; i < img->width; i++) {
		for (register int j = 0; j < img->height; j++) {
			unsigned char pixel_value = (unsigned char)img->imageData[i + j * img->widthStep];

			if (pixel_value == 255) {
				// Detection�� ������ �ȼ� ��ǥ�� ü������ �ڳ������� �̷���� �ﰢ�� �װ��� ���̸� ���� 
				_TTriangle1Area = area_tri(cvPoint(i,j), _T_LH, _T_RH);
				_TTriangle2Area = area_tri(cvPoint(i,j), _T_RH, _T_RL);
				_TTriangle3Area = area_tri(cvPoint(i,j), _T_RL, _T_LL);
				_TTriangle4Area = area_tri(cvPoint(i,j), _T_LL, _T_LH);

				float totalArea = _TTriangle1Area + _TTriangle2Area + _TTriangle3Area + _TTriangle4Area;

				// �� �ﰢ���� ���� chessboard�� ���̿� ���ٸ� return true; => �ȼ��� chessboard ���ο� ������.
				if (fabs(_TTriangleArea - totalArea) < 2)
					return true;
			}
		}
	}

	// binary image�� �ȼ��� chessboard ���ο� �������� ����
	return false;
}

bool CheckInChessboard::Check_imgZero(IplImage *img){
	// binary image�� �ȼ����� ��� 0������ Ȯ��
	unsigned char _TPixelValue;

	for (register int i = 0; i < img->width; i++)
		for (register int j = 0; j < img->height; j++) {
			_TPixelValue = (unsigned char)img->imageData[i + (j * img->widthStep)];

			if (_TPixelValue != 0)
				return false;
		}

		return true;
}

CvPoint CheckInChessboard::Get_Chessidx(CvPoint point, vector<ChessPoint> cross_point){
	// chessboard�� ������ index�� �ο�.
	// left-top�� ������ ��ǥ��� index�� �ο�
	for (register int i = 0; i < cross_point.size() - 10; i++) {
		if (cross_point.at(i).Cordinate.x <= point.x && cross_point.at(i).Cordinate.y <= point.y) {
			if (cross_point.at(i + 10).Cordinate.x > point.x && cross_point.at(i + 10).Cordinate.y > point.y)
				return cross_point.at(i).index;
		}
	}
	return cvPoint(-1, -1);
}

CvPoint CheckInChessboard::Get_ChessboxPos(int width, int height, vector<ChessPoint> cross_point) {
	// width, height�� ����Ű�� �ȼ��� ��� ü������ �ε����� ���������� ����Ͽ� ��ȯ.
	// ������ ����� ����ȭ�� �̹����� true ���� ������ �ȼ��� ��ǥ�� ü������ �ε��� ��ȯ.
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
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

void CheckInChessboard::Calculate_Movement(IplImage *bin, vector<ChessPoint> cross_point, CvPoint *out1, CvPoint *out2){
	// �������� ��� �̹����� �̿��Ͽ� ü�������� score�� �ο�.
	// score / ���� �� �̿��Ͽ� ���� ���� ���� �� ��ǥ�� ��ȯ.
	float score_box[8][8]; // �������� ���� �迭.
	float chess_area[8][8]; // ü�� ���� ���� �迭.
	const float score_threshold = 0.1; // ���� ���� threshold.

	// �� ü�� ���� ���� ����
	for(int i = 0; i < 8; i++){
		for(int j = 0; j < 8; j++){
			int cross_idx = i + (j * 9);
			CvPoint Head_point = cross_point.at(cross_idx).Cordinate;
			CvPoint Head_right = cross_point.at(cross_idx + 1).Cordinate;
			CvPoint Head_down = cross_point.at(cross_idx + 9).Cordinate;
			CvPoint right_down = cross_point.at(cross_idx + 10).Cordinate;
			score_box[i][j] = 0;
			chess_area[i][j] = area_tri(Head_point, Head_right, Head_down) + area_tri(Head_right, right_down, Head_down);
		}
	}

	// ������ ���� ����.
	for (int i = 0; i < bin->width; i++){
		for (int j = 0; j < bin->height; j++){
			unsigned char pixel_value = bin->imageData[i + (j * bin->widthStep)];

			// ��� ��ǥ�� ��ġ�ϴ��� Ȯ��.
			if (pixel_value != 0){
				CvPoint chessbox_pos = Get_ChessboxPos(i, j, cross_point);
				if (chessbox_pos.x != -1 || chessbox_pos.y != -1)
					score_box[chessbox_pos.x][chessbox_pos.y]++;
			}
		}
	}

	// ���ھ �������� �����༭ ������ ����.
	// ���� ������ ū �� ��ǥ�� ����.
	float temp_max1, temp_max2;
	CvPoint p_max1, p_max2;

	temp_max1 = temp_max2 = -1.0;
	p_max1 = p_max2 = cvPoint(-1, -1);
	for (int i = 0 ; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			score_box[i][j] /= chess_area[i][j];

			if (score_box[i][j] >= score_threshold) {
				if (temp_max1 <= score_box[i][j]) {
					temp_max2 = temp_max1;
					p_max2 = p_max1;
					temp_max1 = score_box[i][j];
					p_max1 = cvPoint(i, j);
				}
				else if (temp_max1 > score_box[i][j] && score_box[i][j] > temp_max2) {
					temp_max2 = score_box[i][j];
					p_max2 = cvPoint(i, j);
				}
			}
		}
	}

	*out1 = p_max1, *out2 = p_max2;
}