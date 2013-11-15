﻿//////////////////////////////////////////////////////////////////////////////////////////////
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

#ifndef _CVEO_hpp_
#define _CVEO_hpp_

#include "EngineO.hpp"

//#if defined(USING_QT)
#include <QMainWindow>
#include <QApplication>
#include <QDesktopWidget>

namespace Ui {
	class CVEO;
}

class CVEO : public QMainWindow {
	Q_OBJECT

private:
	Ui::CVEO *ui;

	QDesktopWidget *desktop;

	InternalProtocolSeeker _InternalProtocolSeeker;
	StringTools _StringTools;

	int _ChessGridMinX;
	int _ChessGridMinY;
	int _ChessGridMaxX;
	int _ChessGridMaxY;

public:
	explicit CVEO(QWidget *parent = 0);
	~CVEO();

	EngineO *_EngineO;
	static void EngineODataReceivedCallback(char *Buffer);
};
//#endif
#endif // _CVEO_hpp_
