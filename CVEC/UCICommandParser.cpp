#include "UCICommandParser.hpp"

#define R_N(x) { return x + 1; }

UCICommandParser::UCICommandParser() {

}

int UCICommandParser::UCIString_Seeker(const char *Str){
	// �� String�� ��ü Command String�� ���غ���
	// �� ���� string�� ���ٸ� �� String�� ���� ��ȣ�� Return.
	char *_TStr = (char *)Str;

	strtok((char *)_TStr, "\n");

	for (int i=0; i<GUI_TO_ENGINE_COMMAND_QTY; i++) {
		char *_StrPtr = (char *)calloc(BUFFER_MAX_32767, sizeof(_StrPtr));
		
		memset(_StrPtr, NULL, sizeof(_StrPtr));
		
		//_StrPtr = strtok((char *)Str, " ");
		// ���� ��Ī �Ǿ��� ��.
		// ó���� �� String�� ��Ī �� Ȯ���� ���� ������ ���� �տ��� ó��.
		if (strcmp(_TStr, _GUIToEngineString[i]) == 0) 
			R_N(i);
		
		// ���ʸ� ��Ī �Ǿ��� ��.
		if (strstr(_TStr, _GUIToEngineString[i]) == _TStr)
			R_N(i);

		// �� ������ �˻��غ��� ������ �������� �Ѿ.
		/*
		else if (strstr(Str, _GUIToEngineString[i]) == Str){

		}
		*/
		// ù��°, ù ���忡�� ���ϴ� �ܾ ���ʿ� ��Ī �Ǿ�����?
		/*
		if (strstr(Str, _GUIToEngineString[i]) == Str) {
			_StrMatchFirst = true;
			if (((_StrPtr = strtok((char *)Str, " ")) != NULL) ||
				(_StrMatchFirst == true && strcmp(Str, _GUIToEngineString[i]) == 0)) {
				// �׷��� �ڿ� ������ �����ϴ���?
				//_StrMatchFirstAfterBlank = true;
				R_N(i);
			}
		}
		*/
		/*
		// ������ �����ϰ� �Ǹ� ������� ���� �ȵǰ���?
		// �ι�°, �� string���� �ܾ ������ ��ġ�ϴ°�?
		if (_StrMatchFirst == true && strcmp(Str, _GUIToEngineString[i]) == 0) {
			R_N(i);
		}
		*/
		/*
		if (strstr(Str, _GUIToEngineString[i]) == Str) {
		//if (((ptr = strtok((char *)Str, " ")) != NULL) ? ((strcmp(ptr, _GUIToEngineString[i]))==0) : ((strcmp(Str, _GUIToEngineString[i]))==0)){
			return i + 1;
		}
		*/
	}
}