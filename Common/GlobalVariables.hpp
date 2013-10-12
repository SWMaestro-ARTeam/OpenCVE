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

#ifndef _GlobalVariables_hpp_
#define _GlobalVariables_hpp_

#include <list>
#include <string>

using namespace std;
#pragma region struct_CommandString
// Define UCICommandString
typedef struct CommandString {
	// lists
	list<string> *StringList;
	list<char *> *CharArrayList;

	// iterators
	list<string>::iterator StringListIter;
	list<char *>::iterator CharArrayListIter;

	bool IsInitialize;
	int EndPoint;
	//int SeekPoint;

	CommandString() {
		StringList = new list<string>();
		CharArrayList = new list<char *>();
		EndPoint = 0;
	}

	CommandString(list<string> *_UCIStringList) {
		if (IsInitialize == true) Release();
		UCIStringInitialize(_UCIStringList);
	}

	CommandString(list<char *> *_UCICharArrayList) {
		if (IsInitialize == true) Release();
		UCICharArrayInitialize(_UCICharArrayList);
	}

	void UCIStringInitialize(list<string> *_UCIStringList) {
		StringList = new list<string>();

		StringList = _UCIStringList;
		StringListIter = StringList->begin();
		EndPoint = _UCIStringList->size();
		IsInitialize = true;
	}

	void UCICharArrayInitialize(list<char *> *_UCICharArrayList) {
		CharArrayList = new list<char *>();

		CharArrayList = _UCICharArrayList;
		CharArrayListIter = CharArrayList->begin();
		EndPoint = CharArrayList->size();
		IsInitialize = true;
	}

	void Release() {
		if (StringList != NULL) delete StringList;
		if (CharArrayList != NULL) delete CharArrayList;

		IsInitialize = false;
	}

	bool PrevStringIter() {
		list<string>::iterator _TIter = StringListIter;
		_TIter--;
		if (IsInitialize == true && StringList->begin() != _TIter) {
			//if (IsInitialize == true && UCIStringList->begin() != UCIStringListIter) {
			StringListIter--;
			return true;
		}
		return false;
	}

	bool NextStringIter() {
		list<string>::iterator _TIter = StringListIter;
		_TIter++;
		if (IsInitialize == true && StringList->end() != _TIter) {
			//if (IsInitialize == true && UCIStringList->end() != UCIStringListIter) {
			StringListIter++;
			return true;
		}
		return false;
	}

	bool PrevCharArrayIter() {
		list<char *>::iterator _TIter = CharArrayListIter;
		_TIter--;
		if (IsInitialize == true && CharArrayList->begin() != _TIter) {
			//if (IsInitialize == true && UCICharArrayList->begin() != UCICharArrayListIter) {
			CharArrayListIter--;
			return true;
		}
		return false;
	}

	bool NextCharArrayIter() {
		list<char *>::iterator _TIter = CharArrayListIter;
		_TIter++;
		if (IsInitialize == true && CharArrayList->end() != _TIter) {
			//if (IsInitialize == true && UCICharArrayList->end() != UCICharArrayListIter) {
			CharArrayListIter++;
			return true;
		}
		return false;
	}
} CS;
#pragma endregion struct_CommandString

#endif