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