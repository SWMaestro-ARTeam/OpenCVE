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

#define GUI_TO_ENGINE_COMMAND_QTY 11

using namespace std;

static char *_GUIToEngineString[GUI_TO_ENGINE_COMMAND_QTY] =
	{STR_UCI, STR_DEBUG, STR_ISREADY, STR_SETOPTION, STR_UCINEWGAME,
	STR_REGISTER, STR_POSITION, STR_GO, STR_STOP, STR_PONDERHIT,
	STR_QUIT};

class UCICommandParser {
private:

public:
	UCICommandParser();
	int UCIString_Seeker(const char *Str);
};

#endif