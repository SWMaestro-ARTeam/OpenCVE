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

#include "Common.hpp"
#include "StringTools.hpp"

/*
template<typename T>
string StringTools::ToString(const T& Any) {
	return dynamic_cast<std::ostringstream &>((std::ostringstream() << std::dec << Any)).str();
}
*/
const char *StringTools::StringToConstCharPointer(string Str) {
	//const char *_TConstStr = new const char[BUFFER_MAX_32767];
	//memset((char *)_TConstStr, NULL, sizeof(_TConstStr));
	//_TConstStr = Str.c_str();
	//return _TConstStr;
	const char *_TConstStr = Str.c_str();
	char *_TStr = new char[BUFFER_MAX_32767];
	memset(_TStr, NULL, sizeof(_TStr));
	strcpy(_TStr, _TConstStr);
	return ConstCharToChar(_TStr);
}

char *StringTools::ConstCharToChar(const char *Str) {
	char *_TStr = const_cast<char *>(Str);
	return _TStr;
}

char *StringTools::StringToChar(string Str) {
	return ConstCharToChar(StringToConstCharPointer(Str));
}