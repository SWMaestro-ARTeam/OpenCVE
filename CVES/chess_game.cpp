#include "chess_game.hpp"
#include <stdio.h>

chess_game::chess_game()
{
	turn = true;		// น้

	for(int i=0;i<8;i++)
		for(int j=0;j<8;j++)
			board[i][j] = Ground;

	for(int i=0;i<8;i++)
		board[1][i] = B_Pawn, board[6][i] = W_Pawn;

	board[0][0] = board[0][7] = B_Rook;
	board[7][0] = board[7][7] = W_Rook;
	board[0][1] = board[0][6] = B_Knight;
	board[7][1] = board[7][6] = W_Knight;
	board[0][2] = board[0][5] = B_Bishop;
	board[7][2] = board[7][5] = W_Bishop;
	board[0][3] = B_Queen;
	board[7][3] = W_Queen;
	board[0][4] = B_King;
	board[7][4] = W_King;
}


chess_game::~chess_game()
{
}

void chess_game::Chess_process(CvPoint input1, CvPoint input2){
	if(board[input1.x][input1.y] == 0 && board[input2.x][input2.y] != 0){
		board[input1.x][input1.y] = board[input2.x][input2.y];
		board[input2.x][input2.y] = Ground;
		if(turn) turn = false;
		else turn = true;
	}
	else if(board[input2.x][input2.y] == 0 && board[input1.x][input1.y] != 0){
		board[input2.x][input2.y] = board[input1.x][input1.y];
		board[input1.x][input1.y] = Ground;
		if(turn) turn = false;
		else turn = true;
	}
	else{
		if(turn){
			if(board[input1.x][input1.y] >= W_King && board[input1.x][input1.y] <= W_Pawn){
				board[input2.x][input2.y] = board[input1.x][input1.y];
				board[input1.x][input1.y] = 0;
			}
			else{
				board[input1.x][input1.y] = board[input2.x][input2.y];
				board[input2.x][input2.y] = 0;
			}
			turn = false;
		}
		else{
			if(board[input1.x][input1.y] >= B_King && board[input1.x][input1.y] <= B_Pawn){
				board[input2.x][input2.y] = board[input1.x][input1.y];
				board[input1.x][input1.y] = 0;
			}
			else{
				board[input1.x][input1.y] = board[input2.x][input2.y];
				board[input2.x][input2.y] = 0;
			}
			turn = true;
		}
	}
}

void chess_game::Show_chess_board(){
	for(int i=0;i<8;i++){
		for(int j=0;j<8;j++){
			printf("%3d", board[i][j]);
		}
		printf("\n");
	}
}