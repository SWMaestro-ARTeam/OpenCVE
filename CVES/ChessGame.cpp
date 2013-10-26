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

#include "ChessGame.hpp"

ChessGame::ChessGame() {
	_Turn = true; // 백

	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			_Board[i][j] = Ground;

//<<<<<<< HEAD:CVES/ChessGame.cpp
//	for (int i = 0; i < 8; i++)
//		board[1][i] = B_Pawn, board[6][i] = W_Pawn;
//
//	board[0][0] = board[0][7] = B_Rook;
//	board[7][0] = board[7][7] = W_Rook;
//	board[0][1] = board[0][6] = B_Knight;
//	board[7][1] = board[7][6] = W_Knight;
//	board[0][2] = board[0][5] = B_Bishop;
//	board[7][2] = board[7][5] = W_Bishop;
//	board[0][3] = B_Queen;
//	board[7][3] = W_Queen;
//	board[0][4] = B_King;
//	board[7][4] = W_King;
//=======
	for (int i = 0; i < 8; i++)
		_Board[i][6] = B_Pawn, _Board[i][1] = W_Pawn;

	_Board[0][0] = _Board[7][0] = W_Rook;
	_Board[0][7] = _Board[7][7] = B_Rook;
	_Board[1][7] = _Board[6][7] = B_Knight;
	_Board[1][0] = _Board[6][0] = W_Knight;
	_Board[2][7] = _Board[5][7] = B_Bishop;
	_Board[2][0] = _Board[5][0] = W_Bishop;
	_Board[3][7] = B_Queen;
	_Board[3][0] = W_Queen;
	_Board[4][7] = B_King;
	_Board[4][0] = W_King;
//>>>>>>> CVES_HandRecognition:CVES/chess_game.cpp
}

ChessGame::~ChessGame() {
}


void ChessGame::Chess_process(CvPoint input1[], int MOVE_MODE) {
	CvPoint _TMove[4];

	switch (MOVE_MODE) {
		case CASTLING_MOVE:					// 캐슬링
			for (int i = 0; i < 4; i++)
				_TMove[i] = input1[i];

			break;
		case ENPASSANT_MOVE:				// 앙파상
			for (int i = 0; i < 3; i++)
				_TMove[i] = input1[i];

			int *_TValue[3];
			_TValue[0] = &_Board[_TMove[0].y][_TMove[0].x];
			_TValue[1] = &_Board[_TMove[1].y][_TMove[1].x];
			_TValue[2] = &_Board[_TMove[2].y][_TMove[2].x];

			int *white, *zero_pic, *black;
			for (int i = 0; i < 3; i++){
				if (*_TValue[i] == 0)
					zero_pic = _TValue[0];
				else if (*_TValue[i] == W_Pawn)
					white = _TValue[i];
				else if (*_TValue[i] == B_Pawn)
					black = _TValue[i];
			}

			if (_Turn == true) {
			
			}
			else if (_Turn == false) {

	//>>>>>>> CVES_HandRecognition:CVES/chess_game.cpp
			}

			break;
		default :
			// 기본 무브
			for (int i = 0; i < 2; i++)
				_TMove[i] = input1[i];

			int _TValue1, _TValue2;
			_TValue1 = _Board[_TMove[0].y][_TMove[0].x];
			_TValue2 = _Board[_TMove[1].y][_TMove[1].x];

			// 체스 무브 진행
			if (_Turn == true){
				// 백색 차례일때

				if (1 <= _TValue1 && _TValue1 <= 6){
					_Board[_TMove[1].y][_TMove[1].x] = 0;
					SWAP(_Board[_TMove[0].y][_TMove[0].x], _Board[_TMove[1].y][_TMove[1].x]);
				}
				else if (1 <= _TValue2 && _TValue2 <= 6){
					_Board[_TMove[0].y][_TMove[0].x] = 0;
					SWAP(_Board[_TMove[0].y][_TMove[0].x], _Board[_TMove[1].y][_TMove[1].x]);
				}
			}
			else if (_Turn == false) {
				//검은색 차례일때

				if (7 <= _TValue1 && _TValue1 <= 12) {
					_Board[_TMove[1].y][_TMove[1].x] = 0;
					SWAP(_Board[_TMove[0].y][_TMove[0].x], _Board[_TMove[1].y][_TMove[1].x]);
				}
				else if (7 <= _TValue2 && _TValue2 <= 12) {
					_Board[_TMove[0].y][_TMove[0].x] = 0;
					SWAP(_Board[_TMove[0].y][_TMove[0].x], _Board[_TMove[1].y][_TMove[1].x]);
				}
			}
			break;
	}

	_Turn = !_Turn;
}

void ChessGame::Show_chess_board() {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			printf("%3d", _Board[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}