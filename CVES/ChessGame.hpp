﻿//////////////////////////////////////////////////////////////////////////////////////////////
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

#include <stdio.h>
#include <cv.h>
#include <highgui.h>

#define CASTLING_MOVE				4
#define ENPASSANT_MOVE			3
#define DEBUGUI_PATH "./Chess_DebugUI/"

#define SWAP(x, y) { int t; t = x; x = y; y = t; }

class ChessGame {
	enum {
		Ground, 
		W_King, W_Queen, W_Rook, W_Bishop, W_Knight, W_Pawn,
		B_King, B_Queen, B_Rook, B_Bishop, B_Knight, B_Pawn,
	};
private:
	int _Board[8][8];
	bool _Turn;
	CvPoint _Before, _After;
	
	IplImage *chessboard_img;
	IplImage *chess_piece;
public:
	ChessGame();
	~ChessGame();

	// chess board의 말 움직임 진행 함수.
	// MOVE_MODE : CASTLING_MOVE - 캐슬링 detect, ENPASSANT_MOVE - 앙파상 detect, other - 두 가지 말의 이동만을 체크함.
	void Chess_process(CvPoint input1[], int MOVE_MODE);
	void Show_chess_board();
	void Show_chessImage();
};
#endif