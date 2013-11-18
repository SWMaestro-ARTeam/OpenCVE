//////////////////////////////////////////////////////////////////////////////////////////////
//	The OpenCVE Project.
//
//	The MIT License (MIT)
//	Copyright ⓒ 2013 {Doohoon Kim, Sungpil Moon, Kyuhong Choi} at AR Team of SW Maestro 4th
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

#include "CVES.hpp"
#include "ui_CVES.h"

CVES *G_CVES;

CVES::CVES(QWidget *parent) :
QMainWindow(parent),
ui(new Ui::CVES) {
	ui->setupUi(this);
	G_CVES = this;

	_Timer = new QTimer(this);
	_ImageTransformForQT = new ImageTransformForQT();

	_FrameQueue = new queue<IplImage *>();

	connect(_Timer, SIGNAL(timeout()), this, SLOT(FrameUpdate()));
	_Timer->start(100);

}

CVES::~CVES() {
	delete _Timer;
	delete _ImageTransformForQT;
	delete _FrameQueue;

	delete ui;
}

void CVES::EngineSFrameCallback(IplImage *NowFrame) {
	// 생각보다 Qt의 QImage 처리가 매우 느리다.
	// 이미 예상은 했지만, Memory Leak이 날 정도로 매우 심각하므로,
	// 이것을 빨리 해결해야한다.

	if (G_CVES->_FrameQueue->size() < 10) {
		G_CVES->_FrameQueue->push(NowFrame);
	}
	else {
		IplImage *_TImage = G_CVES->_FrameQueue->front();
		cvReleaseImage(&_TImage);
		G_CVES->_FrameQueue->pop();
		cvReleaseImage(&NowFrame);
	}

	//if (G_CVES->_EngineS->IsAllInitialize() == true) {
		//G_CVES->_FrameImageProtectQMutex.lock();
		//IplImage *_TFrame = NowFrame;
		/*
		uchar *_TData;
		//QImage *_TFrameImage = G_CVES->_ImageTransformForQT.IplImageToQImage(NowFrame);
		QImage *_TFrameImage = G_CVES->_ImageTransformForQT.IplImageToQImage(NowFrame, &_TData);
		//QImage _TFrameImage((uchar *)NowFrame->imageData, NowFrame->width, NowFrame->height, NowFrame->widthStep, QImage::Format_);
		G_CVES->ui->ImageViewLabel->setPixmap(QPixmap::fromImage(*_TFrameImage));

		delete _TFrameImage;
		*/
		//delete _TData;
		//cvReleaseImage(&_TFrame);
		//G_CVES->_FrameImageProtectQMutex.unlock();
	//}

}

void CVES::FrameUpdate() {
	if (_FrameQueue->empty() != true) {
		_FrameImageProtectQMutex.lock();
		IplImage *_TFrameIplImage = _FrameQueue->front();
		_FrameQueue->pop();

		uchar *_TData;
		QImage *_TFrameQImage = G_CVES->_ImageTransformForQT->IplImageToQImage(_TFrameIplImage, &_TData);
		//QImage _TFrameQImage((uchar *)_TFrameIplImage->imageData, _TFrameIplImage->width, _TFrameIplImage->height, _TFrameIplImage->widthStep, QImage::Format_RGB888);
		//G_CVES->ui->ImageViewLabel->setPixmap(QPixmap::fromImage(_TFrameQImage));
		G_CVES->ui->ImageViewLabel->setPixmap(QPixmap::fromImage(*_TFrameQImage).scaled(G_CVES->ui->ImageViewLabel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

		//delete _TFrameQImage;
		//if (_TFrameIplImage != NULL && _TFrameIplImage != 0x00)
			cvReleaseImage(&_TFrameIplImage);

		_FrameImageProtectQMutex.unlock();
	}
	/*
	if (_EngineS->IsAllInitialize() == true) {
		IplImage *_TFrame = _EngineS->Get_FrameImage();
		uchar *_TData;
		QImage *_TFrameImage = _ImageTransformForQT.IplImageToQImage(_TFrame, &_TData);

		ui->ImageViewLabel->setPixmap(QPixmap::fromImage(*_TFrameImage));

		cvReleaseImage(&_TFrame);
	}
	*/
}

//void CVES::show() {
//	_EngineS->TEngineSFrameCallback = EngineSFrameCallback;
//}

void CVES::on_actionExit_triggered() {
	_EngineS->EngineS_Destroy();
	this->close();
}
