#ifndef _UCIParser_hpp_
#define _UCIParser_hpp_

#include <cstdio>
#include <cstdlib>

#include <string.h>
#include <stdlib.h>

#include "UCICommandParser.hpp"

class UCIParser {
private:
	// Variables
	char *Command_Str;

	// Functions
	void init_CommandStr();

	void Get_Command_Str();
	void Clear_Str();

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

	bool isServerOrClient;
	// Functions
	void initializing();
	void Parsing_Engine_Start();
};

#endif