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

#include "chess_game.hpp"

chess_game::chess_game() {
	turn = true;		// 백

	for (int i=0; i<8; i++)
		for (int j=0; j<8; j++)
			board[i][j] = Ground;

	for (int i=0; i<8; i++)
		board[i][6] = B_Pawn, board[i][1] = W_Pawn;

	board[0][0] = board[7][0] = W_Rook;
	board[0][7] = board[7][7] = B_Rook;
	board[1][7] = board[6][7] = B_Knight;
	board[1][0] = board[6][0] = W_Knight;
	board[2][7] = board[5][7] = B_Bishop;
	board[2][0] = board[5][0] = W_Bishop;
	board[3][7] = B_Queen;
	board[3][0] = W_Queen;
	board[4][7] = B_King;
	board[4][0] = W_King;
}

chess_game::~chess_game() {
}

void chess_game::Chess_process(CvPoint input1[], int MOVE_MODE) {
	CvPoint temp_move[4];

	switch (MOVE_MODE)
	{
	case CASTLING_MOVE:					// 캐슬링
		for(int i = 0; i < 4; i++)
			temp_move[i] = input1[i];

		break;
	case ENPASSANT_MOVE:				// 앙파상
		for(int i = 0; i < 3; i++)
			temp_move[i] = input1[i];

		int *value[3];
		value[0] = &board[temp_move[0].y][temp_move[0].x];
		value[1] = &board[temp_move[1].y][temp_move[1].x];
		value[2] = &board[temp_move[2].y][temp_move[2].x];

		int *white, *zero_pic, *black;
		for(int i = 0; i < 3; i++){
			if(*value[i] == 0)
				zero_pic = value[0];
			else if(*value[i] == W_Pawn)
				white = value[i];
			else if(*value[i] == B_Pawn)
				black = value[i];
		}

		if(turn == true){
			
		}else if(turn == false){

		}

		break;
	default :										// 기본 무브
		for(int i = 0; i < 2; i++)
			temp_move[i] = input1[i];

		int value1, value2;
		value1 = board[temp_move[0].y][temp_move[0].x];
		value2 = board[temp_move[1].y][temp_move[1].x];

		//체스 무브 진행
		if(turn == true){
			//백색 차례일때

			if(1 <= value1 && value1 <= 6){
				board[temp_move[1].y][temp_move[1].x] = 0;
				SWAP(board[temp_move[0].y][temp_move[0].x], board[temp_move[1].y][temp_move[1].x]);
			}else if(1 <= value2 && value2 <= 6){
				board[temp_move[0].y][temp_move[0].x] = 0;
				SWAP(board[temp_move[0].y][temp_move[0].x], board[temp_move[1].y][temp_move[1].x]);
			}
		}
		else if(turn == false){
			//검은색 차례일때

			if(7 <= value1 && value1 <= 12){
				board[temp_move[1].y][temp_move[1].x] = 0;
				SWAP(board[temp_move[0].y][temp_move[0].x], board[temp_move[1].y][temp_move[1].x]);
			}else if(7 <= value2 && value2 <= 12){
				board[temp_move[0].y][temp_move[0].x] = 0;
				SWAP(board[temp_move[0].y][temp_move[0].x], board[temp_move[1].y][temp_move[1].x]);
			}
		}

		break;
	}

	turn = !turn;
}

void chess_game::Show_chess_board() {
	for (int i=0; i<8; i++) {
		for (int j=0; j<8; j++) {
			printf("%3d", board[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}