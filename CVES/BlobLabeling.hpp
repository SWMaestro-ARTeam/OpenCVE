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

#ifndef _BlobLabeling_hpp_
#define _BlobLabeling_hpp_

#include "Common.hpp"

#include "CVESDependent.hpp"

class BlobLabeling {
private:
	IplImage *_LabelingImage; // 레이블링을 위한 이미지
//<<<<<<< HEAD
	//CvRect *_LabelingInfomation; // 각 레이블 정보
	Visited *_VisitPoint; // 레이블링시 방문정보
//=======
	
//	Visited *m_vPoint; // 레이블링시 방문정보
//>>>>>>> origin/CVES_NewEngine_Extended
	
	int _LabelingThreshold; // 레이블링 스레스홀드 값
	int _ROI_Width, _ROI_Height; // image 관심영역에 대한 사이즈

	bool _IsInitialized;

	void Initialize_VisitPoint(int nWidth, int nHeight); // VisitPoint 초기화.
	void DeletevPoint();
	// _Labling 내부 사용 함수.
	int NRFIndNeighbor(unsigned char *DataBuf, int nWidth, int nHeight, int nPosX, int nPosY, int *StartX, int *StartY, int *EndX, int *EndY);
	int Area(unsigned char *DataBuf, int StartX, int StartY, int EndX, int EndY, int nWidth, int nLevel);

	int Labeling(unsigned char *DataBuf, int nWidth, int nHeight, int nThreshold); // 레이블링(실제 알고리즘).
	int LabelingActivate(IplImage *image, int nThreshold); // 레이블링(동작).
	void DetectLabelingRegion(int nLabelNumber, unsigned char *DataBuf, int nWidth, int nHeight); // 레이블링 결과 얻기.

public:
	BlobLabeling();
	~BlobLabeling();

//<<<<<<< HEAD
	void Get_SideBlob(IplImage *TargetImage, std::vector<int> *PieceIndex, IplImage *OtherImage); // 벽에 붙은 이미지 외에 other에 저장.
	void Set_Parameter(IplImage *TargetImage, int Threshold); // 레이블링 이미지 선택.
	void DrawLabel(IplImage *img, CvScalar RGB); // 레이블 그리기.	
	void Go_Labeling(); // 레이블링(실행).
//=======
	CvRect *_LabelingInfomation; // 각 레이블 정보
	int _LabelingQty; // 레이블의 갯수

	/*
	// 레이블링 이미지 선택
	void SetParam(IplImage *image, int nThreshold);
	// 레이블링(실행)
	void DoLabeling();
	// 레이블 그리기
	void DrawLabel(IplImage *img, CvScalar RGB);
	// 벽에 붙은 이미지 외에 other에 저장
	void GetSideBlob(IplImage *img, std::vector<int> *piece_idx, IplImage *other);
>>>>>>> origin/CVES_NewEngine_Extended
*/
	void Initialize_BlobLabeling();
	void Deinitialize_BlobLabeling();
};
#endif
