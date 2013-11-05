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

#include "CVESDependent.hpp"

using namespace std;

class CheckInChessboard
{
private:
	// p,q,r�� �̷���� �ﰢ���� ���� return
	float area_tri(CvPoint p, CvPoint q, CvPoint r);
	// ���� ��� ü���ǿ� �ִ����� üũ.
	CvPoint	Get_Chessidx(CvPoint point, vector<ChessPoint> cross_point);
	// width, height�� ����Ű�� �ȼ��� ��� ü������ �ε����� ���������� ����Ͽ� ��ȯ.
	CvPoint Get_ChessboxPos(int width, int height, vector<ChessPoint> cross_point);

public:
	CheckInChessboard(void);
	~CheckInChessboard(void);

	// binary image�� ü������ �ȿ� �ȼ��� �������� �˻�.
	bool Check_InChessboard(IplImage *img, vector<ChessPoint> point);
	// img�� �ȼ����� �ƹ��͵� ������ �ʴ��� üũ.
	bool Check_imgZero(IplImage *img);
	// �������� ����� ���� ���� �̹����� ����Ͽ� ü������ ��ǥ�̵��� ��ȯ.
	void Calculate_Movement(IplImage *bin, vector<ChessPoint> cross_point, CvPoint *out1, CvPoint *out2);
};

