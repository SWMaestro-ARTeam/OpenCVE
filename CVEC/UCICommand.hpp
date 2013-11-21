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

#ifndef _UCICommand_hpp_
#define _UCICommand_hpp_

// 표준에 정의 된 대로 그대로 적용.

// define of UCI command string(GUI to Engine).
#define STR_UCI "uci"
#define STR_DEBUG "debug"
#define STR_ISREADY "isready"
#define STR_SETOPTION "setoption"
#define STR_UCINEWGAME "ucinewgame"
#define STR_REGISTER "register"
#define STR_POSITION "position"
#define STR_GO "go"
#define STR_STOP "stop"
#define STR_PONDERHIT "ponderhit"
#define STR_QUIT "quit"

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

#define STR_POSITION_FEN "fen"
#define STR_POSITION_STARTPOS "startpos"
#define STR_POSITION_MOVES "moves"

#define STR_SETOPTION_NAME "name"
#define STR_SETOPTION_VALUE "value"

// define of UCI command value(GUI to Engine).
#define VALUE_UCI 1001
#define VALUE_DEBUG 1002
#define VALUE_ISREADY 1003
#define VALUE_SETOPTION 1004
#define VALUE_UCINEWGAME 1005
#define VALUE_REGISTER 1006
#define VALUE_POSITION 1007
#define VALUE_GO 1008
#define VALUE_STOP 1009
#define VALUE_PONDERHIT 1010
#define VALUE_QUIT 1011

#define VALUE_GO_BINC 1101
#define VALUE_GO_BTIME 1102
#define VALUE_GO_DEPTH 1103
#define VALUE_GO_INFINITE 1104
#define VALUE_GO_MATE 1105
#define VALUE_GO_MOVESTOGO 1106
#define VALUE_GO_MOVETIME 1107
#define VALUE_GO_NODES 1108
#define VALUE_GO_PONDER 1109
#define VALUE_GO_SEARCHMOVES 1110
#define VALUE_GO_WINC 1111
#define VALUE_GO_WTIME 1112

#define VALUE_POSITION_FEN 1201
#define VALUE_POSITION_STARTPOS 1202
#define VALUE_POSITION_MOVES 1203

#define VALUE_SETOPTION_NAME 1301
#define VALUE_SETOPTION_VALUE 1302

#define VALUE_ANYVALUES -1

// summation commnad string with "Head", "go", "position", "setoption"
#define GUI_TO_ENGINE_COMMAND_HEAD_QTY (11 + 12 + 3 + 2)

//#define GUI_TO_ENGINE_COMMAND_HEAD_QTY 11

static const char *_GUIToEngineString[GUI_TO_ENGINE_COMMAND_HEAD_QTY] =
{ 
	// It's Head String.
	STR_UCI, STR_DEBUG, STR_ISREADY, STR_SETOPTION, STR_UCINEWGAME,
	STR_REGISTER, STR_POSITION, STR_GO, STR_STOP, STR_PONDERHIT,
	STR_QUIT, 
	// It's string next with "go"
	STR_GO_BINC, STR_GO_BTIME, STR_GO_DEPTH, STR_GO_INFINITE,	STR_GO_MATE,
	STR_GO_MOVESTOGO, STR_GO_MOVETIME, STR_GO_NODES, STR_GO_PONDER,	STR_GO_SEARCHMOVES,
	STR_GO_WINC, STR_GO_WTIME,
	// It's string next with "position"
	STR_POSITION_FEN, STR_POSITION_STARTPOS, STR_POSITION_MOVES,
	// It's string next with "setoption"
	STR_SETOPTION_NAME, STR_SETOPTION_VALUE
};

/*
#define GUI_TO_ENGINE_COMMAND_GO_QTY 12

static char *_GUIToEngineString_Go[GUI_TO_ENGINE_COMMAND_GO_QTY] =
{STR_GO_BINC, STR_GO_BTIME, STR_GO_DEPTH, STR_GO_INFINITE, STR_GO_MATE,
STR_GO_MOVESTOGO, STR_GO_MOVETIME, STR_GO_NODES, STR_GO_PONDER, STR_GO_SEARCHMOVES,
STR_GO_WINC, STR_GO_WTIME};
*/

#endif