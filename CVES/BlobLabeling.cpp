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

BlobLabeling::BlobLabeling() {
	m_nThreshold = 0;
	m_nBlobs = _DEF_MAX_BLOBS;
	m_Image	= NULL;
	m_recBlobs = NULL;
}

BlobLabeling::~BlobLabeling() {
	if (m_Image != NULL)
		cvReleaseImage(&m_Image);

	if (m_recBlobs != NULL) {
		delete m_recBlobs;
		m_recBlobs = NULL;
	}
}

void BlobLabeling::SetParam(IplImage *image, int nThreshold) {
	if (m_recBlobs != NULL) {
		delete m_recBlobs;

		m_recBlobs = NULL;
		m_nBlobs = _DEF_MAX_BLOBS;
	}

	if (m_Image != NULL)
		cvReleaseImage(&m_Image);

	m_Image	= cvCloneImage(image);

	m_nThreshold = nThreshold;
}

void BlobLabeling::DoLabeling() {
	m_nBlobs = Labeling(m_Image, m_nThreshold);
}

int BlobLabeling::Labeling(IplImage *image, int nThreshold) {
	roi_width = image->width;
	roi_height = image->height;

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
	InitvPoint(_TWidth, _THeight);

	// 레이블링
	_TNumber = _Labeling(_TBuffer, _TWidth, _THeight, nThreshold);

	// 포인트 메모리 해제
	DeletevPoint();

	if (_TNumber != _DEF_MAX_BLOBS)
		m_recBlobs = new CvRect[_TNumber];

	if (_TNumber != 0)
		DetectLabelingRegion(_TNumber, _TBuffer, _TWidth, _THeight);

	for (register int j = 0; j < _THeight; j++)
		for (register int i = 0; i < _TWidth; i++) {

			image->imageData[(j * image->widthStep) + i] = _TBuffer[(j * _TWidth) + i];
		}

	delete _TBuffer;
	return _TNumber;
}

// m_vPoint 초기화 함수
void BlobLabeling::InitvPoint(int nWidth, int nHeight) {
	m_vPoint = new Visited[nWidth * nHeight];

	for(int nY = 0; nY < nHeight; nY++) {
		for(int nX = 0; nX < nWidth; nX++) {
			m_vPoint[(nY * nWidth) + nX].bVisitedFlag = false;
			m_vPoint[(nY * nWidth) + nX].ptReturnPoint.x	= nX;
			m_vPoint[(nY * nWidth) + nX].ptReturnPoint.y	= nY;
		}
	}
}

void BlobLabeling::DeletevPoint() {
	delete m_vPoint;
}

// Size가 nWidth이고 nHeight인 DataBuf에서 
// nThreshold보다 작은 영역을 제외한 나머지를 blob으로 획득
int BlobLabeling::_Labeling(unsigned char *DataBuf, int nWidth, int nHeight, int nThreshold) {
	int _TIndex = 0, _TNumber = 0;
	//int nX, nY, k, l;
	int _TStartX, _TStartY, _TEndX , _TEndY;
	
	// Find connected components
	for (register int nY = 0; nY < nHeight; nY++)	{
		for (register int nX = 0; nX < nWidth; nX++) {
			// Is this a new component?, 255 == Object
			if (DataBuf[(nY * nWidth) + nX] == 255) {
				_TNumber++;

				DataBuf[(nY * nWidth) + nX] = _TNumber;
				
				_TStartX = nX, _TStartY = nY, _TEndX = nX, _TEndY= nY;

				__NRFIndNeighbor(DataBuf, nWidth, nHeight, nX, nY, &_TStartX, &_TStartY, &_TEndX, &_TEndY);

				if (__Area(DataBuf, _TStartX, _TStartY, _TEndX, _TEndY, nWidth, _TNumber) < nThreshold) {
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

// Blob labeling해서 얻어진 결과의 rec을 얻어냄 
void BlobLabeling::DetectLabelingRegion(int nLabelNumber, unsigned char *DataBuf, int nWidth, int nHeight) {
	//int nX, nY;
	int _TLabelIndex;

	bool _TFirstFlag[255] = {false,};
	
	for (register int nY = 1; nY < nHeight - 1; nY++) {
		for (register int nX = 1; nX < nWidth - 1; nX++) {
			_TLabelIndex = DataBuf[(nY * nWidth) + nX];

			// Is this a new component?, 255 == Object
			if (_TLabelIndex != 0) {
				if (_TFirstFlag[_TLabelIndex] == false) {
					m_recBlobs[_TLabelIndex - 1].x	= nX;
					m_recBlobs[_TLabelIndex - 1].y	= nY;
					m_recBlobs[_TLabelIndex - 1].width	= 0;
					m_recBlobs[_TLabelIndex - 1].height = 0;
				
					_TFirstFlag[_TLabelIndex] = true;
				}
				else {
					int _TLeft = m_recBlobs[_TLabelIndex - 1].x;
					int _TRight	= _TLeft + m_recBlobs[_TLabelIndex - 1].width;
					int _TTop	= m_recBlobs[_TLabelIndex - 1].y;
					int _TBottom = _TTop + m_recBlobs[_TLabelIndex - 1].height;

					if (_TLeft >= nX) _TLeft = nX;
					if (_TRight <= nX) _TRight = nX;
					if (_TTop >= nY) _TTop = nY;
					if (_TBottom <= nY) _TBottom = nY;

					m_recBlobs[_TLabelIndex - 1].x = _TLeft;
					m_recBlobs[_TLabelIndex - 1].y	= _TTop;
					m_recBlobs[_TLabelIndex - 1].width = _TRight - _TLeft;
					m_recBlobs[_TLabelIndex - 1].height = _TBottom - _TTop;
				}
			}
				
		}
	}
	
}

// Blob Labeling을 실제 행하는 function
// 2000년 정보처리학회에 실린 논문 참조
int BlobLabeling::__NRFIndNeighbor(unsigned char *DataBuf, int nWidth, int nHeight, int nPosX, int nPosY, int *StartX, int *StartY, int *EndX, int *EndY) {
	CvPoint _TCurrentPoint;
	
	_TCurrentPoint.x = nPosX;
	_TCurrentPoint.y = nPosY;

	m_vPoint[(_TCurrentPoint.y * nWidth) +  _TCurrentPoint.x].bVisitedFlag = true;
	m_vPoint[(_TCurrentPoint.y * nWidth) +  _TCurrentPoint.x].ptReturnPoint.x = nPosX;
	m_vPoint[(_TCurrentPoint.y * nWidth) +  _TCurrentPoint.x].ptReturnPoint.y = nPosY;
			
	while (1) {
		// -X 방향
		if ((_TCurrentPoint.x != 0) && (DataBuf[(_TCurrentPoint.y * nWidth) + _TCurrentPoint.x - 1] == 255)) {
			if (m_vPoint[(_TCurrentPoint.y * nWidth) +  _TCurrentPoint.x - 1].bVisitedFlag == false) {
				DataBuf[(_TCurrentPoint.y * nWidth) + _TCurrentPoint.x - 1] = DataBuf[(_TCurrentPoint.y * nWidth) + _TCurrentPoint.x];	// If so, mark it
				m_vPoint[(_TCurrentPoint.y * nWidth) + _TCurrentPoint.x - 1].bVisitedFlag = true;
				m_vPoint[(_TCurrentPoint.y * nWidth) + _TCurrentPoint.x - 1].ptReturnPoint	= _TCurrentPoint;
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
			if (m_vPoint[(_TCurrentPoint.y * nWidth) +  _TCurrentPoint.x + 1].bVisitedFlag == false) {

				DataBuf[(_TCurrentPoint.y * nWidth) + _TCurrentPoint.x + 1]	= DataBuf[(_TCurrentPoint.y * nWidth) + _TCurrentPoint.x];	// If so, mark it
				m_vPoint[(_TCurrentPoint.y * nWidth) + _TCurrentPoint.x + 1].bVisitedFlag	= true;
				m_vPoint[(_TCurrentPoint.y * nWidth) + _TCurrentPoint.x + 1].ptReturnPoint	= _TCurrentPoint;
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
			if (m_vPoint[((_TCurrentPoint.y - 1) * nWidth) +  _TCurrentPoint.x].bVisitedFlag == false) {
				DataBuf[((_TCurrentPoint.y - 1) * nWidth) + _TCurrentPoint.x] = DataBuf[(_TCurrentPoint.y * nWidth) + _TCurrentPoint.x];	// If so, mark it
				m_vPoint[((_TCurrentPoint.y - 1) * nWidth) + _TCurrentPoint.x].bVisitedFlag = true;
				m_vPoint[((_TCurrentPoint.y - 1) * nWidth) + _TCurrentPoint.x].ptReturnPoint = _TCurrentPoint;
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
			if (m_vPoint[((_TCurrentPoint.y + 1) * nWidth) +  _TCurrentPoint.x].bVisitedFlag == false) {
				DataBuf[((_TCurrentPoint.y + 1) * nWidth) + _TCurrentPoint.x] = DataBuf[(_TCurrentPoint.y * nWidth) + _TCurrentPoint.x];	// If so, mark it
				m_vPoint[((_TCurrentPoint.y + 1) * nWidth) +  _TCurrentPoint.x].bVisitedFlag	= true;
				m_vPoint[((_TCurrentPoint.y + 1) * nWidth) +  _TCurrentPoint.x].ptReturnPoint = _TCurrentPoint;
				_TCurrentPoint.y++;

				if (_TCurrentPoint.y >= nHeight - 1)
					_TCurrentPoint.y = nHeight - 1;

				if (*EndY <= _TCurrentPoint.y)
					*EndY = _TCurrentPoint.y;

				continue;
			}
		}

		if ((_TCurrentPoint.x == m_vPoint[(_TCurrentPoint.y * nWidth) + _TCurrentPoint.x].ptReturnPoint.x) 
			&& (_TCurrentPoint.y == m_vPoint[(_TCurrentPoint.y * nWidth) + _TCurrentPoint.x].ptReturnPoint.y)) {
			break;
		}
		else {
			_TCurrentPoint = m_vPoint[(_TCurrentPoint.y * nWidth) + _TCurrentPoint.x].ptReturnPoint;
		}
	}

	return 0;
}

// 영역중 실제 blob의 칼라를 가진 영역의 크기를 획득
int BlobLabeling::__Area(unsigned char *DataBuf, int StartX, int StartY, int EndX, int EndY, int nWidth, int nLevel) {
	int _TArea = 0;

	for (register int nY = StartY; nY < EndY; nY++)
		for (register int nX = StartX; nX < EndX; nX++) {

			if (DataBuf[(nY * nWidth) + nX] == nLevel)
				++_TArea;
		}

	return _TArea;
}

void BlobLabeling::DrawLabel(IplImage *img, CvScalar RGB) {
	//printf("n_blobs : %d\n", m_nBlobs);
	for (register int i = 0; i < m_nBlobs; i++) {
		//cvDrawCircle(img, cvPoint(m_recBlobs[i].x, m_recBlobs[i].y), 10, RGB);
		cvDrawRect(img, cvPoint(m_recBlobs[i].x, m_recBlobs[i].y),
			cvPoint(m_recBlobs[i].x + m_recBlobs[i].width, m_recBlobs[i].y + m_recBlobs[i].height), RGB);
	}
}

void BlobLabeling::GetSideBlob(IplImage *img, std::vector<int> *piece_idx, IplImage *other) {
	int _TIndex = 0;

	piece_idx->clear();
	cvZero(other);

	for (register int i = 0; i < m_nBlobs; i++) {
		CvRect temp = m_recBlobs[i];
		temp.width++;
		temp.height++;

		if (temp.x == 1) {
			_TIndex = i;
			continue;
		}
		else if (temp.y == 1) {
			_TIndex = i;
			continue;
		}
		else if (temp.x + temp.width >= roi_width - 1) {
			_TIndex = i;
			continue;
		}
		else if (temp.y + temp.height >= roi_height - 1) {
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
			cvSetImageROI(img, temp);
			cvSetImageROI(other, temp);
			cvCopy(img, other);
			cvZero(img);
			cvResetImageROI(img);
			cvResetImageROI(other);

			piece_idx->push_back(i);
		}
	}

	for (register int i = 0; i < img->width; i++)
		for (register int j = 0; j < img->height; j++){
			if (m_recBlobs[_TIndex].x <= i && m_recBlobs[_TIndex].x + m_recBlobs[_TIndex].width >= i && m_recBlobs[_TIndex].y <= j && m_recBlobs[_TIndex].y + m_recBlobs[_TIndex].height >= j)
				continue;
			else {
				img->imageData[i + (j * img->widthStep)];
			}
		}
}