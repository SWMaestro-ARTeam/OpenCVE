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

#ifndef _InternalProtocol_hpp_
#define _InternalProtocol_hpp_

/************************************************************************/
/* Type String.                                                         */
/************************************************************************/

// CVES -> Any(CVEC, CVEO)
#define STR_I_PTYPE "PType"

// Any(CVEC, CVEO) -> CVES
#define STR_I_INFO "Info"
#define STR_I_INFO_TYPE "Type"

// CVEC -> CVES
#define STR_I_SERVERKILL "ServerKill"
#define STR_I_SERVERISALIVE "ServerIsAlive"
#define STR_I_IMWHITE "ImWhite"
#define STR_I_STOP "Stop"
#define STR_I_START "Start"
#define STR_I_ISRESTOREPOSSIBLE "IsRestorePossible"
#define STR_I_ISSERVERREADY "IsServerReady"
// Info Command For 'CVEC' Only.
#define STR_I_INFO_TYPE_CLIENT "Client" //

#define STR_I_INFO_GO "Go" //
#define STR_I_INFO_BLACKTIME "BlackTime" //
#define STR_I_INFO_WHITETIME "WhiteTime" //
#define STR_I_INFO_TURN "Turn" //

#define STR_I_INFO_POSITION "Position" //
#define STR_I_INFO_ENEMYMOVE "EnemyMove" //
#define STR_I_INFO_MOVENULL "MoveNULL" //

// CVES -> CVEC
#define STR_I_ALIVE "Alive"
#define STR_I_BUSY "Busy"
#define STR_I_MOVE "Move"
#define STR_I_RESTOREOK "RestoreOK"
#define STR_I_RESTORENOT "RestoreNOT"
#define STR_I_SERVERISREADY "ServerisReady"

// CVES -> Observer
#define STR_I_STATUSNOW "StatusNow"
#define STR_I_TICTOKON "TicTokOn"
#define STR_I_TICTOKOFF "TicTokOff"

// Observer -> CVES
#define STR_I_TICTOK "TicTok"
#define STR_I_TICTOKISON "TicTokisOn"
#define STR_I_TICTOKISOFF "TicTokisOff"
// Info Command For 'CVEO' Only.
#define STR_I_INFO_TYPE_OBSERVER "Observer" //

/************************************************************************/
/* Type Number.                                                         */
/************************************************************************/

// CVES -> Any(CVEC, CVEO)
#define VALUE_I_PTYPE 10001

// Any(CVEC, CVEO) -> CVES
#define VALUE_I_INFO 11000
#define VALUE_I_INFO_TYPE 11100

// CVEC -> CVES
#define VALUE_I_SERVERKILL 1100
#define VALUE_I_SERVERISALIVE 1200
#define VALUE_I_IMWHITE 1300
#define VALUE_I_STOP 1400
#define VALUE_I_START 1500
#define VALUE_I_ISRESTOREPOSSIBLE 1600
#define VALUE_I_ISSERVERREADY 1700
// Info Command For 'CVEC' Only.
#define VALUE_I_INFO_TYPE_CLIENT 11110 //

#define VALUE_I_INFO_GO 11200 //
#define VALUE_I_INFO_BLACKTIME 11210 //
#define VALUE_I_INFO_WHITETIME 11220 //
#define VALUE_I_INFO_TURN 11230 //

#define VALUE_I_INFO_POSITION 11300 //
#define VALUE_I_INFO_ENEMYMOVE 11400 //
#define VALUE_I_INFO_MOVENULL 11500 //

// CVES -> CVEC
#define VALUE_I_ALIVE 2100
#define VALUE_I_BUSY 2200
#define VALUE_I_MOVE 2300
#define VALUE_I_RESTOREOK 2400
#define VALUE_I_RESTORENOT 2500
#define VALUE_I_SERVERISREADY 2600

// CVES -> Observer
#define VALUE_I_STATUSNOW 3100
#define VALUE_I_TICTOKON 3200
#define VALUE_I_TICTOKOFF 3300

// Observer -> CVES
#define VALUE_I_TICTOK 4100
#define VALUE_I_TICTOKISON 4200
#define VALUE_I_TICTOKISOFF 4300
// Info Command For 'CVEO' Only.
#define VALUE_I_INFO_TYPE_OBSERVER 11120

#define VALUE_I_ANYVALUES -1

#endif