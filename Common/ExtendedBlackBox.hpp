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

#ifndef _ExtendedBlackBox_hpp_
#define _ExtendedBlackBox_hpp_

#include <string>
#include <map>

using namespace std;
/*
 * 확장 구문이 더 많아 진다면 Black Box로 명명하고 따로 뺄 것.
 * - GabrielKim
 */

/*
 * ! Notice :
 * 본 STRING_SWITCH_BEGIN 구문에 대해서.
 * 이 구문을 동작시키기 위해서는 반드시 Program Database mode, 즉, Zi Option을 주고 돌려야 함.
 * 반드시, 쓰기전 <string>을 include 하고 쓸 것.
 * Release 에서 잘 작동.
 *
 * ! Usage :
 * STRING_SWITCH_BEGIN(sz)
 * {
 *	 CASE("Name")
 *	　 ...
 *		 break;
 *	 CASE("Age")
 *　 　...
 * 　　break;
 * }
 * STRING_SWITCH_BEGIN()
 */

#define STRING_SWITCH_BEGIN(Str) \
{ \
	static std::map<string, int> _SMap; \
	static bool _SBInit = false; \
	bool _BLoop = true; \
	while (_BLoop) \
	{ \
		int _NStr = -1; \
		if(_SBInit) { _NStr=_SMap[Str]; _BLoop = false; } \
		switch(_NStr) \
		{ \
			case -1: {

#define CASE(_StrToken) } case __LINE__: if(!_SBInit) _SMap[_StrToken] = __LINE__; else {
#define DEFAULT() } case 0: default: if(_SBInit) {

#define STRING_SWITCH_END() \
			} \
		} \
		if(!_SBInit) _SBInit=true; \
	} \
}

// Iter name using "_TVal"
#define for_IterToEnd(_TTemplate, _TType, _TTypeObject) \
	for(_TTemplate<_TType>::iterator _TVal = _TTypeObject.begin(); \
		_TVal != _TTypeObject.end(); _TVal++)

#define for_IterToBegin(_TTemplate, _TType, _TTypeObject) \
	for(_TTemplate<_TType>::iterator _TVal = _TTypeObject.end(); \
		_TVal != _TTypeObject.begin(); _TVal--)
#endif