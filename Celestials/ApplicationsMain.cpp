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

#include "ApplicationsMain.h"

Celestials *G_Celestials;

ServerInitThread::ServerInitThread() {
}

void ServerInitThread::run() {
	//Celestials w;
	// 1. CVES Engine 생성.
	//EngineS *_EngineS = new EngineS();
	//G_Celestials._EngineS = _EngineS;
	// CVES View 보여주기.

	// 2. Engine Enable.
	G_Celestials->_EngineS->EngineEnable = true;
	// 3. Engine Start.
	G_Celestials->_EngineS->EngineS_Start();
	delete G_Celestials->_EngineS;
}

ApplicationsMain::ApplicationsMain() {
	G_Celestials = new Celestials();
}

int ApplicationsMain::GoCVEC() {
	int _TApplicationReturnValue = 0;
	// 1. CVEC Engine 생성.
	EngineC *_EngineC = new EngineC();
	// 2. Engine Enable.
	_EngineC->EngineEnable = true;
	// 3. Engine Start.
	_EngineC->EngineC_Start();

	// 4. Delete pointer.
	delete _EngineC;
	return _TApplicationReturnValue;
}

int ApplicationsMain::GoCVES(int argc, char *argv[]) {
	int _TApplicationReturnValue = 0;
	// 1. CVES Engine 생성.
	_EngineS = new EngineS();
	G_Celestials->_EngineS = _EngineS;
	G_Celestials->show();
	QApplication a(argc, argv);

	_ServerInitThread.start();

	// 2. Engine Enable.
	//_Celestials._EngineS->EngineEnable = true;
	// 3. Engine Start.
	//w._EngineS->EngineS_Start();
	_TApplicationReturnValue = a.exec();

	// 4. Delete pointer.
//	delete _EngineS;
	return _TApplicationReturnValue;
}

int ApplicationsMain::GoCVEO() {
	int _TApplicationReturnValue = 0;
	// 아직 미구현.

	return _TApplicationReturnValue;
}
