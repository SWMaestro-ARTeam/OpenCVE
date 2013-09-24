#include "UCICommandSeeker.hpp"

int UCICommandSeeker::UCIString_Seeker(const char *Str){
	// �� String�� ��ü Command String�� ���غ���
	// �� ���� string�� ���ٸ� �� String�� ���� ��ȣ�� Return.
	// Str�� ���� ����Ǹ� �ȵ�.

	// ���� ���ϱⰡ �ʹ� ��ġ ����.
	/*
	char *_StrPtr = (char *)calloc(BUFFER_MAX_32767, sizeof(_StrPtr));

	memset(_StrPtr, NULL, sizeof(_StrPtr));
	memcpy(_StrPtr, (char *)Str, sizeof((char *)Str));

	// ������ ���� ��� �̰��� ��ġ�Ѵ�.
	char *_TmpStr = strtok(_StrPtr, " ");
	if (_TmpStr != NULL) {
		// NULL�� �ƴ� ���(������ ���� ���).
		strcat(_TmpStr, " ");
		sprintf(_StrPtr, "%s ", _StrPtr);
	}
	else{
		// NULL �� ���(������ ���� ���).
		// �׳� ����.
		_TmpStr = (char *)_StrPtr;
	}
	*/
	char *_StrPtr = strtok((char *)Str, " ");

	if (_StrPtr != NULL) {
		// NULL�� �ƴ� ���(������ ���� ���).
		//strcat(_StrPtr, " ");
		sprintf(_StrPtr, "%s ", Str);
	}
	else {
		// NULL �� ���(������ ���� ���).
		// �׳� ����.
		_StrPtr = (char *)Str;
	}

	for (int i=0; i<GUI_TO_ENGINE_COMMAND_HEAD_QTY; i++) {
		// ���� ��Ī �Ǿ��� ��.
		// ó���� �� String�� ��Ī �� Ȯ���� ���� ������ ���� �տ��� ó��.
		if (strcmp(_StrPtr, _GUIToEngineString[i]) == 0) {
			R_N(i);
		}
		
		// ���ʸ� ��Ī �Ǿ��� ��.
		// �׷���, �̶��� �׻� ������ �����ϰų�, ���̰� �ٸ� ���� ������ ������ ���� Check�� �Ѵ�.
		if (strstr(_StrPtr, _GUIToEngineString[i]) == _StrPtr){
			// 1. ������ �����ϴ°�?(strtok�� ���� ��)
			
			// 2. ���̰� �Ϻ��� �G�°�?
			if (strlen(_StrPtr) == strlen(_GUIToEngineString[i])) break;
			R_N(i);
		}
	}
	free(_StrPtr);
}