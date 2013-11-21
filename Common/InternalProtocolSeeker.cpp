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

// 여기 있는 Protocol의 모든 명세는 InternalProtocol.hpp에 있음.
int InternalProtocolSeeker::InternalProtocolString_Seeker(const char *Str) {
	int _SeekedNumber = 0;

	STRING_SWITCH_BEGIN(string((char *)Str)) {
		CASE(STR_I_PTYPE)
			_SeekedNumber = VALUE_I_PTYPE;
			break;

		CASE(STR_I_INFO)
			_SeekedNumber = VALUE_I_INFO;
			break;
		CASE(STR_I_INFO_TYPE)
			_SeekedNumber = VALUE_I_INFO_TYPE;
			break;

		CASE(STR_I_SERVERKILL)
			_SeekedNumber = VALUE_I_SERVERKILL;
			break;
		CASE(STR_I_SERVERISALIVE)
			_SeekedNumber = VALUE_I_SERVERISALIVE;
			break;
		CASE(STR_I_STOP)
			_SeekedNumber = VALUE_I_STOP;
			break;
		CASE(STR_I_START)
			_SeekedNumber = VALUE_I_START;
			break;
		CASE(STR_I_ISRESTOREPOSSIBLE)
			_SeekedNumber = VALUE_I_ISRESTOREPOSSIBLE;
			break;
		CASE(STR_I_ISSERVERREADY)
			_SeekedNumber = VALUE_I_ISSERVERREADY;
			break;
		CASE(STR_I_AIMODE)
			_SeekedNumber = VALUE_I_AIMODE;
			break;

		CASE(STR_I_INFO_TYPE_CLIENT)
			_SeekedNumber = VALUE_I_INFO_TYPE_CLIENT;
			break;

		CASE(STR_I_INFO_GO)
			_SeekedNumber = VALUE_I_INFO_GO;
			break;
		CASE(STR_I_INFO_BLACKTIME)
			_SeekedNumber = VALUE_I_INFO_BLACKTIME;
			break;
		CASE(STR_I_INFO_WHITETIME)
			_SeekedNumber = VALUE_I_INFO_WHITETIME;
			break;
		CASE(STR_I_INFO_TURN)
			_SeekedNumber = VALUE_I_INFO_TURN;
			break;

		CASE(STR_I_INFO_POSITION)
			_SeekedNumber = VALUE_I_INFO_POSITION;
			break;
		CASE(STR_I_INFO_ENEMYMOVE)
			_SeekedNumber = VALUE_I_INFO_ENEMYMOVE;
			break;
		CASE(STR_I_INFO_MOVENULL)
			_SeekedNumber = VALUE_I_INFO_MOVENULL;
			break;
		CASE(STR_I_INFO_WHITE)
			_SeekedNumber = VALUE_I_INFO_WHITE;
			break;
		CASE(STR_I_INFO_BLACK)
			_SeekedNumber = VALUE_I_INFO_BLACK;
			break;

		CASE(STR_I_ALIVE)
			_SeekedNumber = VALUE_I_ALIVE;
			break;
		CASE(STR_I_BUSY)
			_SeekedNumber = VALUE_I_BUSY;
			break;
		CASE(STR_I_MOVE)
			_SeekedNumber = VALUE_I_MOVE;
			break;
		CASE(STR_I_RESTOREOK)
			_SeekedNumber = VALUE_I_RESTOREOK;
			break;
		CASE(STR_I_RESTORENOT)
			_SeekedNumber = VALUE_I_RESTORENOT;
			break;
		CASE(STR_I_SERVERISREADY)
			_SeekedNumber = VALUE_I_SERVERISREADY;
			break;

		CASE(STR_I_TICTOK)
			_SeekedNumber = VALUE_I_TICTOK;
			break;
		CASE(STR_I_TICTOKISON)
			_SeekedNumber = VALUE_I_TICTOKISON;
			break;
		CASE(STR_I_TICTOKISOFF)
			_SeekedNumber = VALUE_I_TICTOKISOFF;
			break;

		CASE(STR_I_DISP)
			_SeekedNumber = VALUE_I_DISP;
			break;

		CASE(STR_I_DISP_MINX)
			_SeekedNumber = VALUE_I_DISP_MINX;
			break;
		CASE(STR_I_DISP_MINY)
			_SeekedNumber = VALUE_I_DISP_MINY;
			break;
		CASE(STR_I_DISP_MAXX)
			_SeekedNumber = VALUE_I_DISP_MAXX;
			break;
		CASE(STR_I_DISP_MAXY)
			_SeekedNumber = VALUE_I_DISP_MAXY;
			break;

		CASE(STR_I_OMOVE)
			_SeekedNumber = VALUE_I_OMOVE;
			break;

		CASE(STR_I_OMOVE_BLACK)
			_SeekedNumber = VALUE_I_OMOVE_BLACK;
			break;
		CASE(STR_I_OMOVE_WHITE)
			_SeekedNumber = VALUE_I_OMOVE_WHITE;
			break;

		CASE(STR_I_OUPDATECHESSBOARD)
			_SeekedNumber = VALUE_I_OUPDATECHESSBOARD;
			break;

		CASE(STR_I_STATUSNOW)
			_SeekedNumber = VALUE_I_STATUSNOW;
			break;
		CASE(STR_I_TICTOKON)
			_SeekedNumber = VALUE_I_TICTOKON;
			break;
		CASE(STR_I_TICTOKOFF)
			_SeekedNumber = VALUE_I_TICTOKOFF;
			break;

		CASE(STR_I_INFO_TYPE_OBSERVER)
			_SeekedNumber = VALUE_I_INFO_TYPE_OBSERVER;
			break;
		
		// Here is No Command string.
		DEFAULT()
			_SeekedNumber = -1;
	}
	STRING_SWITCH_END()

	return _SeekedNumber;
}
