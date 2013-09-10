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


	// 4 Parser Engine Start.
	printf("OpenCVE Connector Ver %s Start.\n",  ENGINE_EXEC_VER);
	printf("Engine Copyright by ARTeam.\n");

	_Parser->Parsing_Engine_Start();

	// 5. delete pointer.
	delete _Parser;
	delete _ProcessConfirm;

	return 0;
}
