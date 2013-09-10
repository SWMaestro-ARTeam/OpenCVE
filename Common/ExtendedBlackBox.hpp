#ifndef _ExtendedBlackBox_hpp_
#define _ExtendedBlackBox_hpp_

#include <hash_map>

using namespace std;
/*
 * 확장 구문이 더 많아 진다면 Black Box로 명명하고 따로 뺄 것.
 * - GabrielKim
 */

/*
 * ! Notice :
 * 본 STRING_SWITCH_BEGIN 구문에 대해서.
 * 이 구문을 동작시키기 위해서는 반드시 Program Database mode, 즉, Zi Option을 주고 돌려야 함.
 *
 * ! Usage :
 * STR_SWITCH_BEGIN(sz)
 * {
 *	 CASE("Name")
 *	　 ...
 *		 break;
 *	 CASE("Age")
 *　 　...
 * 　　break;
 * }
 * STR_SWITCH_END()
 */

#define STRING_SWITCH_BEGIN(Str) \
{ \
	static stdext::hash_map<string, int> _SHashMap; \
	static bool _SBInit = false; \
	bool _BLoop = true; \
	while (_BLoop) \
	{ \
		int _NStr = -1; \
		if(_SBInit) { _NStr=_SHashMap[Str]; _BLoop = false; } \
		switch(_NStr) \
		{ \
			case -1: {

#define CASE(_StrToken) } case __LINE__: if(!_SBInit) _SHashMap[_StrToken] = __LINE__; else {
#define DEFAULT() } case 0: default: if(_SBInit) {

#define STRING_SWITCH_END() \
			} \
		} \
		if(!_SBInit) _SBInit=true; \
	} \
}

#endif