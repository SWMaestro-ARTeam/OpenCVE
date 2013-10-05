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

#include "InternalProtocolSeeker.hpp"

int InternalProtocolSeeker::InternalProtocolString_Seeker(const char *Str) {
	int _SeekedNumber = 0;

	STRING_SWITCH_BEGIN(string((char *)Str)) {
		CASE(STR_SREVERKILL)
			_SeekedNumber = VALUE_SERVERKILL;
			break;
		CASE(STR_SERVERISALIVE)
			_SeekedNumber = VALUE_SERVERISALIVE;
			break;
		CASE(STR_IMFIRST)
			_SeekedNumber = VALUE_IMFIRST;
			break;
		CASE(STR_STOP)
			_SeekedNumber = VALUE_STOP;
			break;
		CASE(STR_START)
			_SeekedNumber = VALUE_START;
			break;
		CASE(STR_ISRESTOREPOSSIBLE)
			_SeekedNumber = VALUE_ISRESTOREPOSSIBLE;
			break;

		CASE(STR_ALIVE)
			_SeekedNumber = VALUE_ALIVE;
		break;
		CASE(STR_BUSY)
			_SeekedNumber = VALUE_BUSY;
		break;
		CASE(STR_MOVE)
			_SeekedNumber = VALUE_MOVE;
		break;
		CASE(STR_RESTOREOK)
			_SeekedNumber = VALUE_RESTOREOK;
		break;
		CASE(STR_RESTORENOT)
			_SeekedNumber = VALUE_RESTORENOT;
		break;

		CASE(STR_STATUSNOW)
			_SeekedNumber = VALUE_STATUSNOW;
		break;
		CASE(STR_TICTOKON)
			_SeekedNumber = VALUE_TICTOKON;
		break;
		CASE(STR_TICTOKOFF)
			_SeekedNumber = VALUE_TICTOKOFF;

		break;
		CASE(STR_TICTOK)
			_SeekedNumber = VALUE_TICTOK;
		break;
		CASE(STR_TICTOKISON)
			_SeekedNumber = VALUE_TICTOKISON;
		break;
		CASE(STR_TICTOKISOFF)
			_SeekedNumber = VALUE_TICTOKISOFF;
		break;
		
		// Here is No Command string.
		DEFAULT()
			_SeekedNumber = -1;
	}
	STRING_SWITCH_END()

	return _SeekedNumber;
}
