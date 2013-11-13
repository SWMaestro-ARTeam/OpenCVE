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

//#include "SystemDependency.hpp"

#include "AdapterS.hpp"

int AdapterS::Go_EngineS(int argc, char* argv[]) {
	int _TApplicationReturnValue = 0;
	EngineS *_EngineS;

	// 1. CVEC Engine 생성.
	_EngineS = new EngineS();

#if defined(USING_QT)
	QApplication a(argc, argv);
	CVES w;

	w._EngineS = _EngineS;
#endif

	// 2. Engine Enable.
	_EngineS->EngineEnable = true;
	// 3. Engine Start.
	_EngineS->EngineS_Start();

#ifdef USING_QT
	w.show();
	_TApplicationReturnValue = a.exec();
#else
	// Thread 처리 할 경우 Main Application이 Thread보다 먼저 죽어버리는 경우가 발생하므로,
	// 이를 방지하기 위해 Engine이 Enable일 때까지 계속 멈춰 있게 하여야 한다.
	while (_EngineS->EngineEnable) 
		Sleep(10);
#endif
	// 4. Delete pointer.
	delete _EngineS;

	return _TApplicationReturnValue;
}
