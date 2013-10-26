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

//#include <EngineC.hpp>
//#include <EngineS.hpp>

#include <string.h>
#include <string>

#include "Celestials.h"
#include <QApplication>

int GoCVEC() {
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

int GoCVES(int argc, char *argv[]) {
	int _TApplicationReturnValue = 0;
	QApplication a(argc, argv);
	Celestials w;
	// 1. CVES Engine 생성.
	EngineS *_EngineS = new EngineS();
	w._EngineS = _EngineS;
	// CVES View 보여주기.
	w.show();

	// 2. Engine Enable.
	w._EngineS->EngineEnable = true;
	// 3. Engine Start.
	w._EngineS->EngineS_Start();
	_TApplicationReturnValue = a.exec();

	// 4. Delete pointer.
	delete _EngineS;
	return _TApplicationReturnValue;
}

int GoCVEO() {
	int _TApplicationReturnValue = 0;
	// 아직 미구현.

	return _TApplicationReturnValue;
}

// 통합 Engine 구현(CVEC, CVES, CVEO).
int main(int argc, char *argv[]) {
	int _TApplicationReturnValue = 0;

	STRING_SWITCH_BEGIN(string((char *)argv[1]))
	{
		CASE("/Server")
			_TApplicationReturnValue = GoCVES(argc, argv);
			break;
		CASE("/Client")
			_TApplicationReturnValue = GoCVEC();
			break;
		CASE("/Observer")
			_TApplicationReturnValue = GoCVEO();
			break;
		DEFAULT()
			fprintf(stdout, "Invaild Command.");
	}
	STRING_SWITCH_END()

	return _TApplicationReturnValue;
}
