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

#ifndef _UCICommandParser_hpp_
#define _UCICommandParser_hpp_

#include <stdio.h>
#include <string.h>

#include "Common.hpp"
#include "ExtendedBlackBox.hpp"
// 표준에 정의 된 대로 그대로 적용.

// define of UCI command string(GUI to Engine).
#define STR_UCI "uci"
#define STR_DEBUG "debug "
#define STR_ISREADY "isready"
#define STR_SETOPTION "setoption "
#define STR_UCINEWGAME "ucinewgame"
#define STR_REGISTER "register"
#define STR_POSITION "position "
#define STR_GO "go "
#define STR_STOP "stop"
#define STR_PONDERHIT "ponderhit"
#define STR_QUIT "quit"

// define of UCI command value(GUI to Engine).
#define VALUE_UCI 1
#define VALUE_DEBUG 2
#define VALUE_ISREADY 3
#define VALUE_SETOPTION 4
#define VALUE_UCINEWGAME 5
#define VALUE_REGISTER 6
#define VALUE_POSITION 7
#define VALUE_GO 8
#define VALUE_STOP 9
#define VALUE_PONDERHIT 10
#define VALUE_QUIT 11

#define GUI_TO_ENGINE_COMMAND_HEAD_QTY 11

static char *_GUIToEngineString_Head[GUI_TO_ENGINE_COMMAND_HEAD_QTY] =
	{STR_UCI, STR_DEBUG, STR_ISREADY, STR_SETOPTION, STR_UCINEWGAME,
	STR_REGISTER, STR_POSITION, STR_GO, STR_STOP, STR_PONDERHIT,
	STR_QUIT};

#define STR_GO_BINC "binc"
#define STR_GO_BTIME "btime"
#define STR_GO_DEPTH "depth"
#define STR_GO_INFINITE "infinite"
#define STR_GO_MATE "mate"
#define STR_GO_MOVESTOGO "movestogo"
#define STR_GO_MOVETIME "movetime"
#define STR_GO_NODES "nodes"
#define STR_GO_PONDER "ponder"
#define STR_GO_SEARCHMOVES "searchmoves"
#define STR_GO_WINC "winc"
#define STR_GO_WTIME "wtime"

#define VALUE_GO_BINC 1
#define VALUE_GO_BTIME 2
#define VALUE_GO_DEPTH 3
#define VALUE_GO_INFINITE 4
#define VALUE_GO_MATE 5
#define VALUE_GO_MOVESTOGO 6
#define VALUE_GO_MOVETIME 7
#define VALUE_GO_NODES 8
#define VALUE_GO_PONDER 9
#define VALUE_GO_SEARCHMOVES 10
#define VALUE_GO_WINC 11
#define VALUE_GO_WTIME 12

#define GUI_TO_ENGINE_COMMAND_GO_QTY 12

static char *_GUIToEngineString_Go[GUI_TO_ENGINE_COMMAND_GO_QTY] =
	{STR_GO_BINC, STR_GO_BTIME, STR_GO_DEPTH, STR_GO_INFINITE, STR_GO_MATE,
	STR_GO_MOVESTOGO, STR_GO_MOVETIME, STR_GO_NODES, STR_GO_PONDER, STR_GO_SEARCHMOVES,
	STR_GO_WINC, STR_GO_WTIME};

using namespace std;

class UCICommandParser {
private:

public:
	UCICommandParser();
	int UCIString_Seeker(const char *Str);
};

#endif