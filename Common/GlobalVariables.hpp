﻿//////////////////////////////////////////////////////////////////////////////////////////////
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

#ifndef _GlobalVariables_hpp_
#define _GlobalVariables_hpp_

#include <list>
#include <string>

using namespace std;
#pragma region struct_CommandString
// Define UCICommandString
typedef struct _CommandString {
	// lists
	list<string> *StringList;
	list<char *> *CharArrayList;

	// iterators
	list<string>::iterator StringListIter;
	list<char *>::iterator CharArrayListIter;

	bool IsIsInitializeToStringList;
	bool IsIsInitializeToCharArrayList;

	bool IsInitialize;
	int EndPoint;
	//int SeekPoint;

	/*
	_CommandString() {
		StringList = new list<string>();
		CharArrayList = new list<char *>();
		//IsIsInitializeToStringList = true;
		//IsIsInitializeToCharArrayList = true;
		EndPoint = 0;
		IsInitialize = false;
	}
	*/
	~_CommandString() {
		if (IsInitialize == true)
			Release();
	}

	_CommandString(list<string> *_UCIStringList) {
		IsIsInitializeToStringList = false;
		IsIsInitializeToCharArrayList = false;
		IsInitialize = false;
		UCIStringInitialize(_UCIStringList);
	}

	_CommandString(list<char *> *_UCICharArrayList) {
		IsIsInitializeToStringList = false;
		IsIsInitializeToCharArrayList = false;
		IsInitialize = false;
		UCICharArrayInitialize(_UCICharArrayList);
	}

	void UCIStringInitialize(list<string> *_UCIStringList) {
		StringList = new list<string>();

		StringList = _UCIStringList;
		StringListIter = StringList->begin();
		EndPoint = _UCIStringList->size();
		IsInitialize = true;
		IsIsInitializeToStringList = true;
	}

	void UCICharArrayInitialize(list<char *> *_UCICharArrayList) {
		CharArrayList = new list<char *>();

		CharArrayList = _UCICharArrayList;
		CharArrayListIter = CharArrayList->begin();
		EndPoint = CharArrayList->size();
		IsInitialize = true;
		IsIsInitializeToCharArrayList = true;
	}

	void Release() {
		if (StringList != NULL && IsIsInitializeToStringList == true)
			delete StringList;

		if (CharArrayList != NULL && IsIsInitializeToCharArrayList == true)
			delete CharArrayList;

		IsInitialize = false;
	}

	bool PrevStringIter() {
		if (IsInitialize == true && IsFirstStringIter() != true) {
			StringListIter--;
			return true;
		}
		return false;
	}

	bool NextStringIter() {
		if (IsInitialize == true && IsLastStringIter() != true) {
			StringListIter++;
			return true;
		}
		return false;
	}

	bool IsFirstStringIter() {
		list<string>::iterator _TIter = StringListIter;
		_TIter--;
		if (IsInitialize == true && StringList->begin() != _TIter) {
			return false;
		}
		return true;
	}

	bool IsLastStringIter() {
		list<string>::iterator _TIter = StringListIter;
		_TIter++;
		if (IsInitialize == true && StringList->end() != _TIter) {
			return false;
		}
		return true;
	}

	bool PrevCharArrayIter() {
		if (IsInitialize == true && IsFirstCharArrayIter() != true) {
			CharArrayListIter--;
			return true;
		}
		return false;
	}

	bool NextCharArrayIter() {
		if (IsInitialize == true && IsLastCharArrayIter() != true) {
			CharArrayListIter++;
			return true;
		}
		return false;
	}

	bool IsFirstCharArrayIter() {
		list<char *>::iterator _TIter = CharArrayListIter;
		_TIter--;
		if (IsInitialize == true && CharArrayList->begin() != _TIter) {
			return false;
		}
		return true;
	}

	bool IsLastCharArrayIter() {
		list<char *>::iterator _TIter = CharArrayListIter;
		_TIter++;
		if (IsInitialize == true && CharArrayList->end() != _TIter) {
			return false;
		}
		return true;
	}
} CommandString;
#pragma endregion struct_CommandString
#endif