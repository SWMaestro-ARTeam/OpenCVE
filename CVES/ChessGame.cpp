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

	chessboard_img = cvLoadImage("./Chess_DebugUI/Chessboard.png", CV_LOAD_IMAGE_UNCHANGED);
}

ChessGame::~ChessGame() {
	cvReleaseImage(&chessboard_img);
}


void ChessGame::Chess_process(CvPoint input1[], int MOVE_MODE) {
	CvPoint _TMove[4];
	move_format temp_move;

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

			if (_Turn == WHITE_TURN) {
			
			}
			else if (_Turn == BLACK_TURN) {

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
			if (_Turn == WHITE_TURN){
				// 백색 차례일때
				temp_move.turn_flag = WHITE_TURN;

				if (1 <= _TValue1 && _TValue1 <= 6){
					_Board[_TMove[1].y][_TMove[1].x] = 0;
					SWAP(_Board[_TMove[0].y][_TMove[0].x], _Board[_TMove[1].y][_TMove[1].x]);
					MakeUCI(_TMove[0], _TMove[1], &temp_move);
				}
				else if (1 <= _TValue2 && _TValue2 <= 6){
					_Board[_TMove[0].y][_TMove[0].x] = 0;
					SWAP(_Board[_TMove[0].y][_TMove[0].x], _Board[_TMove[1].y][_TMove[1].x]);
					MakeUCI(_TMove[1], _TMove[0], &temp_move);
				}
			}
			else if (_Turn == BLACK_TURN) {
				//검은색 차례일때
				temp_move.turn_flag = BLACK_TURN;

				if (7 <= _TValue1 && _TValue1 <= 12) {
					_Board[_TMove[1].y][_TMove[1].x] = 0;
					SWAP(_Board[_TMove[0].y][_TMove[0].x], _Board[_TMove[1].y][_TMove[1].x]);
					MakeUCI(_TMove[0], _TMove[1], &temp_move);
				}
				else if (7 <= _TValue2 && _TValue2 <= 12) {
					_Board[_TMove[0].y][_TMove[0].x] = 0;
					SWAP(_Board[_TMove[0].y][_TMove[0].x], _Board[_TMove[1].y][_TMove[1].x]);
					MakeUCI(_TMove[1], _TMove[0], &temp_move);
				}
			}

			//queue에 넣음
			_chess_movement.push(temp_move);

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

void ChessGame::Show_chessImage(){
	char temp_buf[32];
	IplImage *tempgame_board;
	tempgame_board = cvCloneImage(chessboard_img);

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if(_Board[i][j] != 0){
				sprintf(temp_buf, "./Chess_DebugUI/%d.png", _Board[i][j]);
				chess_piece = cvLoadImage(temp_buf, CV_LOAD_IMAGE_UNCHANGED);

				//cvSetImageROI(chessboard_img, cvRect(j*64, i*64, 64, 64));
				// 알파값 추가 처리.
				for(int k = 0; k < chess_piece->width; k++){
					for(int l = 0; l < chess_piece->height; l++){
						unsigned char B = (unsigned char)chess_piece->imageData[(k*4) + (l*chess_piece->widthStep) + 0];
						unsigned char G = (unsigned char)chess_piece->imageData[(k*4) + (l*chess_piece->widthStep) + 1];
						unsigned char R = (unsigned char)chess_piece->imageData[(k*4) + (l*chess_piece->widthStep) + 2];
						unsigned char A = (unsigned char)chess_piece->imageData[(k*4) + (l*chess_piece->widthStep) + 3];

						// 아이콘이 불투명한 영역만 붙여 넣기.
						if(A > 100){
							tempgame_board->imageData[(j*64+k)*4 + (i*64+l)*tempgame_board->widthStep + 0] = B;
							tempgame_board->imageData[(j*64+k)*4 + (i*64+l)*tempgame_board->widthStep + 1] = G;
							tempgame_board->imageData[(j*64+k)*4 + (i*64+l)*tempgame_board->widthStep + 2] = R;
						}
					}
				}

				//cvResetImageROI(chessboard_img);

				cvReleaseImage(&chess_piece);
			}
		}
	}

	cvShowImage("ChessGame", tempgame_board);
	cvReleaseImage(&tempgame_board);
}

void ChessGame::MakeUCI(CvPoint before, CvPoint after, move_format *dst){
	char buf[6];

	sprintf(buf, "%c%d%c%d", char_mapping(before.y), before.x+1, char_mapping(after.y), after.x+1);
	buf[5] = '\0';

	strcpy(dst->movement, buf);
}

void ChessGame::Get_RecentMove(char *str){
	move_format temp_move;
	char buf[6];

	//dequeue
	temp_move = _chess_movement.front();
	_chess_movement.pop();

	strcpy(buf, temp_move.movement);
	strcpy(str, buf);
}

char ChessGame::char_mapping(int position){
	switch(position){
	case 0:
		return 'a';
	case 1:
		return 'b';
	case 2:
		return 'c';
	case 3:
		return 'd';
	case 4:
		return 'e';
	case 5:
		return 'f';
	case 6:
		return 'g';
	case 7:
		return 'h';
	default:		//error
		break;
	}
}