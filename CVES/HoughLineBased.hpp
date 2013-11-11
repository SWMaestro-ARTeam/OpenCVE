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

#ifndef _HoughLine_hpp_
#define _HoughLine_hpp_

#include "SystemDependency.hpp"
#include "CVESDependent.hpp"

using namespace std;

class HoughLineBased {
private:
	static bool DescSortOfFirstValue(pair<float, float> Ta, pair<float, float> Tb); //벡터 정렬 규칙
public:
	vector<std::pair<float, float> > _Vector_LineX, _Vector_LineY; // 라인 : <rho, theta>

	void NonMaximumSuppression(IplImage* Image1, IplImage* Image2, int Kernel);
	void CastSequence(CvSeq *linesX, CvSeq *linesY); // 벡터로 변환
	void MergeLine(vector<std::pair<float, float> > *Lines); // Hough Line Trasform 결과를 체스판에 맞추어 병합.
	// 라인 return.
	//void Get_Line(vector<pair<float, float> > *XLines, vector<pair<float, float> > *YLines);
	// 라인 그리기.
	void DrawLines(vector<pair<float, float> > Lines, IplImage* TargetImage);
	// 교차점 구하기.
	void FindIntersections(vector<pair<float, float> > XLines, vector<pair<float, float> > YLines, vector<ChessPoint> *Point);
};

#endif