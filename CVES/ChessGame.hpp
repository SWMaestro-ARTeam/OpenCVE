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

typedef struct move_format{
	char movement[32];
	bool turn_flag;
} move_format;

typedef struct before_move{
	int piece;
	CvPoint position;
} before_move;

enum {			//체스말
	Ground, 
	W_King, W_Queen, W_Rook, W_Bishop, W_Knight, W_Pawn,
	B_King, B_Queen, B_Rook, B_Bishop, B_Knight, B_Pawn,
};

using namespace std;

class ChessGame {

private:
	int _Board[8][8];
	bool _Turn;
	char recent_move[6];		//가장 최근 움직임을 저장
	CvPoint _Before, _After;
	before_move _before_move;
	IplImage *chessboard_img;
	IplImage *chess_piece;

	FILE *_Movement_log;		//체스 이동경로 Log를 남길 파일포인터.
	std::queue<move_format> _chess_movement;

	void MakeUCI(CvPoint before, CvPoint after, move_format *dst);
	char char_mapping(int position);	// 글자 좌표로 매핑

	// MODE 판독 함수 

	// 각 체스 룰을 다루는 함수
	void castling_move(CvPoint move_input[]);
	void enpassant_move(CvPoint move_input[]);
	void default_move(CvPoint move_input[]);

public:
	ChessGame();
	~ChessGame();

	// chess board의 말 움직임 진행 함수.
	// MOVE_MODE : CASTLING_MOVE - 캐슬링 detect, ENPASSANT_MOVE - 앙파상 detect, other - 두 가지 말의 이동만을 체크함.
	void Chess_process(CvPoint input1[], int MOVE_MODE);
	void Show_chess_board();		// cmd에 체스판을 그림.
	void Show_chessImage();			// chess UI를 Draw
	void Get_RecentMove(char *str);     // list에 저장된 가장 최근 movement를 return; Dequeue와 같음.
	int Mode_read();
};
#endif
