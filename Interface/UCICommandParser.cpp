#include "UCICommandParser.hpp"

UCICommandParser::UCICommandParser() {

}

int UCICommandParser::UCIString_Seeker(const char *Str){
	// 각 String을 전체 Command String과 비교해보고
	// 맨 앞의 string과 같다면 그 String의 최종 번호를 Return.

	for (int i=0; i<GUI_TO_ENGINE_COMMAND_QTY; i++) {
		char *_StrPtr = (char *)calloc(BUFFER_MAX_32767, sizeof(_StrPtr));

		memset(_StrPtr, NULL, sizeof(_StrPtr));
		
		if (strstr(Str, _GUIToEngineString[i]) == Str) {
			if (strcmp(Str, _GUIToEngineString[i]) == 0)
			if ((_StrPtr = strtok((char *)Str, " ")) != NULL)
		}
		if (strstr(Str, _GUIToEngineString[i]) == Str) {
		//if (((ptr = strtok((char *)Str, " ")) != NULL) ? ((strcmp(ptr, _GUIToEngineString[i]))==0) : ((strcmp(Str, _GUIToEngineString[i]))==0)){
			return i + 1;
		}
	}
}