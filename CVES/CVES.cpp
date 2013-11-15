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
	/*
	_Timer = new QTimer(this);
	connect(_Timer, SIGNAL(timeout()), this, SLOT(FrameUpdate()));
	_Timer->start(33);
	*/

}

CVES::~CVES() {
	delete ui;
}

void CVES::EngineSFrameCallback(IplImage *NowFrame) {
	if (G_CVES->_EngineS->IsAllInitialize() == true) {
		//G_CVES->_FrameImageProtectQMutex.lock();
		//IplImage *_TFrame = NowFrame;
		//uchar *_TData;
		QImage *_TFrameImage = G_CVES->_ImageTransformForQT.IplImageToQImage(NowFrame);
		//QImage *_TFrameImage = G_CVES->_ImageTransformForQT.IplImageToQImage(NowFrame, &_TData);
		//QImage _TFrameImage((uchar *)NowFrame->imageData, NowFrame->width, NowFrame->height, NowFrame->widthStep, QImage::Format_);
		G_CVES->ui->ImageViewLabel->setPixmap(QPixmap::fromImage(*_TFrameImage));

		delete _TFrameImage;
		//delete _TData;
		//cvReleaseImage(&_TFrame);
		//G_CVES->_FrameImageProtectQMutex.unlock();
	}
}
/*
void CVES::FrameUpdate() {
	if (_EngineS->IsAllInitialize() == true) {
		IplImage *_TFrame = _EngineS->Get_FrameImage();
		uchar *_TData;
		QImage *_TFrameImage = _ImageTransformForQT.IplImageToQImage(_TFrame, &_TData);

		ui->ImageViewLabel->setPixmap(QPixmap::fromImage(*_TFrameImage));

		cvReleaseImage(&_TFrame);
	}
}
*/

//void CVES::show() {
//	_EngineS->TEngineSFrameCallback = EngineSFrameCallback;
//}

void CVES::on_actionExit_triggered() {
	_EngineS->EngineS_Destroy();
	this->close();
}
