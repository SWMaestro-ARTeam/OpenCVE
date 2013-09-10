#include "UCICommandParser.hpp"

#define R_N(x) { return x + 1; }

UCICommandParser::UCICommandParser() {

}

int UCICommandParser::UCIString_Seeker(const char *Str){
	// 각 String을 전체 Command String과 비교해보고
	// 맨 앞의 string과 같다면 그 String의 최종 번호를 Return.
	char *_TStr = (char *)Str;

	strtok((char *)_TStr, "\n");

	for (int i=0; i<GUI_TO_ENGINE_COMMAND_QTY; i++) {
		char *_StrPtr = (char *)calloc(BUFFER_MAX_32767, sizeof(_StrPtr));
		
		memset(_StrPtr, NULL, sizeof(_StrPtr));
		
		//_StrPtr = strtok((char *)Str, " ");
		// 완전 매칭 되었을때 
		if (strcmp(_TStr, _GUIToEngineString[i]) == 0 || strstr(_TStr, _GUIToEngineString[i]) == _TStr) {
			R_N(i);
		}
		/*
		else if (strstr(Str, _GUIToEngineString[i]) == Str){

		}
		*/
		// 첫번째, 첫 문장에서 원하는 단어가 앞쪽에 매칭 되었는지?
		/*
		if (strstr(Str, _GUIToEngineString[i]) == Str) {
			_StrMatchFirst = true;
			if (((_StrPtr = strtok((char *)Str, " ")) != NULL) ||
				(_StrMatchFirst == true && strcmp(Str, _GUIToEngineString[i]) == 0)) {
				// 그런데 뒤에 공백이 존재하는지?
				//_StrMatchFirstAfterBlank = true;
				R_N(i);
			}
		}
		*/
		/*
		// 공백이 존재하게 되면 여기까지 오면 안되겠지?
		// 두번째, 그 string에서 단어가 완전히 일치하는가?
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