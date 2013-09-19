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

// main.cpp
// OpenCVE main 진입점.

// 기본 Library
#include <cstdio>
#include <cstdlib>

// 공통 상수
#include "Common.hpp"

// Modules
#include "UCIParser.hpp"
#include "ProcessConfirm.hpp"

int main(int argc, char* argv[]) {
	ProcessConfirm *_ProcessConfirm = new ProcessConfirm();
	UCIParser *_Parser;
	
	bool _BIsServerProcessActive = false;
	bool _BIsAnotherClientProcessActive = false;

	// 1. Process 확인. 
	_BIsServerProcessActive = _ProcessConfirm->CheckProcess(SERVER_ENGINE_EXEC_FILENAME);
	_BIsAnotherClientProcessActive = _ProcessConfirm->CheckProcess(CLIENT_ENGINE_EXEC_FILENAME);

	// 2. CVES Process가 없다면 Process 실행.
	if (_BIsServerProcessActive == false) {
		if (_BIsAnotherClientProcessActive == false)

		// CVES 실행.
	}

	// 3. CVEC Parser 실행.
	// 이때, Client Network도 같이 실행 된다.
	_Parser = new UCIParser();
	_Parser->Parsing_Engine_Start();

	// 5. Delete pointer.
	delete _Parser;
	delete _ProcessConfirm;

	return 0;
}
