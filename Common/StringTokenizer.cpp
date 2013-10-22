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

#include "StringTokenizer.hpp"

StringTokenizer::StringTokenizer() {
	StringTokenizerInitialize();
}

StringTokenizer::~StringTokenizer() {
	StringTokenizerDeinitialize();
}

// 초기에 초기화 할 것에 대한 모든 것을 담는다.
void StringTokenizer::StringTokenizerInitialize() {
	TokenClear();
	_StringList = new list<string>();

}

// 마지막에 없에야 할 모든것을 담는다.
void StringTokenizer::StringTokenizerDeinitialize() {
	delete _StringList;
}

void StringTokenizer::StringListClear() {
	// String List Clear.
	_StringList->clear();
}


void StringTokenizer::TokenClear() {
	// Token List Clear.
	_Token = "";
}

// String Input Functions.
void StringTokenizer::SetInputCharString(const char* Str) {
	_InternalInputString.append(Str);
}

void StringTokenizer::SetInputString(string Str) {
	_InternalInputString = Str;
}

// String Token의 종류.
void StringTokenizer::SetSingleToken(const char *SingleToken) {
	string _TempTokenString(SingleToken);
	_Token = _TempTokenString;
}

// Implement me.
/*
void StringTokenizer::SetMultiToken(const char *, ...) {

}
*/

bool StringTokenizer::StringTokenGo() {
	string _TempString = _InternalInputString;
	int _TTokenPointer = 0;
	list<string>::iterator _StringIter;

	// if, return Key or Empty that return false.
	if (_TempString.c_str() == "\r" || _TempString.c_str() == "" || _TempString.empty())
		return false;

	// Go Tokening.
	while ((_TTokenPointer = _TempString.find_first_of(_Token)) != _TempString.npos) {
		if (_TTokenPointer > 0)
			_StringList->push_back(_TempString.substr(0, _TTokenPointer));
		_TempString = _TempString.substr(_TTokenPointer + 1);
	}
	
	// 마지막 String 넣기.
	if (_TempString.length() > 0)
		_StringList->push_back(_TempString.substr(0, _TTokenPointer));
}

list<string> *StringTokenizer::GetTokenedStringList() {
	return _StringList;
}

list<char *> *StringTokenizer::GetTokenedCharListArrays() {
	list<char *> *_TempTokenedCharList = new list<char *>();
	list<string>::iterator _TStringIter;

	for (_TStringIter = _StringList->begin();
		_TStringIter != _StringList->end(); _TStringIter++) {
			char *_TCharArray = (char *)(_TStringIter->c_str());

			_TempTokenedCharList->push_back(_TCharArray);
	}

	return _TempTokenedCharList;
}

// 1. String List 초기화.
// 2. String 입력.
// 3. String Token 단일 인자 받기.
// 4. String 분리 시작.
// Ext-1. String Token 첫번째 인자 결과 Pointer 넘기기.
// Ext-2. String Token 마지막 인자 결과 Pointer 넘기기.
// Ext-3. char to String(All string).
// Ext-4. string List to Char *