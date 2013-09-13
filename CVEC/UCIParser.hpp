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

#ifndef _UCIParser_hpp_
#define _UCIParser_hpp_

#include <cstdio>
#include <cstdlib>

#include <string.h>
#include <stdlib.h>

#include "UCICommandParser.hpp"
#include "Telepathy.hpp"

class UCIParser {
private:
	// Variables
	char *Command_Str;
	bool _IsSocketConnented;
	Telepathy::Client *_TClient;

	// Functions
	void init_CommandStr();
	bool Init_ClientSocket();

	void Put_Author();

	void Get_Command_Str();
	void Clear_Str();

	void Clear_ClientSocket();

	void SendToGUI(const char *Str, ...);

	void Command_UCI();
	void Command_Debug();
	void Command_Isready();
	void Command_Setoption();
	void Command_Ucinewgame();
	void Command_Register();
	void Command_Position();
	void Command_Go();
	void Command_Stop();
	void Command_Ponderhit();
	void Command_Quit();

	void Parsing_Command();

public:
	// Constructor
	UCIParser();
	~UCIParser();

	bool isServerOrClient;
	// Functions
	void initializing();
	void Parsing_Engine_Start();
};

void ClientReceivedCallback(char *Buffer);

#endif