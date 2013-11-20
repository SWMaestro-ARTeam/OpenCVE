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

#include "HoughLineBased.hpp"

#pragma region Private Functions
bool HoughLineBased::DescSortOfFirstValue(pair<float, float> Ta, pair<float, float> Tb) {
	// 각도로 정렬.
	return Ta.first < Tb.first;
}
#pragma endregion Private Functions

#pragma region Public Functions
// 이웃들의 값을 살펴보고 조건에 해당하지 않으면 지움.
void HoughLineBased::NonMaximumSuppression(IplImage* Image1, IplImage* Image2, int Kernel) {
	//조건은 이웃값보다 작음
	float neighbor, neighbor2;

	for (register int y = 0; y < Image1->height; y++)	{
		for (register int x = 0; x < Image1->width; x++) {
			float intensity = CV_IMAGE_ELEM(Image1, float, y, x);

			if (intensity > 0) {
				int flag = 0;

				for (register int ky = -Kernel; ky <= Kernel; ky++) {
					// in y-direction.
					if (y + ky < 0 || y + ky >= Image1->height) {
						// border check.
						continue;
					}
					// in x-direction.
					for (register int kx = -Kernel; kx <= Kernel; kx++) {
						if (x + kx < 0 || x + kx >= Image1->width) {
							// border check.
							continue;
						}
						neighbor = CV_IMAGE_ELEM(Image1, float, y + ky, x + kx);
						neighbor2 = CV_IMAGE_ELEM(Image2, float, y + ky, x + kx);

						if (intensity < neighbor || intensity < neighbor2) {
							CV_IMAGE_ELEM(Image1, float, y, x) = 0.0;
							flag = 1;
							break;
						}
					}
					if (1 == flag) {
						break;
					}
				}
			}
			else {
				CV_IMAGE_ELEM(Image1, float, y, x) = 0.0;
			}
		}  
	}
}

void HoughLineBased::CastSequence(CvSeq* linesX, CvSeq* linesY) {
	// CvSeq 객체를 rho, theta로 이루이전 직선으로 변환 후, stl vector에 저장.
	_Vector_LineX.clear();
	_Vector_LineY.clear();

	// X축 방향의 직선에 대하여 연산.
	for (register int i = 0; i < MIN(linesX->total, 100); i++)	{
		//cvSeq로 부터 rho, theta 연산 
		float *_TLine = (float *)cvGetSeqElem(linesX, i);
		float _TRho = _TLine[0];
		float _TTheta = _TLine[1];

		_Vector_LineX.push_back(pair<float, float>(_TRho, _TTheta));
	}

	// Y축 방향에 대하여 직선 연산.
	for (register int i = 0; i < MIN(linesY->total, 100); i++) {
		float *_TLine = (float*)cvGetSeqElem(linesY, i);
		float _TRho = _TLine[0];
		float _TTheta = _TLine[1];

		_Vector_LineY.push_back(pair<float, float>(_TRho, _TTheta));
	}
}

void HoughLineBased::MergeLine(vector<std::pair<float, float> > *Lines) {
	// Threshold를 기준으로 유사한 라인을 병합함.
	float _TSUB_UNDER = 0.0, _TSUB_MIN = 9999;
	vector<std::pair<float, float> > _TTemp;
	pair<int, int> _TPair_Min;

	//연산 초기화
	for (register int i = 0; i < Lines->size(); i++) {
		if (Lines->at(i).first < 0) {
			Lines->at(i).first = fabs(Lines->at(i).first);
			Lines->at(i).second = Lines->at(i).second - CV_PI;
		}
	}

	// 벡터 복수 & 직선 벡터를 각도로 정렬.
	copy(Lines->begin(), Lines->end(), back_inserter(_TTemp));
	sort(_TTemp.begin(), _TTemp.end(), DescSortOfFirstValue);

	// chessboard에 존재하는 직선의 갯수가 9개 이므로 9개가 남을때까지 병합.
	if (_TTemp.size() > 9) {
		for (register int i = 0; i < (int)Lines->size() - 9; i++) {
			_TSUB_MIN = 9999;		
			for (register int j = 1; j < _TTemp.size(); j++) {
				float SUB = _TTemp.at(j).first - _TTemp.at(j - 1).first;

				if (SUB < _TSUB_MIN) {
					_TSUB_MIN = SUB;
					_TPair_Min.first = j - 1;
					_TPair_Min.second = j;
				}
			}
			// vector의 first는 rho, second는 theta.
			_TTemp.at(_TPair_Min.first).first = (_TTemp.at(_TPair_Min.first).first + _TTemp.at(_TPair_Min.second).first) / 2.0;
			_TTemp.at(_TPair_Min.first).second = (_TTemp.at(_TPair_Min.first).second + _TTemp.at(_TPair_Min.second).second) / 2.0;

			_TTemp.erase(_TTemp.begin() + _TPair_Min.second);
		}

		Lines->clear();
		copy(_TTemp.begin(), _TTemp.end(), back_inserter(*Lines));
	}
}

void HoughLineBased::DrawLines(vector<pair<float, float> > Lines, IplImage* TargetImage) {
	// line fitting을 이용한 chessboard recognition의 경우 image에 라인을 그려줌.
	for (register int i = 0; i < MIN(Lines.size(), 100); i++) {
		// 호도법에 의해 표현된 line을 각 끝점으로 변환.
		float _TRho = Lines.at(i).first;
		float _TTheta = Lines.at(i).second;
		CvPoint _TPt1, _TPt2;
		double a = cos(_TTheta), b = sin(_TTheta);
		double _T_X0 = a * _TRho, _T_Y0 = b * _TRho; // 수직의 시작이 되는 점.

		_TPt1.x = cvRound(_T_X0 + (1000 * (-b))); // 끝까지로 쭉 그려버림.
		_TPt1.y = cvRound(_T_Y0 + (1000 * (a)));
		_TPt2.x = cvRound(_T_X0 - (1000 * (-b)));
		_TPt2.y = cvRound(_T_Y0 - (1000 * (a)));
		cvLine(TargetImage, _TPt1, _TPt2, CV_RGB(255, 0, 0), 1, 8);

		cvDrawCircle(TargetImage, cvPoint(_T_X0, _T_Y0), 3, cvScalar(255, 255), -1);
	}
}

void HoughLineBased::FindIntersections(vector<pair<float, float> > XLines, vector<pair<float, float> > YLines, vector<ChessPoint> *Point) {
	// line fitting을 이용한 chessboard recognition의 경우 각 라인의 교점을 연산.
	ChessPoint _TTemp_CP;

	Point->clear();

	// X축 방향의 line과 Y축 방향의 Line의 교점을 연산.
	for (register int i = 0; i < MIN(XLines.size(), 100); i++) {
		for (register int j = 0; j < MIN(YLines.size(), 100); j++)	{                
			float _TRhoX = XLines.at(i).first;
			float _TRhoY = YLines.at(j).first;
			float _TThetaX = XLines.at(i).second, thetaY = YLines.at(j).second;

			double _TaX = cos(_TThetaX), bX = sin(_TThetaX);
			double _TaY = cos(thetaY), bY = sin(thetaY);

			CvPoint _Tc; // the intersection point of lineX[i] and lineY[j].

			double _TCx = ((_TRhoX * bY) - (_TRhoY * bX)) / ((_TaX * bY) - (bX * _TaY));
			double _TCy = (_TRhoX - (_TaX * _TCx)) / bX;

			_Tc.x = cvRound(_TCx);
			_Tc.y = cvRound(_TCy);

			// save point.
			_TTemp_CP.Cordinate = _Tc;
			_TTemp_CP.Index = cvPoint(j, i);
			Point->push_back(_TTemp_CP);
		}
	}
}
#pragma endregion Public Functions