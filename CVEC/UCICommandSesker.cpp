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

#include "UCICommandSeeker.hpp"

int UCICommandSeeker::UCIString_Seeker(const char *Str){
	// 특별한 일이 없는 이상, 단일 문자열로 들어온다.
	// 따라서 String Switch ~ case 문으로 간다.
	// 여기에는 UCI와 관련된 문자열만 들어온다.
	// 다른 수, 좌표들은 -1로 분리되어 완전히 별개로 취급한다.
	int _SeekedNumber = 0;
	
	STRING_SWITCH_BEGIN(string((char *)Str)) {
		CASE(STR_UCI)
			_SeekedNumber = VALUE_UCI;
			break;
		CASE(STR_DEBUG)
			_SeekedNumber = VALUE_DEBUG;
			break;
		CASE(STR_ISREADY)
			_SeekedNumber = VALUE_ISREADY;
			break;
		CASE(STR_SETOPTION)
			_SeekedNumber = VALUE_SETOPTION;
			break;
		CASE(STR_UCINEWGAME)
			_SeekedNumber = VALUE_UCINEWGAME;
			break;
		CASE(STR_REGISTER)
			_SeekedNumber = VALUE_REGISTER;
			break;
		CASE(STR_POSITION)
			_SeekedNumber = VALUE_POSITION;
			break;
		CASE(STR_GO)
			_SeekedNumber = VALUE_GO;
			break;
		CASE(STR_STOP)
			_SeekedNumber = VALUE_STOP;
			break;
		CASE(STR_PONDERHIT)
			_SeekedNumber = VALUE_PONDERHIT;
			break;
		CASE(STR_QUIT)
			_SeekedNumber = VALUE_QUIT;
			break;

			// Hear It place to "go" command next
		CASE(STR_GO_BINC)
			_SeekedNumber = VALUE_GO_BINC;
			break;
		CASE(STR_GO_BTIME)
			_SeekedNumber = VALUE_GO_BTIME;
			break;
		CASE(STR_GO_DEPTH)
			_SeekedNumber = VALUE_GO_DEPTH;
			break;
		CASE(STR_GO_INFINITE)
			_SeekedNumber = VALUE_GO_INFINITE;
			break;
		CASE(STR_GO_MATE)
			_SeekedNumber = VALUE_GO_MATE;
			break;
		CASE(STR_GO_MOVESTOGO)
			_SeekedNumber = VALUE_GO_MOVESTOGO;
			break;
		CASE(STR_GO_MOVETIME)
			_SeekedNumber = VALUE_GO_MOVETIME;
			break;
		CASE(STR_GO_NODES)
			_SeekedNumber = VALUE_GO_NODES;
			break;
		CASE(STR_GO_PONDER)
			_SeekedNumber = VALUE_GO_PONDER;
			break;
		CASE(STR_GO_SEARCHMOVES)
			_SeekedNumber = VALUE_GO_SEARCHMOVES;
			break;
		CASE(STR_GO_WINC)
			_SeekedNumber = VALUE_GO_WINC;
			break;
		CASE(STR_GO_WTIME)
			_SeekedNumber = VALUE_GO_WTIME;
			break;

			// Hear It place to "position" command next
		CASE(STR_POSITION_FEN)
			_SeekedNumber = VALUE_POSITION_FEN;
			break;
		CASE(STR_POSITION_STARTPOS)
			_SeekedNumber = VALUE_POSITION_STARTPOS;
			break;
		CASE(STR_POSITION_MOVES)
			_SeekedNumber = VALUE_POSITION_MOVES;
			break;

			// Hear It place to "setoption" command next
		CASE(STR_SETOPTION_NAME)
			_SeekedNumber = VALUE_SETOPTION_NAME;
			break;
		CASE(STR_SETOPTION_VALUE)
			_SeekedNumber = VALUE_SETOPTION_VALUE;
			break;

			// Here is No Command string.
		DEFAULT()
			_SeekedNumber = -1;
	}
	STRING_SWITCH_END()

	return _SeekedNumber;
}