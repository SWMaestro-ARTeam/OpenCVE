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

#ifndef _ChessGame_hpp_
#define _ChessGame_hpp_

#include "CVESDependent.hpp"

#include <stdio.h>
#include <queue>
#include <vector>

#define CASTLING_MOVE	4
#define ENPASSANT_MOVE 3
#define DEFAULT_MOVE 2
#define DEBUGUI_PATH "./Chess_UI/"

#define WHITE_TURN true
#define BLACK_TURN false

using namespace std;

typedef struct __MoveFormat {
	char *_Movement;
	bool _TurnFlag;
} MoveFormat;

typedef struct __BeforeMove {
	int Piece;
	CvPoint position;
} BeforeMove;

enum {			//체스말
	Ground, 
	W_King, W_Queen, W_Rook, W_Bishop, W_Knight, W_Pawn,
	B_King, B_Queen, B_Rook, B_Bishop, B_Knight, B_Pawn,
};

class ChessGame {

private:
	int _Board[8][8];
	bool _Turn;
	char _RecentMove[6]; //가장 최근 움직임을 저장
	CvPoint _Before, _After;
	CvPoint _BeforeMove_Pawn;
	BeforeMove _BeforeMove;
	MoveFormat _TempMove;
	IplImage *_ChessBoard_Image;
	IplImage *_ChessPiece;

	bool _W_PawnMove[8], _B_PawnMove[8];

	FILE *_Movement_Log;		//체스 이동경로 Log를 남길 파일포인터.
	std::queue<MoveFormat> _Chess_Movement;

	void Make_UCICoordinate(CvPoint before, CvPoint after);
	char Mapping_UCICharacter(int position); // 글자 좌표로 매핑

	// MODE 판독 함수 

	// 각 체스 룰을 다루는 함수
	void Moving_Castling(CvPoint Moving_Input[]);
	void Moving_Enpassant(CvPoint Moving_Input[]);
	void Moving_Default(CvPoint Moving_Input[]);

	// 기본 이동 체스룰

	bool Rule_DefaultMove(CvPoint before, CvPoint after);

public:
	ChessGame();
	~ChessGame();

	// chess board의 말 움직임 진행 함수.
	// MOVE_MODE : CASTLING_MOVE - 캐슬링 detect, ENPASSANT_MOVE - 앙파상 detect, other - 두 가지 말의 이동만을 체크함.
	bool Chess_process(CvPoint input1[], int MOVE_MODE);
	void Show_ChessBoard(); // cmd에 체스판을 그림.
	void Show_ChessImage(); // chess UI를 Draw
	string Get_RecentMove(); // list에 저장된 가장 최근 movement를 return; Dequeue와 같음.
	int Read_Mode();
};
#endif
