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
		board[i][1] = B_Pawn, board[i][6] = W_Pawn;

	board[0][0] = board[7][0] = B_Rook;
	board[0][7] = board[7][7] = W_Rook;
	board[1][0] = board[6][0] = B_Knight;
	board[1][7] = board[6][7] = W_Knight;
	board[2][0] = board[5][2] = B_Bishop;
	board[2][7] = board[5][7] = W_Bishop;
	board[3][0] = B_Queen;
	board[3][7] = W_Queen;
	board[4][0] = B_King;
	board[4][7] = W_King;
}

chess_game::~chess_game() {
}

void chess_game::Chess_process(CvPoint input1, CvPoint input2) {
	if (board[input1.x][input1.y] == 0 && board[input2.x][input2.y] != 0) {
		board[input1.x][input1.y] = board[input2.x][input2.y];
		board[input2.x][input2.y] = Ground;
		if (turn) turn = false;
		else turn = true;
	}
	else if (board[input2.x][input2.y] == 0 && board[input1.x][input1.y] != 0) {
		board[input2.x][input2.y] = board[input1.x][input1.y];
		board[input1.x][input1.y] = Ground;
		if (turn) turn = false;
		else turn = true;
	}
	else {
		if (turn) {
			if (board[input1.x][input1.y] >= W_King && board[input1.x][input1.y] <= W_Pawn) {
				board[input2.x][input2.y] = board[input1.x][input1.y];
				board[input1.x][input1.y] = 0;
			}
			else {
				board[input1.x][input1.y] = board[input2.x][input2.y];
				board[input2.x][input2.y] = 0;
			}
			turn = false;
		}
		else {
			if (board[input1.x][input1.y] >= B_King && board[input1.x][input1.y] <= B_Pawn) {
				board[input2.x][input2.y] = board[input1.x][input1.y];
				board[input1.x][input1.y] = 0;
			}
			else {
				board[input1.x][input1.y] = board[input2.x][input2.y];
				board[input2.x][input2.y] = 0;
			}
			turn = true;
		}
	}
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