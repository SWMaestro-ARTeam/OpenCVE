#pragma once
#include <cv.h>

class chess_game
{
	enum{
		Ground, 
		W_King, W_Queen, W_Rook, W_Bishop, W_Knight, W_Pawn,
		B_King, B_Queen, B_Rook, B_Bishop, B_Knight, B_Pawn,
	};
private:
	int board[8][8];
	bool turn;
	CvPoint before, after;
public:
	chess_game();
	~chess_game();

	void Chess_process(CvPoint input1, CvPoint input2);
	void Show_chess_board();
};

