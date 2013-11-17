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

#include "BlobLabeling.hpp"

#pragma region Constructor & Destructor
BlobLabeling::BlobLabeling() {
	_IsInitialized = false;
}

BlobLabeling::~BlobLabeling() {
	_IsInitialized = false;
}
#pragma endregion Constructor & Destructor

#pragma region Private Functions
void BlobLabeling::Initialize_VisitPoint(int nWidth, int nHeight) {
	_VisitPoint = new Visited[nWidth * nHeight];

	for(int nY = 0; nY < nHeight; nY++) {
		for(int nX = 0; nX < nWidth; nX++) {
			_VisitPoint[(nY * nWidth) + nX].bVisitedFlag = false;
			_VisitPoint[(nY * nWidth) + nX].ptReturnPoint.x	= nX;
			_VisitPoint[(nY * nWidth) + nX].ptReturnPoint.y	= nY;
		}
	}
}

void BlobLabeling::DeletevPoint() {
	delete _VisitPoint;
}

int BlobLabeling::NRFIndNeighbor(unsigned char *DataBuf, int nWidth, int nHeight, int nPosX, int nPosY, int *StartX, int *StartY, int *EndX, int *EndY) {
	// Blob Labeling을 실제 행하는 function.
	// 2000년 정보처리학회에 실린 논문 참조.
	CvPoint _TCurrentPoint;

	_TCurrentPoint.x = nPosX;
	_TCurrentPoint.y = nPosY;

	_VisitPoint[(_TCurrentPoint.y * nWidth) +  _TCurrentPoint.x].bVisitedFlag = true;
	_VisitPoint[(_TCurrentPoint.y * nWidth) +  _TCurrentPoint.x].ptReturnPoint.x = nPosX;
	_VisitPoint[(_TCurrentPoint.y * nWidth) +  _TCurrentPoint.x].ptReturnPoint.y = nPosY;

	while (1) {
		// -X 방향
		if ((_TCurrentPoint.x != 0) && (DataBuf[(_TCurrentPoint.y * nWidth) + _TCurrentPoint.x - 1] == 255)) {
			if (_VisitPoint[(_TCurrentPoint.y * nWidth) +  _TCurrentPoint.x - 1].bVisitedFlag == false) {
				DataBuf[(_TCurrentPoint.y * nWidth) + _TCurrentPoint.x - 1] = DataBuf[(_TCurrentPoint.y * nWidth) + _TCurrentPoint.x];	// If so, mark it
				_VisitPoint[(_TCurrentPoint.y * nWidth) + _TCurrentPoint.x - 1].bVisitedFlag = true;
				_VisitPoint[(_TCurrentPoint.y * nWidth) + _TCurrentPoint.x - 1].ptReturnPoint	= _TCurrentPoint;
				_TCurrentPoint.x--;

				if (_TCurrentPoint.x <= 0)
					_TCurrentPoint.x = 0;

				if (*StartX >= _TCurrentPoint.x)
					*StartX = _TCurrentPoint.x;

				continue;
			}
		}

		// -X 방향
		if ((_TCurrentPoint.x != nWidth - 1) && (DataBuf[(_TCurrentPoint.y * nWidth) + _TCurrentPoint.x + 1] == 255)) {
			if (_VisitPoint[(_TCurrentPoint.y * nWidth) +  _TCurrentPoint.x + 1].bVisitedFlag == false) {

				DataBuf[(_TCurrentPoint.y * nWidth) + _TCurrentPoint.x + 1]	= DataBuf[(_TCurrentPoint.y * nWidth) + _TCurrentPoint.x];	// If so, mark it
				_VisitPoint[(_TCurrentPoint.y * nWidth) + _TCurrentPoint.x + 1].bVisitedFlag	= true;
				_VisitPoint[(_TCurrentPoint.y * nWidth) + _TCurrentPoint.x + 1].ptReturnPoint	= _TCurrentPoint;
				_TCurrentPoint.x++;

				if (_TCurrentPoint.x >= nWidth - 1)
					_TCurrentPoint.x = nWidth - 1;

				if (*EndX <= _TCurrentPoint.x)
					*EndX = _TCurrentPoint.x;

				continue;
			}
		}

		// -X 방향
		if ((_TCurrentPoint.y != 0) && (DataBuf[((_TCurrentPoint.y - 1) * nWidth) + _TCurrentPoint.x] == 255)) {
			if (_VisitPoint[((_TCurrentPoint.y - 1) * nWidth) +  _TCurrentPoint.x].bVisitedFlag == false) {
				DataBuf[((_TCurrentPoint.y - 1) * nWidth) + _TCurrentPoint.x] = DataBuf[(_TCurrentPoint.y * nWidth) + _TCurrentPoint.x];	// If so, mark it
				_VisitPoint[((_TCurrentPoint.y - 1) * nWidth) + _TCurrentPoint.x].bVisitedFlag = true;
				_VisitPoint[((_TCurrentPoint.y - 1) * nWidth) + _TCurrentPoint.x].ptReturnPoint = _TCurrentPoint;
				_TCurrentPoint.y--;

				if(_TCurrentPoint.y <= 0)
					_TCurrentPoint.y = 0;

				if(*StartY >= _TCurrentPoint.y)
					*StartY = _TCurrentPoint.y;

				continue;
			}
		}

		// -X 방향
		if ((_TCurrentPoint.y != nHeight - 1) && (DataBuf[((_TCurrentPoint.y + 1) * nWidth) + _TCurrentPoint.x] == 255)) {
			if (_VisitPoint[((_TCurrentPoint.y + 1) * nWidth) +  _TCurrentPoint.x].bVisitedFlag == false) {
				DataBuf[((_TCurrentPoint.y + 1) * nWidth) + _TCurrentPoint.x] = DataBuf[(_TCurrentPoint.y * nWidth) + _TCurrentPoint.x];	// If so, mark it
				_VisitPoint[((_TCurrentPoint.y + 1) * nWidth) +  _TCurrentPoint.x].bVisitedFlag	= true;
				_VisitPoint[((_TCurrentPoint.y + 1) * nWidth) +  _TCurrentPoint.x].ptReturnPoint = _TCurrentPoint;
				_TCurrentPoint.y++;

				if (_TCurrentPoint.y >= nHeight - 1)
					_TCurrentPoint.y = nHeight - 1;

				if (*EndY <= _TCurrentPoint.y)
					*EndY = _TCurrentPoint.y;

				continue;
			}
		}

		if ((_TCurrentPoint.x == _VisitPoint[(_TCurrentPoint.y * nWidth) + _TCurrentPoint.x].ptReturnPoint.x) 
			&& (_TCurrentPoint.y == _VisitPoint[(_TCurrentPoint.y * nWidth) + _TCurrentPoint.x].ptReturnPoint.y)) {
				break;
		}
		else {
			_TCurrentPoint = _VisitPoint[(_TCurrentPoint.y * nWidth) + _TCurrentPoint.x].ptReturnPoint;
		}
	}

	return 0;
}

int BlobLabeling::Area(unsigned char *DataBuf, int StartX, int StartY, int EndX, int EndY, int nWidth, int nLevel) {
	// 영역중 실제 blob의 칼라를 가진 영역의 크기를 획득
	int _TArea = 0;

	for (register int nY = StartY; nY < EndY; nY++)
		for (register int nX = StartX; nX < EndX; nX++) {
			if (DataBuf[(nY * nWidth) + nX] == nLevel)
				++_TArea;
		}

		return _TArea;
}

int BlobLabeling::Labeling(unsigned char *DataBuf, int nWidth, int nHeight, int nThreshold) {
	// Size가 nWidth이고 nHeight인 DataBuf에서 
	// nThreshold보다 작은 영역을 제외한 나머지를 blob으로 획득.
	int _TIndex = 0, _TNumber = 0;
	int _TStartX, _TStartY, _TEndX , _TEndY;

	// Find connected components
	for (register int nY = 0; nY < nHeight; nY++)	{
		for (register int nX = 0; nX < nWidth; nX++) {
			// Is this a new component?, 255 == Object
			if (DataBuf[(nY * nWidth) + nX] == 255) {
				_TNumber++;
				DataBuf[(nY * nWidth) + nX] = _TNumber;
				_TStartX = nX, _TStartY = nY, _TEndX = nX, _TEndY= nY;

				NRFIndNeighbor(DataBuf, nWidth, nHeight, nX, nY, &_TStartX, &_TStartY, &_TEndX, &_TEndY);

				if (Area(DataBuf, _TStartX, _TStartY, _TEndX, _TEndY, nWidth, _TNumber) < nThreshold) {
					for (register int k = _TStartY; k <= _TEndY; k++) {
						for (register int l = _TStartX; l <= _TEndX; l++)	{
							if (DataBuf[(k * nWidth) + l] == _TNumber)
								DataBuf[(k * nWidth) + l] = 0;
						}
					}
					--_TNumber;

					if (_TNumber > 250)
						return  0;
				}
			}
		}
	}

	return _TNumber;	
}

int BlobLabeling::LabelingActivate(IplImage *image, int nThreshold) {
	_ROI_Width = image->width;
	_ROI_Height = image->height;

	if (image->nChannels != 1)
		return 0;

	int _TNumber;
	int _TWidth = image->width;
	int _THeight = image->height;

	unsigned char *_TBuffer = new unsigned char[_TWidth * _THeight];

	for (register int j = 0; j < _THeight; j++)
		for (register int i = 0; i < _TWidth; i++) {
			_TBuffer[(j * _TWidth) + i] = (unsigned char)image->imageData[(j * image->widthStep) + i];
		}

		// 레이블링을 위한 포인트 초기화
		Initialize_VisitPoint(_TWidth, _THeight);

		// 레이블링
		_TNumber = Labeling(_TBuffer, _TWidth, _THeight, nThreshold);

		// 포인트 메모리 해제
		DeletevPoint();

		if (_TNumber != _DEF_MAX_BLOBS)
			_LabelingInfomation = new CvRect[_TNumber];

		if (_TNumber != 0)
			DetectLabelingRegion(_TNumber, _TBuffer, _TWidth, _THeight);

		for (register int j = 0; j < _THeight; j++)
			for (register int i = 0; i < _TWidth; i++) {

				image->imageData[(j * image->widthStep) + i] = _TBuffer[(j * _TWidth) + i];
			}

			delete _TBuffer;
			return _TNumber;
}

void BlobLabeling::DetectLabelingRegion(int nLabelNumber, unsigned char *DataBuf, int nWidth, int nHeight) {
	// Blob labeling해서 얻어진 결과의 rec을 얻어냄.
	int _TLabelIndex;
	bool _TFirstFlag[255] = {false, };

	for (register int nY = 1; nY < nHeight - 1; nY++) {
		for (register int nX = 1; nX < nWidth - 1; nX++) {
			_TLabelIndex = DataBuf[(nY * nWidth) + nX];

			// Is this a new component?, 255 == Object
			if (_TLabelIndex != 0) {
				if (_TFirstFlag[_TLabelIndex] == false) {
					_LabelingInfomation[_TLabelIndex - 1].x	= nX;
					_LabelingInfomation[_TLabelIndex - 1].y	= nY;
					_LabelingInfomation[_TLabelIndex - 1].width	= 0;
					_LabelingInfomation[_TLabelIndex - 1].height = 0;

					_TFirstFlag[_TLabelIndex] = true;
				}
				else {
					int _TLeft = _LabelingInfomation[_TLabelIndex - 1].x;
					int _TRight	= _TLeft + _LabelingInfomation[_TLabelIndex - 1].width;
					int _TTop	= _LabelingInfomation[_TLabelIndex - 1].y;
					int _TBottom = _TTop + _LabelingInfomation[_TLabelIndex - 1].height;

					if (_TLeft >= nX) _TLeft = nX;
					if (_TRight <= nX) _TRight = nX;
					if (_TTop >= nY) _TTop = nY;
					if (_TBottom <= nY) _TBottom = nY;

					_LabelingInfomation[_TLabelIndex - 1].x = _TLeft;
					_LabelingInfomation[_TLabelIndex - 1].y	= _TTop;
					_LabelingInfomation[_TLabelIndex - 1].width = _TRight - _TLeft;
					_LabelingInfomation[_TLabelIndex - 1].height = _TBottom - _TTop;
				}
			}
		}
	}
}
#pragma endregion Private Functions

#pragma region Pubilc Functions
void BlobLabeling::Get_SideBlob(IplImage *TargetImage, std::vector<int> *PieceIndex, IplImage *OtherImage) {
	if (_IsInitialized == true) {
		int _TIndex = 0;

		PieceIndex->clear();
		cvZero(OtherImage);

		for (register int i = 0; i < _LabelingQty; i++) {
			CvRect _TTempRect = _LabelingInfomation[i];
			_TTempRect.width++;
			_TTempRect.height++;

			if (_TTempRect.x == 1) {
				_TIndex = i;
				continue;
			}
			else if (_TTempRect.y == 1) {
				_TIndex = i;
				continue;
			}
			else if (_TTempRect.x + _TTempRect.width >= _ROI_Width - 1) {
				_TIndex = i;
				continue;
			}
			else if (_TTempRect.y + _TTempRect.height >= _ROI_Height - 1) {
				_TIndex = i;
				continue;
			}
			else {    //지우기
				/*for(int j = 0; j < temp.width; j++)
					for(int k = 0; k < temp.height; k++){
						unsigned char pixel = (unsigned char)img->imageData[(temp.x + j) + (temp.y + k) * img->widthStep];
						if(pixel != 0)
							img->imageData[(temp.x + j) + (temp.y + k) * img->widthStep] = 255;
					}*/
				cvSetImageROI(TargetImage, _TTempRect);
				cvSetImageROI(OtherImage, _TTempRect);
				cvCopy(TargetImage, OtherImage);
				cvZero(TargetImage);
				cvResetImageROI(TargetImage);
				cvResetImageROI(OtherImage);

				PieceIndex->push_back(i);
			}
		}

		for (register int i = 0; i < TargetImage->width; i++) {
			for (register int j = 0; j < TargetImage->height; j++) {
				if (_LabelingInfomation[_TIndex].x <= i 
					&& _LabelingInfomation[_TIndex].x + _LabelingInfomation[_TIndex].width >= i 
					&& _LabelingInfomation[_TIndex].y <= j
					&& _LabelingInfomation[_TIndex].y + _LabelingInfomation[_TIndex].height >= j)
					continue;
				else {
					TargetImage->imageData[i + (j * TargetImage->widthStep)];
				}
			}
		}
	}
	
}

void BlobLabeling::Set_Parameter(IplImage *TargetImage, int Threshold) {
	if (_IsInitialized == true) {
		if (_LabelingInfomation != NULL) {
			delete _LabelingInfomation;

			_LabelingInfomation = NULL;
			_LabelingQty = _DEF_MAX_BLOBS;
		}

		if (_LabelingImage != NULL)
			cvReleaseImage(&_LabelingImage);

		_LabelingImage	= cvCloneImage(TargetImage);

		_LabelingThreshold = Threshold;
	}
}

void BlobLabeling::DrawLabel(IplImage *Image, CvScalar RGB) {
	if (_IsInitialized == true) {
		//printf("n_blobs : %d\n", m_nBlobs);
		for (register int i = 0; i < _LabelingQty; i++) {
			//cvDrawCircle(img, cvPoint(m_recBlobs[i].x, m_recBlobs[i].y), 10, RGB);
			cvDrawRect(Image, cvPoint(_LabelingInfomation[i].x, _LabelingInfomation[i].y),
				cvPoint(_LabelingInfomation[i].x + _LabelingInfomation[i].width, _LabelingInfomation[i].y + _LabelingInfomation[i].height), RGB);
		}
	}
}

void BlobLabeling::Go_Labeling() {
	if (_IsInitialized == true) {
		_LabelingQty = LabelingActivate(_LabelingImage, _LabelingThreshold);
	}
}

void BlobLabeling::Initialize_BlobLabeling() {
	_IsInitialized = true;
	_LabelingThreshold = 0;
	_LabelingQty = _DEF_MAX_BLOBS;
	_LabelingImage	= NULL;
	_LabelingInfomation = NULL;
}

void BlobLabeling::Deinitialize_BlobLabeling() {
	if (_LabelingImage != NULL)
		cvReleaseImage(&_LabelingImage);

	if (_LabelingInfomation != NULL) {
		delete _LabelingInfomation;
		_LabelingInfomation = NULL;
	}
	
	_IsInitialized = false;
}
#pragma endregion Pubilc Functions