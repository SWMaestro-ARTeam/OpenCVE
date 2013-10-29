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

#include "CodeConverter.hpp"

// char -> wchar
wchar_t *CodeConverter::CharToWChar(const char *CharStr) {
	wchar_t* _TWStr = NULL;
	int _TWStrLength, _TCharLength;

	_TCharLength = strlen(CharStr);
	_TWStrLength = MultiByteToWideChar(CP_ACP, 0, CharStr, _TCharLength, NULL, 0);

	if(_TWStrLength > 0) {
		_TWStr = (wchar_t*)malloc(sizeof(wchar_t) * (_TWStrLength+1));
		MultiByteToWideChar(CP_ACP, 0, CharStr, _TCharLength, _TWStr, _TWStrLength);
	}

	if (_TWStr != NULL)
		_TWStr[_TWStrLength] = 0;

	return _TWStr;
}

// wchar -> char
char *CodeConverter::WCharToChar(const wchar_t* WcharStr) {
	char* _TStr = NULL;
	int _TWStrLength, _TCharLength;

	_TWStrLength = wcslen(WcharStr);
	_TCharLength = WideCharToMultiByte(CP_ACP, 0, WcharStr, _TWStrLength, NULL, 0, NULL, FALSE);

	if(_TCharLength > 0) {
		_TStr = (char*)malloc(sizeof(char) * (_TCharLength+1));
		WideCharToMultiByte(CP_ACP, 0, WcharStr, _TWStrLength, _TStr, _TCharLength, NULL, FALSE);
	}

	if (_TStr != NULL)
		_TStr[_TCharLength] = 0;

	return _TStr;
}