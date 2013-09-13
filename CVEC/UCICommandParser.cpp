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

#include "UCICommandParser.hpp"

#define R_N(x) { return x + 1; }

UCICommandParser::UCICommandParser() {

}

int UCICommandParser::UCIString_Seeker(const char *Str){
	// 각 String을 전체 Command String과 비교해보고
	// 맨 앞의 string과 같다면 그 String의 최종 번호를 Return.
	// Str은 절대 변경되면 안됨.

	// 순서 정하기가 너무 골치 아픔.
	/*
	char *_StrPtr = (char *)calloc(BUFFER_MAX_32767, sizeof(_StrPtr));

	memset(_StrPtr, NULL, sizeof(_StrPtr));
	memcpy(_StrPtr, (char *)Str, sizeof((char *)Str));

	// 공백이 있을 경우 이것을 대치한다.
	char *_TmpStr = strtok(_StrPtr, " ");
	if (_TmpStr != NULL) {
		// NULL이 아닌 경우(공백이 있을 경우).
		strcat(_TmpStr, " ");
		sprintf(_StrPtr, "%s ", _StrPtr);
	}
	else{
		// NULL 일 경우(공백이 없는 경우).
		// 그냥 대입.
		_TmpStr = (char *)_StrPtr;
	}
	*/
	char *_StrPtr = strtok((char *)Str, " ");

	if (_StrPtr != NULL) {
		// NULL이 아닌 경우(공백이 있을 경우).
		//strcat(_StrPtr, " ");
		sprintf(_StrPtr, "%s ", Str);
	}
	else{
		// NULL 일 경우(공백이 없는 경우).
		// 그냥 대입.
		_StrPtr = (char *)Str;
	}

	for (int i=0; i<GUI_TO_ENGINE_COMMAND_HEAD_QTY; i++) {
		// 완전 매칭 되었을 때.
		// 처음에 두 String이 매칭 될 확률이 적기 때문에 가장 앞에서 처리.
		if (strcmp(_StrPtr, _GUIToEngineString_Head[i]) == 0) {
			R_N(i);
		}
		
		// 앞쪽만 매칭 되었을 때.
		// 그런데, 이때는 항상 공백이 존재하거나, 길이가 다른 경우기 때문에 다음과 같은 Check를 한다.
		if (strstr(_StrPtr, _GUIToEngineString_Head[i]) == _StrPtr){
			// 1. 공백이 존재하는가?(strtok를 했을 때)
			
			// 2. 길이가 완벽히 밎는가?
			if (strlen(_StrPtr) == strlen(_GUIToEngineString_Head[i])) break;
			R_N(i);
		}
	}
	free(_StrPtr);
}