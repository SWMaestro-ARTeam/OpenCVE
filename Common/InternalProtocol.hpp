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
#define STR_SREVERKILL "ServerKill"
#define STR_SERVERISALIVE "ServerIsAlive"
#define STR_IMFIRST "ImFirst"
#define STR_STOP "Stop"
#define STR_START "Start"
#define STR_ISRESTOREPOSSIBLE "IsRestorePossible"

// CVES -> CVEC
#define STR_ALIVE "Alive"
#define STR_BUSY "Busy"
#define STR_MOVE "Move"
#define STR_RESTOREOK "RestoreOK"
#define STR_RESTORENOT "RestoreNOT"

// CVES -> Observer
#define STR_STATUSNOW "StatusNow"
#define STR_TICTOKON "TicTokOn"
#define STR_TICTOKOFF "TicTokOff"

// Observer -> CVES
#define STR_TICTOK "TicTok"
#define STR_TICTOKISON "TicTokisOn"
#define STR_TICTOKISOFF "TicTokisOff"

// CVEC -> CVES
#define VALUE_SERVERKILL 1
#define VALUE_SERVERISALIVE 2
#define VALUE_IMFIRST 3
#define VALUE_STOP 4
#define VALUE_START 5
#define VALUE_ISRESTOREPOSSIBLE 6

// CVES -> CVEC
#define VALUE_ALIVE 7
#define VALUE_BUSY 8
#define VALUE_MOVE 9
#define VALUE_RESTOREOK 10
#define VALUE_RESTORENOT 11

// CVES -> Observer
#define VALUE_STATUSNOW 12
#define VALUE_TICTOKON 13
#define VALUE_TICTOKOFF 14

// Observer -> CVES
#define VALUE_TICTOK 15
#define VALUE_TICTOKISON 16
#define VALUE_TICTOKISOFF 17

#endif