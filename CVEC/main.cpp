// main.cpp
// OpenCVE main ������.

// �⺻ Library
#include <cstdio>
#include <cstdlib>

// ���� ���
#include "Common.hpp"

// Modules
#include "UCIParser.hpp"
#include "ProcessConfirm.hpp"

int main(int argc, char* argv[]) {
	UCIParser *_Parser = new UCIParser();
	ProcessConfirm *_ProcessConfirm = new ProcessConfirm();
	
	bool _BisProcessDoing = false;
	bool _BisPartofServerOrClient = false; // false is Client, True is Server

	// 1. Process Ȯ��. && 2. Server/Client Initializing.
	_BisPartofServerOrClient = _ProcessConfirm->CheckAnotherEngineProcess(ENGINE_EXEC_FILENAME);

	// 2. TCP/IP Socket Start.
	/*
	(_BisPartofServerOrClient == true) ? 
		// true �� �� �ش� Process�� �ִٴ� ������ ����, �����ϴ� Process�� Server��� �̾߱��.
		// �׷��Ƿ� Client Mode�� �Ǿ�� �Ѵ�.

		// Client Mode

		:
		// flase �� �� �ش� Process�� ���ٴ� ������ ����.
		// �׷��Ƿ� Server Mode�� �Ǿ�� �Ѵ�.

		// Server Mode
		*/
		/*
	if (_BisPartofServerOrClient == true) {
		// true �� �� �ش� Process�� �ִٴ� ������ ����, �����ϴ� Process�� Server��� �̾߱��.
		// �׷��Ƿ� Client Mode�� �Ǿ�� �Ѵ�.

		// Client Mode

	}
	else {
		// flase �� �� �ش� Process�� ���ٴ� ������ ����.
		// �׷��Ƿ� Server Mode�� �Ǿ�� �Ѵ�.

		// Server Mode

	}
	// 
	*/
	// 2. Parser �ʱ�ȭ.
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
