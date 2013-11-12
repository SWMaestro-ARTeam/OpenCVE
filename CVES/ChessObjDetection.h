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

#include "SystemDependency.hpp"
#include "CVESDependent.hpp"
#include "CheckInChessboard.hpp"

using namespace std;

class ChessObjDetection
{
private:
	CheckInChessboard *_CheckChessboard;

	int _Canny_LowThreshold, _Canny_HighThreshold;	// Canny Edge Detection에 사용되는 Threshold
	int _low_bright, _high_bright;									// 이미지 밝기 조정에 사용되는 값
	int _thickness;																	// 체스보드 격자를 지워낼 두께
	int _score_threshold;														// 스코어 thresholding 변수
	int _sub_threshold;															// 움직임 Detection시 좌표를 반환할 차이 스코어 차이 threshold

	void add_CannyImg(IplImage *H_canny, IplImage *S_Canny, IplImage *dst); // 두가지 CannyEdge Detection OR 연산 이미지 생성
	void Delete_ChessLine(IplImage *edge, vector<_ChessPoint> _cross_point); // 디텍션된 엣지 영상에서 교점들을 사용하여 엣지들을 최소화함

	void Get_Edgeimg(IplImage *src, IplImage *dst); // RGB 영상을 입력하여 HSV의 엣지 영상을 생성
	void add_Canny(IplImage *H, IplImage *S, IplImage *dst); // H와 S 평면에 캐니엣지를 디텍션해서 합영상을 생성

	void Thresholding_score(float score[][8], float threshold); // 연산해낸 스코어에 경계화를 진행함
public:
	ChessObjDetection(void);
	~ChessObjDetection(void);

	void SetCannyThreshold(int Low, int High); // Canny Edge Detection Threshold 재설정
	void SetBright(int Low, int High);					// 이미지 밝기 조정 값 셋팅 
	void SetThickness(int thickness);						// 체스보드의 격자를 지워낼 두께 셋팅
	void SetSubThreshold(int threshold);			// 움직임 Detection시 사용되는 threshold.
	void SetScoreThreshold(int threshold);	// Score에 Thresholding

	void DetectScore(IplImage *src, vector<_ChessPoint> _cross_point, float score_out[][8]); // RGB영상과 좌표를 출력을 이용하여 오브젝트의 좌표를 디텍션
	void DetectMovement(float score_before[][8], float score_after[][8], CvPoint out[]);	// 스코어의 차이를 보고 좌표를 반환함.
	void Get_Movement(IplImage *before, IplImage *after, vector<_ChessPoint> _cross_point, CvPoint out[]); // 이전 영상과 이후 영상으로 좌표의 차이를 구함
	void Detect_SobelCannyScore(IplImage *src, vector<_ChessPoint> _cross_point, float score_out[][8]); // RGB영상과 좌표를 출력을 이용하여 오브젝트의 좌표를 디텍션
};

