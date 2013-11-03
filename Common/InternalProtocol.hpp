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

// CVEC -> CVES
#define STR_I_SERVERKILL "ServerKill"
#define STR_I_SERVERISALIVE "ServerIsAlive"
#define STR_I_IMFIRST "ImFirst"
#define STR_I_STOP "Stop"
#define STR_I_START "Start"
#define STR_I_ISRESTOREPOSSIBLE "IsRestorePossible"
#define STR_I_ISSERVERREADY "IsServerReady"
#define STR_I_INFO "Info"

#define STR_I_INFO_GO "Go"
#define STR_I_INFO_POSITION "Position"

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

// CVEC -> CVES
#define VALUE_I_SERVERKILL 1001
#define VALUE_I_SERVERISALIVE 1002
#define VALUE_I_IMFIRST 1003
#define VALUE_I_STOP 1004
#define VALUE_I_START 1005
#define VALUE_I_ISRESTOREPOSSIBLE 1006
#define VALUE_I_ISSERVERREADY 1007
#define VALUE_I_INFO 1008

#define VALUE_I_INFO_GO 1101
#define VALUE_I_INFO_POSITION 1102

// CVES -> CVEC
#define VALUE_I_ALIVE 2001
#define VALUE_I_BUSY 2002
#define VALUE_I_MOVE 2003
#define VALUE_I_RESTOREOK 2004
#define VALUE_I_RESTORENOT 2005
#define VALUE_I_SERVERISREADY 2006

// CVES -> Observer
#define VALUE_I_STATUSNOW 3001
#define VALUE_I_TICTOKON 3002
#define VALUE_I_TICTOKOFF 3003

// Observer -> CVES
#define VALUE_I_TICTOK 4001
#define VALUE_I_TICTOKISON 4002
#define VALUE_I_TICTOKISOFF 4003

#endif