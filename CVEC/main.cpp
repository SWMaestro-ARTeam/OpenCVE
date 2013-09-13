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
	UCIParser *_Parser = new UCIParser();
	ProcessConfirm *_ProcessConfirm = new ProcessConfirm();
	
	bool _BisProcessDoing = false;
	bool _BisPartofServerOrClient = false; // false is Client, True is Server

	// 1. Process 확인. && 2. Server/Client Initializing.
	_BisPartofServerOrClient = _ProcessConfirm->CheckAnotherEngineProcess(ENGINE_EXEC_FILENAME);

	// 2. TCP/IP Socket Start.
	/*
	(_BisPartofServerOrClient == true) ? 
		// true 일 때 해당 Process가 있다는 것으로 간주, 현존하는 Process가 Server라는 이야기다.
		// 그러므로 Client Mode가 되어야 한다.

		// Client Mode

		:
		// flase 일 때 해당 Process가 없다는 것으로 간주.
		// 그러므로 Server Mode가 되어야 한다.

		// Server Mode
		*/
		/*
	if (_BisPartofServerOrClient == true) {
		// true 일 때 해당 Process가 있다는 것으로 간주, 현존하는 Process가 Server라는 이야기다.
		// 그러므로 Client Mode가 되어야 한다.

		// Client Mode

	}
	else {
		// flase 일 때 해당 Process가 없다는 것으로 간주.
		// 그러므로 Server Mode가 되어야 한다.

		// Server Mode

	}
	// 
	*/
	// 2. Parser 초기화.
	_Parser->initializing();
	
	// 3. Vision Engine Start.


	_Parser->Parsing_Engine_Start();

	// 5. delete pointer.
	delete _Parser;
	delete _ProcessConfirm;

	return 0;
}
