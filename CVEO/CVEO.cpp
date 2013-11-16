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

#include "CVEO.hpp"
#include "ui_CVEO.h"

CVEO *G_CVEO;

CVEO::CVEO(QWidget *parent) :
QMainWindow(parent), ui(new Ui::CVEO) {
	ui->setupUi(this);

	G_CVEO = this;

	_ChessGridMinX = 0;
	_ChessGridMinY = 0;
	_ChessGridMaxX = 0;
	_ChessGridMaxY = 0;

	desktop = QApplication::desktop();

	if (desktop->screenCount() == 1) {
		// single monitor - use built in
		showFullScreen();
	}
	else {
		QRect rect = desktop->screenGeometry(1);
		move(rect.topLeft());
		setWindowState(Qt::WindowFullScreen);
	}
}

CVEO::~CVEO() {
	delete ui;
}

void CVEO::EngineODataReceivedCallback(char *Buffer) {
	char _TStrBuffer[BUFFER_MAX_32767];
	memset(_TStrBuffer, NULL, sizeof(_TStrBuffer));
	strcpy(_TStrBuffer, Buffer);

	// Callback 명령어 감지.
	StringTokenizer *_StringTokenizer = new StringTokenizer();

	_StringTokenizer->SetInputCharString((const char *)_TStrBuffer);
	_StringTokenizer->SetSingleToken(" ");
	if (_StringTokenizer->StringTokenGo() == false)
		return ;

	bool _IsMinX = false;
	bool _IsMinY = false;
	bool _IsMaxX = false;
	bool _IsMaxY = false;
	bool _IsBlack = false;
	bool _IsWhite = false;

	string _TString = string("");
	CommandString *_InternalProtocolCS = new CommandString(_StringTokenizer->GetTokenedCharListArrays());
	while (_InternalProtocolCS->NextCharArrayIter()) {
		int _NSeek_EngineOToCVEO = G_CVEO->_InternalProtocolSeeker.InternalProtocolString_Seeker((const char *)*_InternalProtocolCS->CharArrayListIter);

		switch (_NSeek_EngineOToCVEO) {
			case VALUE_I_DISP_MINX :
				_IsMinX = true;
				break;
			case VALUE_I_DISP_MINY :
				_IsMinY = true;
				break;
			case VALUE_I_DISP_MAXX :
				_IsMaxX = true;
				break;
			case VALUE_I_DISP_MAXY :
				_IsMaxY = true;
				break;

			case VALUE_I_OMOVE_BLACK :
				_IsBlack = true;
				break;
			case VALUE_I_OMOVE_WHITE :
				_IsWhite = true;
				break;

			default :
				if (_IsMinX) {
					_IsMinX = false;
					// MinX 값을 설정.
					_TString.append(string((const char *)*_InternalProtocolCS->CharArrayListIter));
					G_CVEO->_ChessGridMinX = atoi(G_CVEO->_StringTools.StringToConstCharPointer(_TString));
					_TString.clear();
				}
				else if (_IsMinY) {
					_IsMinY = false;
					// MinX 값을 설정.
					_TString.append(string((const char *)*_InternalProtocolCS->CharArrayListIter));
					G_CVEO->_ChessGridMinY = atoi(G_CVEO->_StringTools.StringToConstCharPointer(_TString));
					_TString.clear();
				}
				else if (_IsMaxX) {
					_IsMaxX = false;
					// MinX 값을 설정.
					_TString.append(string((const char *)*_InternalProtocolCS->CharArrayListIter));
					G_CVEO->_ChessGridMaxX = atoi(G_CVEO->_StringTools.StringToConstCharPointer(_TString));
					_TString.clear();
				}
				else if (_IsMinY) {
					_IsMinY = false;
					// MinX 값을 설정.
					_TString.append(string((const char *)*_InternalProtocolCS->CharArrayListIter));
					G_CVEO->_ChessGridMaxY = atoi(G_CVEO->_StringTools.StringToConstCharPointer(_TString));
					_TString.clear();
				}
				else if (_IsBlack) {
					_IsBlack = false;
					// Black의 Move가 갔음을 알린다.
					// 고로 Black이 움직인 것을 표시해야 한다.
				}
				else if (_IsWhite) {
					_IsWhite = false;
					// White의 Move가 갔음을 알린다.
					// 고로 White가 움직인 것을 표시해야 한다.
				}
				break;
		}
	}

	delete _InternalProtocolCS;
	delete _StringTokenizer;
}

