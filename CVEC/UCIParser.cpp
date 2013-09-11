#include "Common.hpp"

// 기본 Header
#include "UCIParser.hpp"

UCIParser::UCIParser() {
	initializing();
}

void UCIParser::init_CommandStr() {
	// Command는 BUFFER_MAX값에 의해서 결정
	Command_Str = (char *)calloc(BUFFER_MAX_32767, sizeof(char));
	Clear_Str();
}

void UCIParser::initializing() {
	// string Buffer Initialize
	init_CommandStr();
}

void UCIParser::Get_Command_Str() {
	if (fgets(Command_Str, BUFFER_MAX_32767, stdin) == NULL) {
		if (feof(stdin)) {
			return ;
		}
	}
}

void UCIParser::Clear_Str() {
	memset(Command_Str, NULL, sizeof(Command_Str));
}

void UCIParser::Command_UCI() {
	printf("1\n");
}

void UCIParser::Command_Debug() {
	printf("2\n");
}

void UCIParser::Command_Isready() {
	printf("3\n");
}

void UCIParser::Command_Setoption() {
	printf("4\n");
}

void UCIParser::Command_Ucinewgame() {
	printf("5\n");
}

void UCIParser::Command_Register() {
	printf("6\n");
}

void UCIParser::Command_Position() {
	printf("7\n");
}

void UCIParser::Command_Go() {
	printf("8\n");
}

void UCIParser::Command_Stop() {
	printf("9\n");
}

void UCIParser::Command_Ponderhit() {
	printf("10\n");
}
void UCIParser::Command_Quit() {
	printf("11\n");
}

void UCIParser::Parsing_Command() {
	UCICommandParser _UCICommandParser;

	// Get UCI String.
	Get_Command_Str();
	int _NSeek_GUIToEngine = _UCICommandParser.UCIString_Seeker(Command_Str);

	switch (_NSeek_GUIToEngine) {
		default:

		case VALUE_UCI :
			Command_UCI();
			break;
		case VALUE_DEBUG :
			Command_Debug();
			break;
		case VALUE_ISREADY :
			Command_Isready();
			break;
		case VALUE_SETOPTION :
			Command_Setoption();
			break;
		case VALUE_UCINEWGAME :
			Command_Ucinewgame();
			break;
		case VALUE_REGISTER :
			Command_Register();
			break;
		case VALUE_POSITION :
			Command_Position();
			break;
		case VALUE_GO :
			Command_Go();
			break;
		case VALUE_STOP :
			Command_Stop();
			break;
		case VALUE_PONDERHIT :
			Command_Ponderhit();
			break;
		case VALUE_QUIT :
			Command_Quit();
			break;
	}
	// Clear UCI String.
	Clear_Str();
}

void UCIParser::Parsing_Engine_Start() {
	while (1) Parsing_Command();
}

