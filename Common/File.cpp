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

#include "File.hpp"

// 현재 실행되는 자신의 Process의 경로를 얻는다.
char *File::GetCurrentPath() {
	char *_TStrFullPath = GetCurrentFullPath();
	char *_TStrProcess = GetCurrentProcessName();
	//char *_TStrBuff = new char[MAX_PATH];
	char _TStrBuff[MAX_PATH];

	memset(_TStrBuff, NULL, sizeof(_TStrBuff));
	strncpy(_TStrBuff, _TStrFullPath, strlen(_TStrFullPath) - strlen(_TStrProcess));
	// Null character into the end to buff.
	_TStrBuff[strlen(_TStrFullPath) - strlen(_TStrProcess)] = '\0';

	return _TStrBuff;
}

// 현재 실행되는 자신의 Process의 경로와 자신의 이름까지 얻어온다.
char *File::GetCurrentFullPath() {
	//char *_TStr = new char[MAX_PATH];
	//char _TStr[MAX_PATH];
#if UNICODE
#if WINDOWS_SYS
	// for windows unicode.
	LPWSTR _TStr = new WCHAR[MAX_PATH];
#endif
#else
	char *_TStr = new char[MAX_PATH];
#endif
	memset(_TStr, NULL, sizeof(_TStr));
#if WINDOWS_SYS
	GetModuleFileName(NULL,
#if	UNICODE
		// for windows unicode.
		(LPWSTR)
#else
		(LPSTR)
#endif
		_TStr, MAX_PATH);
#elif POSIX_SYS
	// POSIX 표준.
	getcwd(_TStr, _MAX_PATH);
#endif

#if UNICODE
	// for windows unicode.
	CodeConverter _TCodeConverter;
	return _TCodeConverter.WCharToChar(_TStr);
#else
	return (char *)_TStr;
#endif

}

// 현재 실행되는 자신의 Process 이름을 얻어온다.
char *File::GetCurrentProcessName() {
	char *_TStr = GetCurrentFullPath();
	char _TStrBuff[MAX_PATH];

	memset(_TStrBuff, NULL, sizeof(_TStrBuff));
	// 가장 마지막에 있는 '\' 뒤에는 반드시 파일이 있기 때문이다.
	// 고로 맨 앞은 현재 파일의 Directory Path.
	strcpy(_TStrBuff, strrchr(_TStr, '\\') + 1);
	_TStrBuff[strlen(_TStrBuff)] = '\0';
	return _TStrBuff;
}

// 현재 그 경로에 해당되는 파일이 있는지를 Check 한다.
bool File::CheckFileExist(char *FileName) {
	if (FileName == NULL)
		return false;

	char *_TStr = GetCurrentPath();
	char _TStrBuff[MAX_PATH];
	//size_t _TLengthFileNameStr = strlen(FileName) + 1;
	//size_t _TLengthPathStr = strlen(_TStr) + 1;
	strcpy(_TStrBuff, _TStr);
	//strcat(_TStr, FileName);
	strcat(_TStrBuff, FileName);
	_TStrBuff[strlen(_TStrBuff)] = '\0';
	//strncat(_TStr, FileName, _TLengthFileNameStr + 1); 

#if WINDOWS_SYS
	CodeConverter _TCodeConverter;
#if	UNICODE
	// for windows unicode.
	LPWSTR _TWCHARStr = _TCodeConverter.CharToWChar((const char *)_TStrBuff);
	//LPWSTR _TWCHARStr = _TCodeConverter.CharToWChar((const char *)_TStr);
#endif
	DWORD _TDWORD = ::GetFileAttributes(
#if	UNICODE
		// for windows unicode.
		(LPCWSTR)
#else
		(LPCSTR)
#endif

#if	UNICODE
		// for windows unicode.
		_TWCHARStr
#else
		_TStr
#endif
		);

	if (_TDWORD == 0xFFFFFFFFL)
		return false;
#elif POSIX_SYS
	DIR *_TDirPointer = NULL;

	if ((_TDirPointer = opendir(_TStr)) == NULL)
		return false;

	closedir(_TDirPointer);
#endif
	return true;
}