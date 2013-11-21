//////////////////////////////////////////////////////////////////////////////////////////////
//	The OpenCVE Project.
//
//	The MIT License (MIT)
//	Copyright © 2013 {Doohoon Kim, Sungpil Moon, Kyuhong Choi} at AR Team of SW Maestro 4th
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

#ifndef _StringTokenizer_hpp_
#define _StringTokenizer_hpp_

#include <stdio.h>
#include <string.h>

#include <string>
#include <list>

using namespace std;

class StringTokenizer {
private:
	list<string> *_StringList;
	string _Token;

	string _InternalInputString;
public:
	StringTokenizer();
	~StringTokenizer();
	
	// Initializers.
	void StringTokenizerInitialize();
	void StringTokenizerDeinitialize();

	void StringListClear();

	void TokenClear();

	// String Input Functions.
	void SetInputCharString(const char* Str);
	void SetInputString(string Str);

	// String Token의 종류.
	void SetSingleToken(const char *SingleToken);
	// Implement me.
	//void SetMultiToken(const char *, ...);

	bool StringTokenGo();
	list<string> *GetTokenedStringList();
	list<char *> *GetTokenedCharListArrays();
};

#endif