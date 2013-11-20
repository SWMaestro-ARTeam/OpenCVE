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

#pragma region Constructor & Destructor
ChessGame::ChessGame() {
	_Turn = true; // 백

	for (register int i = 0; i < 8; i++)
		for (register int j = 0; j < 8; j++)
			_Board[i][j] = Ground;

	for (register int i = 0; i < 8; i++)
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

	_ChessBoard_Image = cvLoadImage("./Chess_UI/Chessboard.png", CV_LOAD_IMAGE_UNCHANGED);

	_BeforeMove._Piece = -1;
	_BeforeMove._Position = cvPoint(-1,-1);

	for(int i=0;i<8;i++){
		_W_PawnMove[i] = false;
		_B_PawnMove[i] = false;
	}

	_TempMove._Movement = new char[32];
	memset(_TempMove._Movement, NULL, sizeof(_TempMove._Movement));

	_error_move._Flag = false;
}

ChessGame::~ChessGame() {
	cvReleaseImage(&_ChessBoard_Image);
}
#pragma endregion Constructor & Destructor

#pragma region Private Functions
void ChessGame::Make_UCICoordinate(CvPoint Before, CvPoint After) {
	char buf[6];

	memset(buf, NULL, sizeof(buf));
	sprintf(buf, "%c%d%c%d", Mapping_UCICharacter(Before.x), Before.y+1, Mapping_UCICharacter(After.x), After.y+1);
	//buf[5] = '\0';

	strcpy(_TempMove._Movement, buf);
}

char ChessGame::Mapping_UCICharacter(int Position) {
	switch (Position) {
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

void ChessGame::Moving_Castling(CvPoint Moving_Input[]){
	int _TValue1, _TValue2, _TValue3, _TValue4;

	CvPoint _T_King = cvPoint(-1,-1), _T_Rook = cvPoint(-1,-1);
	vector<CvPoint> _T_Ground;

	if (_Turn == WHITE_TURN) {
		_TempMove._TurnFlag = WHITE_TURN;
		for (register int i = 0; i < 4; i++) {
			if (_Board[Moving_Input[i].x][Moving_Input[i].y] == W_King)
				_T_King = cvPoint(Moving_Input[i].x, Moving_Input[i].y);
			else if (_Board[Moving_Input[i].x][Moving_Input[i].y] == W_Rook)
				_T_Rook = cvPoint(Moving_Input[i].x, Moving_Input[i].y);
			else if (_Board[Moving_Input[i].x][Moving_Input[i].y] == Ground)
				_T_Ground.push_back(cvPoint(Moving_Input[i].x, Moving_Input[i].y));
		}
		if (_T_King.x != -1 && _T_Rook.x != -1 && _T_Ground.size()) {
			if (_T_King.x < _T_Rook.x) {
				if (_T_Ground[0].x < _T_Ground[1].x) {
					_V_SWAP(_Board[_T_Ground[0].x][_T_Ground[0].y], _Board[_T_Rook.x][_T_Rook.y]);
					_V_SWAP(_Board[_T_Ground[1].x][_T_Ground[1].y], _Board[_T_King.x][_T_King.y]);

					Make_UCICoordinate(_T_King, _T_Ground[1]);
					_BeforeMove._Piece = W_King;
					_BeforeMove._Position = _T_Ground[1];
				}
				else if (_T_Ground[0].x > _T_Ground[1].x) {
					_V_SWAP(_Board[_T_Ground[0].x][_T_Ground[0].y], _Board[_T_King.x][_T_King.y]);
					_V_SWAP(_Board[_T_Ground[1].x][_T_Ground[1].y], _Board[_T_Rook.x][_T_Rook.y]);

					Make_UCICoordinate(_T_King, _T_Ground[0]);
					_BeforeMove._Piece = W_King;
					_BeforeMove._Position = _T_Ground[0];
				}
			}
			else if (_T_King.x > _T_Rook.x) {
				if (_T_Ground[0].x < _T_Ground[1].x) {
					_V_SWAP(_Board[_T_Ground[0].x][_T_Ground[0].y], _Board[_T_King.x][_T_King.y]);
					_V_SWAP(_Board[_T_Ground[1].x][_T_Ground[1].y], _Board[_T_Rook.x][_T_Rook.y]);

					Make_UCICoordinate(_T_King, _T_Ground[0]);
					_BeforeMove._Piece = W_King;
					_BeforeMove._Position = _T_Ground[0];
				}
				else if (_T_Ground[0].x > _T_Ground[1].x) {
					_V_SWAP(_Board[_T_Ground[0].x][_T_Ground[0].y], _Board[_T_Rook.x][_T_Rook.y]);
					_V_SWAP(_Board[_T_Ground[1].x][_T_Ground[1].y], _Board[_T_King.x][_T_King.y]);

					Make_UCICoordinate(_T_King, _T_Ground[1]);
					_BeforeMove._Piece = W_King;
					_BeforeMove._Position = _T_Ground[1];
				}
			}
		}
		else {
			Moving_Default(Moving_Input);
			return;
		}
	}
	else if (_Turn == BLACK_TURN) {
		_TempMove._TurnFlag = BLACK_TURN;
		for (register int i = 0; i < 4; i++) {
			if (_Board[Moving_Input[i].x][Moving_Input[i].y] == B_King)
				_T_King = cvPoint(Moving_Input[i].x, Moving_Input[i].y);
			else if (_Board[Moving_Input[i].x][Moving_Input[i].y] == B_Rook)
				_T_Rook = cvPoint(Moving_Input[i].x, Moving_Input[i].y);
			else if (_Board[Moving_Input[i].x][Moving_Input[i].y] == Ground)
				_T_Ground.push_back(cvPoint(Moving_Input[i].x, Moving_Input[i].y));
		}

		if (_T_King.x != -1 && _T_Rook.x != -1 && _T_Ground.size()) {
			if (_T_King.x < _T_Rook.x) {
				if (_T_Ground[0].x < _T_Ground[1].x) {
					_V_SWAP(_Board[_T_Ground[0].x][_T_Ground[0].y], _Board[_T_Rook.x][_T_Rook.y]);
					_V_SWAP(_Board[_T_Ground[1].x][_T_Ground[1].y], _Board[_T_King.x][_T_King.y]);

					Make_UCICoordinate(_T_King, _T_Ground[1]);
					_BeforeMove._Piece = W_King;
					_BeforeMove._Position = _T_Ground[1];
				}
				else if (_T_Ground[0].x > _T_Ground[1].x) {
					_V_SWAP(_Board[_T_Ground[0].x][_T_Ground[0].y], _Board[_T_King.x][_T_King.y]);
					_V_SWAP(_Board[_T_Ground[1].x][_T_Ground[1].y], _Board[_T_Rook.x][_T_Rook.y]);

					Make_UCICoordinate(_T_King, _T_Ground[0]);
					_BeforeMove._Piece = W_King;
					_BeforeMove._Position = _T_Ground[0];
				}
			}
			else if (_T_King.x > _T_Rook.x) {
				if(_T_Ground[0].x < _T_Ground[1].x){
					_V_SWAP(_Board[_T_Ground[0].x][_T_Ground[0].y], _Board[_T_King.x][_T_King.y]);
					_V_SWAP(_Board[_T_Ground[1].x][_T_Ground[1].y], _Board[_T_Rook.x][_T_Rook.y]);

					Make_UCICoordinate(_T_King, _T_Ground[0]);
					_BeforeMove._Piece = W_King;
					_BeforeMove._Position = _T_Ground[0];
				}
				else if (_T_Ground[0].x > _T_Ground[1].x) {
					_V_SWAP(_Board[_T_Ground[0].x][_T_Ground[0].y], _Board[_T_Rook.x][_T_Rook.y]);
					_V_SWAP(_Board[_T_Ground[1].x][_T_Ground[1].y], _Board[_T_King.x][_T_King.y]);

					Make_UCICoordinate(_T_King, _T_Ground[1]);
					_BeforeMove._Piece = W_King;
					_BeforeMove._Position = _T_Ground[1];
				}
			}
		}
		else {
			Moving_Default(Moving_Input);
			return;
		}
	}

	_Turn = !_Turn;

	strcpy(_RecentMove, _TempMove._Movement);
	_Chess_Movement.push(_TempMove);
}

void ChessGame::Moving_Enpassant(CvPoint Moving_Input[]) {
	int _TValue1, _TValue2, _TValue3;

	_TValue1 = _Board[Moving_Input[0].x][Moving_Input[0].y];
	_TValue2 = _Board[Moving_Input[1].x][Moving_Input[1].y];
	_TValue3 = _Board[Moving_Input[2].x][Moving_Input[2].y];

	for(int i=0;i<3;i++){
		for(int j=0;j<3;j++){
			if(i == j) continue;

			if(abs(Moving_Input[i].x - Moving_Input[j].x) > 1 || abs(Moving_Input[i].y - Moving_Input[j].y) > 1){
				Moving_Default(Moving_Input);
				return;
			}
		}
	}

	if (_Turn == WHITE_TURN) {
		_TempMove._TurnFlag = WHITE_TURN;

		if (_TValue1 == Ground && Moving_Input[0].y - 1 == _BeforeMove._Position.y) {
			if (_TValue2 == W_Pawn) {
				// 1. 폰이 이동할 예정인 빈공간과 현재 이동한 폰의 위치를 스왑한다
				// 2. 나머지 하나는 다른 색 폰이므로 제거를 해주기 위해 Ground로 바꾸어준다

				_V_SWAP(_Board[Moving_Input[0].x][Moving_Input[0].y],_Board[Moving_Input[1].x][Moving_Input[1].y]);
				_Board[Moving_Input[2].x][Moving_Input[2].y] = Ground;

				Make_UCICoordinate(Moving_Input[1], Moving_Input[0]);
				_BeforeMove._Piece = W_Pawn;
				_BeforeMove._Position = Moving_Input[0];
			}
			else if (_TValue3 == W_Pawn) {
				_V_SWAP(_Board[Moving_Input[0].x][Moving_Input[0].y],_Board[Moving_Input[2].x][Moving_Input[2].y]);
				_Board[Moving_Input[1].x][Moving_Input[1].y] = Ground;

				Make_UCICoordinate(Moving_Input[2], Moving_Input[0]);
				_BeforeMove._Piece = W_Pawn;
				_BeforeMove._Position = Moving_Input[0];
			}
		}
		else if (_TValue2 == Ground && Moving_Input[1].y - 1 == _BeforeMove._Position.y) {
			if (_TValue1 == W_Pawn) {
				_V_SWAP(_Board[Moving_Input[1].x][Moving_Input[1].y],_Board[Moving_Input[0].x][Moving_Input[0].y]);
				_Board[Moving_Input[2].x][Moving_Input[2].y] = Ground;

				Make_UCICoordinate(Moving_Input[0], Moving_Input[1]);
				_BeforeMove._Piece = W_Pawn;
				_BeforeMove._Position = Moving_Input[1];
			}
			else if (_TValue3 == W_Pawn) {
				_V_SWAP(_Board[Moving_Input[1].x][Moving_Input[1].y],_Board[Moving_Input[2].x][Moving_Input[2].y]);
				_Board[Moving_Input[0].x][Moving_Input[0].y] = Ground;

				Make_UCICoordinate(Moving_Input[2], Moving_Input[1]);
				_BeforeMove._Piece = W_Pawn;
				_BeforeMove._Position = Moving_Input[1];
			}
		}
		else if (_TValue3 == Ground && Moving_Input[2].y - 1 == _BeforeMove._Position.y) {
			if (_TValue1 == W_Pawn){
				_V_SWAP(_Board[Moving_Input[2].x][Moving_Input[2].y],_Board[Moving_Input[0].x][Moving_Input[0].y]);
				_Board[Moving_Input[1].x][Moving_Input[1].y] = Ground;

				Make_UCICoordinate(Moving_Input[0], Moving_Input[2]);
				_BeforeMove._Piece = W_Pawn;
				_BeforeMove._Position = Moving_Input[2];
			}
			else if (_TValue2 == W_Pawn) {
				_V_SWAP(_Board[Moving_Input[2].x][Moving_Input[2].y],_Board[Moving_Input[1].x][Moving_Input[1].y]);
				_Board[Moving_Input[0].x][Moving_Input[0].y] = Ground;

				Make_UCICoordinate(Moving_Input[1], Moving_Input[2]);
				_BeforeMove._Piece = W_Pawn;
				_BeforeMove._Position = Moving_Input[2];
			}
		}
		else {
			Moving_Default(Moving_Input);
			return;
		}
	}
	else if (_Turn == BLACK_TURN) {
		_TempMove._TurnFlag = BLACK_TURN;

		if(_TValue1 == Ground && Moving_Input[0].y + 1 == _BeforeMove._Position.x){
			if(_TValue2 == B_Pawn){
				_V_SWAP(_Board[Moving_Input[0].x][Moving_Input[0].y],_Board[Moving_Input[1].x][Moving_Input[1].y]);
				_Board[Moving_Input[2].x][Moving_Input[2].y] = Ground;

				Make_UCICoordinate(Moving_Input[1], Moving_Input[0]);
				_BeforeMove._Piece = B_Pawn;
				_BeforeMove._Position = Moving_Input[0];
			}
			else if (_TValue3 == B_Pawn) {
				_V_SWAP(_Board[Moving_Input[0].x][Moving_Input[0].y],_Board[Moving_Input[2].x][Moving_Input[2].y]);
				_Board[Moving_Input[1].x][Moving_Input[1].y] = Ground;

				Make_UCICoordinate(Moving_Input[2], Moving_Input[0]);
				_BeforeMove._Piece = B_Pawn;
				_BeforeMove._Position = Moving_Input[0];
			}
		}
		else if (_TValue2 == Ground && Moving_Input[1].y + 1 == _BeforeMove_Pawn.x) {
			if (_TValue1 == B_Pawn) {
				_V_SWAP(_Board[Moving_Input[1].x][Moving_Input[1].y],_Board[Moving_Input[0].x][Moving_Input[0].y]);
				_Board[Moving_Input[2].x][Moving_Input[2].y] = Ground;

				Make_UCICoordinate(Moving_Input[0], Moving_Input[1]);
				_BeforeMove._Piece = B_Pawn;
				_BeforeMove._Position = Moving_Input[1];
			}
			else if (_TValue3 == B_Pawn) {
				_V_SWAP(_Board[Moving_Input[1].x][Moving_Input[1].y],_Board[Moving_Input[2].x][Moving_Input[2].y]);
				_Board[Moving_Input[0].x][Moving_Input[0].y] = Ground;

				Make_UCICoordinate(Moving_Input[2], Moving_Input[1]);
				_BeforeMove._Piece = B_Pawn;
				_BeforeMove._Position = Moving_Input[1];
			}
		}
		else if (_TValue3 == Ground && Moving_Input[2].y + 1 == _BeforeMove_Pawn.x) {
			if (_TValue1 == B_Pawn) {
				_V_SWAP(_Board[Moving_Input[2].x][Moving_Input[2].y],_Board[Moving_Input[0].x][Moving_Input[0].y]);
				_Board[Moving_Input[1].x][Moving_Input[1].y] = Ground;

				Make_UCICoordinate(Moving_Input[0], Moving_Input[2]);
				_BeforeMove._Piece = B_Pawn;
				_BeforeMove._Position = Moving_Input[2];
			}
			else if (_TValue2 == B_Pawn) {
				_V_SWAP(_Board[Moving_Input[2].x][Moving_Input[2].y],_Board[Moving_Input[1].x][Moving_Input[1].y]);
				_Board[Moving_Input[0].x][Moving_Input[0].y] = Ground;

				Make_UCICoordinate(Moving_Input[1], Moving_Input[2]);
				_BeforeMove._Piece = B_Pawn;
				_BeforeMove._Position = Moving_Input[2];
			}
		}
		else {
			Moving_Default(Moving_Input);
			return;
		}
	}

	_Turn = !_Turn;

	strcpy(_RecentMove, _TempMove._Movement);
	_Chess_Movement.push(_TempMove);
}

void ChessGame::Moving_Default(CvPoint Moving_Input[]) {
	int _TValue1, _TValue2;

	memset(_TempMove._Movement, NULL, sizeof(_TempMove._Movement));
	_TValue1 = _Board[Moving_Input[0].x][Moving_Input[0].y];
	_TValue2 = _Board[Moving_Input[1].x][Moving_Input[1].y];

	// 이전 움직임 초기화
	_BeforeMove._Position = cvPoint(-1,-1);
	_BeforeMove._Piece = -1;
	// 체스 무브 진행
	if (_Turn == WHITE_TURN) {
		// 백색 차례일때
		_TempMove._TurnFlag = WHITE_TURN;

		if (W_King <= _TValue1 && _TValue1 <= W_Pawn) {
			if (Rule_DefaultMove(Moving_Input[0], Moving_Input[1])) {
				_Board[Moving_Input[1].x][Moving_Input[1].y] = Ground;
				_V_SWAP(_Board[Moving_Input[0].x][Moving_Input[0].y], _Board[Moving_Input[1].x][Moving_Input[1].y]);

				if (_TValue1 == W_Pawn && Moving_Input[1].y == 7)
					_Board[Moving_Input[1].x][Moving_Input[1].y] = W_Queen;

				if(_TValue1 == W_Pawn && Moving_Input[1].y == 7)
					_Board[Moving_Input[1].x][Moving_Input[1].y] = W_Queen;

				Make_UCICoordinate(Moving_Input[0], Moving_Input[1]);
				_BeforeMove._Piece = _TValue1;
				_BeforeMove._Position = Moving_Input[1];

				_Turn = !_Turn;
			}
			else{

				//////////////////////////////////////////////////////////////////////////
				// 체스룰에 반하는 무브가 들어왔을때

				_error_move._Flag = true;
				_error_move._Before = Moving_Input[1];
				_error_move._After = Moving_Input[0];
				
				return;
			}
		}
		else if (W_King <= _TValue2 && _TValue2 <= W_Pawn) {
			if (Rule_DefaultMove(Moving_Input[1], Moving_Input[0])) {
				_Board[Moving_Input[0].x][Moving_Input[0].y] = Ground;
				_V_SWAP(_Board[Moving_Input[0].x][Moving_Input[0].y], _Board[Moving_Input[1].x][Moving_Input[1].y]);

				if (_TValue2 == W_Pawn && Moving_Input[0].y == 7)
					_Board[Moving_Input[0].x][Moving_Input[0].y] = W_Queen;

				Make_UCICoordinate(Moving_Input[1], Moving_Input[0]);
				_BeforeMove._Piece = _TValue2;
				_BeforeMove._Position = Moving_Input[0];

				_Turn = !_Turn;
			}
			else{

				_error_move._Flag = true;
				_error_move._Before = Moving_Input[0];
				_error_move._After = Moving_Input[1];
				
				return;
			}
		}
		else{
			//////////////////////////////////////////////////////////////////////////
			// 현재 턴에 맞는 말이 아닌 다른 말을 움직였을 경우

			if(_TValue1 == Ground){
				_error_move._Flag = true;
				_error_move._Before = Moving_Input[0];
				_error_move._After = Moving_Input[1];
			}
			else if(_TValue2 == Ground){
				_error_move._Flag = true;
				_error_move._Before = Moving_Input[1];
				_error_move._After = Moving_Input[0];
			}
		}
	}
	else if (_Turn == BLACK_TURN) {
		//검은색 차례일때
		_TempMove._TurnFlag = BLACK_TURN;

		if (B_King <= _TValue1 && _TValue1 <= B_Pawn) {
			if (Rule_DefaultMove(Moving_Input[0], Moving_Input[1])) {
				_Board[Moving_Input[1].x][Moving_Input[1].y] = Ground;
				_V_SWAP(_Board[Moving_Input[0].x][Moving_Input[0].y], _Board[Moving_Input[1].x][Moving_Input[1].y]);

				if(_TValue1 == B_Pawn && Moving_Input[1].y == 0)
					_Board[Moving_Input[1].x][Moving_Input[1].y] = B_Queen;

				Make_UCICoordinate(Moving_Input[0], Moving_Input[1]);
				_BeforeMove._Piece = _TValue1;
				_BeforeMove._Position = Moving_Input[1];

				_Turn = !_Turn;
			}
			else {
				_error_move._Flag = true;
				_error_move._Before = Moving_Input[1];
				_error_move._After = Moving_Input[0];
				return;
			}
		}
		else if (B_King <= _TValue2 && _TValue2 <= B_Pawn) {
			if (Rule_DefaultMove(Moving_Input[1], Moving_Input[0])) {
				_Board[Moving_Input[0].x][Moving_Input[0].y] = Ground;
				_V_SWAP(_Board[Moving_Input[0].x][Moving_Input[0].y], _Board[Moving_Input[1].x][Moving_Input[1].y]);

				if (_TValue2 == B_Pawn && Moving_Input[0].y == 0)
					_Board[Moving_Input[0].x][Moving_Input[0].y] = B_Queen;

				Make_UCICoordinate(Moving_Input[1], Moving_Input[0]);
				_BeforeMove._Piece = _TValue2;
				_BeforeMove._Position = Moving_Input[0];

				_Turn = !_Turn;
			}
			else {
				_error_move._Flag = true;
				_error_move._Before = Moving_Input[0];
				_error_move._After = Moving_Input[1];
				return;
			}
		}
		else {
			// 자신의 턴이 아닐때 체스말을 움직임.
			if(_TValue1 == Ground){
				_error_move._Flag = true;
				_error_move._Before = Moving_Input[0];
				_error_move._After = Moving_Input[1];
			}
			else if(_TValue2 == Ground){
				_error_move._Flag = true;
				_error_move._Before = Moving_Input[1];
				_error_move._After = Moving_Input[0];
			}
		}
	}
	memset(_RecentMove, NULL, sizeof(_RecentMove));
	//recent_move에 복사
	strcpy(_RecentMove, _TempMove._Movement);
	_Chess_Movement.push(_TempMove);
}

bool ChessGame::Rule_DefaultMove(CvPoint Before, CvPoint After) {
	if (_Board[Before.x][Before.y] == W_Pawn) {
		if (_W_PawnMove[Before.x] == false) {
			// 1. 맨 처음 1칸 또는 2칸 전진 그리고 해당 위치에 말이 없을 경우.
			// 2. 공격시 오늘쪽 위아래에 해당 말이 존재 할 경우.
			if ((After.y == Before.y + 1 && After.x == Before.x && _Board[After.x][After.y] == Ground) || (After.y == Before.y + 2 && After.x == Before.x && _Board[After.x][After.y] == Ground) || 
				(After.x == Before.x - 1 && After.y == Before.y + 1 && _Board[After.x][After.y] != Ground) || (After.x == Before.x + 1 && After.y == Before.y + 1 && _Board[After.x][After.y] != Ground)) {
				_W_PawnMove[Before.x] = true;
				return true;
			}
		}
		else {
			if ((After.y == Before.y + 1 && After.x == Before.x && _Board[After.x][After.y] == Ground) || 
				(After.x == Before.x - 1 && After.y == Before.y + 1 && _Board[After.x][After.y] != Ground) || (After.x == Before.x + 1 && After.y == Before.y + 1 && _Board[After.x][After.y] != Ground))
				return true;
		}
	}
	else if (_Board[Before.x][Before.y] == B_Pawn) {
		if (_B_PawnMove[Before.x] == false) {
			// 1. 맨 처음 1칸 또는 2칸 전진 그리고 해당 위치에 말이 없을 경우
			// 2. 공격시 오늘쪽 위아래에 해당 말이 존재 할 경우
			if ((After.y == Before.y - 1 && After.x == Before.x && _Board[After.x][After.y] == Ground) || (After.y == Before.y - 2 && After.x == Before.x && _Board[After.x][After.y] == Ground) || 
				(After.x == Before.x - 1 && After.y == Before.y - 1 && _Board[After.x][After.y] != Ground) || (After.x == Before.x + 1 && After.y == Before.y - 1 && _Board[After.x][After.y] != Ground)) {
				_B_PawnMove[Before.x] = true;
				return true;
			}
		}
		else {
			if ((After.y == Before.y - 1 && After.x == Before.x && _Board[After.x][After.y] == Ground) || 
				(After.x == Before.x - 1 && After.y == Before.y - 1 && _Board[After.x][After.y] != Ground) || (After.x == Before.x + 1 && After.y == Before.y - 1 && _Board[After.x][After.y] != Ground))
				return true;
		}
	}
	else if (_Board[Before.x][Before.y] == W_King || _Board[Before.x][Before.y] == B_King) {
		// 8방향 검사.
		if ((After.x == Before.x && After.y == Before.y - 1) || (After.x == Before.x - 1 && After.y == Before.y - 1) || (After.x == Before.x - 1 && After.y == Before.y) || 
			(After.x == Before.x - 1 && After.y == Before.y + 1) || (After.x == Before.x && After.y == Before.y + 1) || (After.x == Before.x + 1 && After.y == Before.y + 1) ||
			(After.x == Before.x + 1 && After.y == Before.y) || (After.x == Before.x + 1 && After.y == Before.y - 1))
			return true;
	}
	else if (_Board[Before.x][Before.y] == W_Queen || _Board[Before.x][Before.y] == B_Queen) {
		// 가로 세로 직선, 전과 현재의 이동 위치의 차이의 절대값이 같으면 대각선상에 위치한다 판별
		if ((After.x == Before.x && After.y != Before.y) || (After.x != Before.x && After.y == Before.y) || (abs(After.x - Before.x) == abs(After.y - Before.y)))
			return true;
	}
	else if (_Board[Before.x][Before.y] == W_Rook || _Board[Before.x][Before.y] == B_Rook) {
		if ((After.x == Before.x && After.y != Before.y) || (After.x != Before.x && After.y == Before.y))
			return true;
	}
	else if (_Board[Before.x][Before.y] == W_Bishop || _Board[Before.x][Before.y] == B_Bishop) {
		if (abs(After.x - Before.x) == abs(After.y - Before.y))
			return true;
	}
	else if (_Board[Before.x][Before.y] == W_Knight || _Board[Before.x][Before.y] == B_Knight) {
		// 각 나이트의 이동경로를 하드코딩 ^오^ // ???????? 이 주석은 대체 뭐지?
		// 주석 제대로 써놓을거 아니면 빨리 지우길 바람.
		if((After.x == Before.x - 1 && After.y == Before.y - 2) || (After.x == Before.x + 1 && After.y == Before.y - 2) ||
			(After.x == Before.x - 2 && After.y == Before.y - 1) || (After.x == Before.x - 2 && After.y == Before.y + 1) ||
			(After.x == Before.x - 1 && After.y == Before.y + 2) || (After.x == Before.x + 1 && After.y == Before.y + 2) || 
			(After.x == Before.x + 2 && After.y == Before.y - 1) || (After.x == Before.x + 2 && After.y == Before.y + 1))
			return true;
	}

	return false;
}

//void ChessGame::cvQuiver(IplImage *Image, int _X, int _Y, int _U, int _V, CvScalar Colour, int Size, int Thickness) {
//	CvPoint _TPt1, _TPt2;
//	double _TTheta;
//	double _TPI = 3.1416;
//
//	if (_U == 0)
//		_TTheta = _TPI / 2;
//	else
//		_TTheta = atan2(double(_V),(double)(_U));
//
//	_TPt1.x = _X;
//	_TPt1.y = _Y;
//
//	_TPt2.x = _X + _U;
//	_TPt2.y = _Y + _V;
//
//	cvDrawLine(Image,_TPt1,_TPt2,Colour,Thickness,8);  //Draw Line
//
//	Size = (int)(Size * 0.707);
//
//	if (_TTheta == _TPI/2 && _TPt1.y > _TPt2.y){
//		_TPt1.x = (int)((Size * cos(_TTheta)) - (Size * sin(_TTheta)) + _TPt2.x);
//		_TPt1.y = (int)((Size * sin(_TTheta)) + (Size * cos(_TTheta)) + _TPt2.y);
//		cvDrawLine(Image, _TPt1, _TPt2, Colour, Thickness, 8); //Draw Line
//
//		_TPt1.x = (int)(Size * cos(_TTheta) + Size * sin(_TTheta) + _TPt2.x);
//		_TPt1.y = (int)(Size * sin(_TTheta) - Size * cos(_TTheta) + _TPt2.y);
//		cvDrawLine(Image, _TPt1, _TPt2, Colour, Thickness, 8); //Draw Line
//	}
//	else{
//		_TPt1.x = (int)(-Size * cos(_TTheta) - Size * sin(_TTheta) + _TPt2.x);
//		_TPt1.y = (int)(-Size * sin(_TTheta) + Size * cos(_TTheta) + _TPt2.y);
//		cvDrawLine(Image, _TPt1, _TPt2, Colour, Thickness, 8);  //Draw Line
//
//		_TPt1. x =(int)(-Size * cos(_TTheta) + Size * sin(_TTheta) + _TPt2.x);
//		_TPt1.y = (int)(-Size * sin(_TTheta) - Size * cos(_TTheta) + _TPt2.y);
//		cvDrawLine(Image, _TPt1, _TPt2, Colour, Thickness, 8);  //Draw Line
//	}
//=======
//void ChessGame::cvQuiver(IplImage *Image, CvPoint pt1, CvPoint pt2, CvScalar Color,int Size,int Thickness){
//	double Theta;
//	double PI = 3.1416;
//	if(pt2.x == 0)
//		Theta = PI/2;
//	else
//	{
//		Theta = atan2(double(pt2.y - pt1.y), (double)(pt2.x - pt1.x));
//	}
//	cvLine(Image, pt1, pt2, Color, Thickness, 8);
//	Size = (int)(Size*0.707);
//	double arrow_length = sqrt(pow((float)(pt1.y - pt2.y),2) + pow((float)(pt1.x - pt2.x), 2));
//
//	if(Theta = PI/2 && pt1.y > pt2.y){
//		pt1.x = (int)(Size*cos(Theta) - Size*sin(Theta) + pt2.x);
//		pt1.y = (int)(Size*sin(Theta) + Size*cos(Theta) + pt2.y);
//		cvLine(Image, pt1, pt2, Color, Thickness, 8);
//		pt1.x = (int)(Size*cos(Theta) + Size*sin(Theta) + pt2.x);
//		pt1.y = (int)(Size*sin(Theta) - Size*cos(Theta) + pt2.y);
//		cvLine(Image, pt1, pt2, Color, Thickness, 8);
//	}
//	else{
//		pt1.x = (int)(-Size*cos(Theta) - Size*sin(Theta) + pt2.x);
//		pt1.y = (int)(-Size*sin(Theta) + Size*cos(Theta) + pt2.y);
//		cvLine(Image, pt1, pt2, Color, Thickness, 8);
//		pt1.x = (int)(-Size*cos(Theta) + Size*sin(Theta) + pt2.x);
//		pt1.y = (int)(-Size*sin(Theta) - Size*cos(Theta) + pt2.y);
//		cvLine(Image, pt1, pt2, Color, Thickness, 8);
//	}
//}
void ChessGame::drawArrow(IplImage *img, CvPoint pStart, CvPoint pEnd, int len, int alpha, CvScalar color, int thickness, int lineType)
{    
	const double PI = 3.1415926;    
	CvPoint arrow;
	double angle = atan2((double)(pStart.y - pEnd.y), (double)(pStart.x - pEnd.x));  
	cvLine(img, pStart, pEnd, color, thickness, lineType);
	arrow.x = pEnd.x + len * cos(angle + PI * alpha / 180);     
	arrow.y = pEnd.y + len * sin(angle + PI * alpha / 180);  
	cvLine(img, pEnd, arrow, color, thickness, lineType);   
	arrow.x = pEnd.x + len * cos(angle - PI * alpha / 180);     
	arrow.y = pEnd.y + len * sin(angle - PI * alpha / 180);    
	cvLine(img, pEnd, arrow, color, thickness, lineType);
//>>>>>>> origin/CVES_NewEngine_Extended
}
#pragma endregion Private Functions

#pragma region Public Functions
bool ChessGame::Chess_Process(CvPoint Input[], int MOVE_MODE) {
	CvPoint _TMove[4];

	switch (MOVE_MODE) {
		case CASTLING_MOVE:
			for (register int i = 0; i < 4; i++)
				_TMove[i] = cvPoint(Input[i].x, Input[i].y);

			Moving_Castling(_TMove);
			break;
		case ENPASSANT_MOVE:
			for (register int i = 0; i < 4; i++)
				_TMove[i] = cvPoint(Input[i].x, Input[i].y);

			Moving_Enpassant(_TMove);
			break;
		case DEFAULT_MOVE:
			for (register int i = 0; i < 4; i++)
				_TMove[i] = cvPoint(Input[i].x, Input[i].y);

			Moving_Default(_TMove);
			break;
	}

	return !_Turn;
}

void ChessGame::Show_ChessBoard() {
	for (register int i = 0; i < 8; i++) {
		for (register int j = 0; j < 8; j++) {
			printf("%3d", _Board[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

void ChessGame::Show_ChessImage() {
	char _TTempBuffer[32];
	IplImage *_TTempGameBoard;
	_TTempGameBoard = cvCloneImage(_ChessBoard_Image);

	for (register int i = 0; i < 8; i++) {
		for (register int j = 0; j < 8; j++) {
			if (_Board[i][j] != 0) {
				sprintf(_TTempBuffer, "./Chess_UI/%d.png", _Board[i][j]);
				_ChessPiece = cvLoadImage(_TTempBuffer, CV_LOAD_IMAGE_UNCHANGED);

				//cvSetImageROI(chessboard_img, cvRect(j*64, i*64, 64, 64));
				// 알파값 추가 처리.
				for (register int k = 0; k < _ChessPiece->width; k++){
					for (register int l = 0; l < _ChessPiece->height; l++){
						unsigned char _T_B = (unsigned char)_ChessPiece->imageData[(k * 4) + (l * _ChessPiece->widthStep) + 0];
						unsigned char _T_G = (unsigned char)_ChessPiece->imageData[(k * 4) + (l * _ChessPiece->widthStep) + 1];
						unsigned char _T_R = (unsigned char)_ChessPiece->imageData[(k * 4) + (l * _ChessPiece->widthStep) + 2];
						unsigned char _T_A = (unsigned char)_ChessPiece->imageData[(k * 4) + (l * _ChessPiece->widthStep) + 3];

						// 아이콘이 불투명한 영역만 붙여 넣기.
						if (_T_A > 100){
							_TTempGameBoard->imageData[(j * 64 + k) * 4 + (i * 64 + l) * _TTempGameBoard->widthStep + 0] = _T_B;
							_TTempGameBoard->imageData[(j * 64 + k) * 4 + (i * 64 + l) * _TTempGameBoard->widthStep + 1] = _T_G;
							_TTempGameBoard->imageData[(j * 64 + k) * 4 + (i * 64 + l) * _TTempGameBoard->widthStep + 2] = _T_R;
						}
					}
				}

				//cvResetImageROI(chessboard_img);

				cvReleaseImage(&_ChessPiece);
			}
		}
	}
#if !defined(USING_QT)
	cvShowImage("ChessGame", _TTempGameBoard);
#endif
	cvReleaseImage(&_TTempGameBoard);
}

bool ChessGame::Check_Return(CvPoint move_input[]) {
	/*if(move_input[0].x == _error_move.after.x && move_input[0].y == _error_move.after.y && move_input[1].x == _error_move.before.x && move_input[1].y == _error_move.before.y){
		_error_move.flag = false;
		return true;
	}
	else if(move_input[0].x == _error_move.before.x && move_input[0].y == _error_move.before.y && move_input[1].x == _error_move.after.x && move_input[1].y == _error_move.after.y){
		_error_move.flag = false;
		return true;*/
	if (move_input[0].x == _error_move._After.x
		&& move_input[0].y == _error_move._After.y
		&& move_input[1].x == _error_move._Before.x
		&& move_input[1].y == _error_move._Before.y) {
		_error_move._Flag = false;
		return true;
	}
	else if (move_input[0].x == _error_move._Before.x
		&& move_input[0].y == _error_move._Before.y
		&& move_input[1].x == _error_move._After.x
		&& move_input[1].y == _error_move._After.y) {
		_error_move._Flag = false;
		return true;
	}

	return false;
}

string ChessGame::Get_RecentMove() {
	return string(_RecentMove);
}

int ChessGame::Read_Mode() {
	static int _TW_King_MOVED = false;
	static int _TB_King_MOVED = false;
	static int _TW_Rook_MOVED = false;
	static int _TB_Rook_MOVED = false;

	// 이전 움직임 좌표를 확인하여 flag를 지움
	if (_BeforeMove._Piece == W_King)
		_TW_King_MOVED = true;
	else if (_BeforeMove._Piece == B_King)
		_TB_King_MOVED = true;
	else if (_BeforeMove._Piece == W_Rook)
		_TW_Rook_MOVED = true;
	else if (_BeforeMove._Piece == B_Rook)
		_TB_Rook_MOVED = true;

	if (_BeforeMove._Position.x == -1 || _BeforeMove._Position.y == -1) {
		return DEFAULT_MOVE;
	}
	else {
		// 앙파상 & 캐슬링 판정부
		if (_Turn == WHITE_TURN) {
			if (_TW_King_MOVED == false && _TW_Rook_MOVED == false) {
				if ((_Board[1][0] == Ground && _Board[2][0] == Ground && _Board[3][0] == Ground) || (_Board[5][0] == Ground && _Board[6][0] == Ground))
					return CASTLING_MOVE;
			}
			
			if (_BeforeMove._Piece == B_Pawn)
				return ENPASSANT_MOVE;
			else
				return DEFAULT_MOVE;
		}
		else if (_Turn == BLACK_TURN) {
			if (_TB_King_MOVED == false && _TB_Rook_MOVED == false) {
				if ((_Board[1][7] == Ground && _Board[2][7] == Ground && _Board[3][7] == Ground) || (_Board[5][7] == Ground && _Board[6][7] == Ground))
					return CASTLING_MOVE;
			}
			
			if (_BeforeMove._Piece == W_Pawn) {
				return ENPASSANT_MOVE;
			}
			else
				return DEFAULT_MOVE;
		}
	}
}

bool ChessGame::Check_InvalidMove(IplImage *Source, vector<ChessPoint> _CP, CvPoint _out[], int ROI_X, int ROI_Y) {
	static bool _first_check = false;	// error move가 해당 함수에 처음으로 진입하였는지를 확인

	//if(_error_move.flag == true){
	if (_error_move._Flag == true) {
		//Draw_InvalidMove(Source, _CP, _error_move, ROI_X, ROI_Y);

		if (_first_check == true) {
			//if(check_return(_out) == true){
			if(Check_Return(_out) == true){
				_first_check = false;
			}
		}
		else {
			_first_check = true;
		}

		return true;
	}

	return false;
}

void ChessGame::Draw_InvalidMove(IplImage *Source, vector<ChessPoint> _CP, int ROI_X, int ROI_Y) {
	CvPoint p_before, p_after;
	CvPoint CP_before, CP_after;
	/*p_after = _error_move.after;
	p_before = _error_move.before;*/
	p_after = _error_move._After;
	p_before = _error_move._Before;

	bool after_complete, before_complete;
	after_complete = before_complete = false;

	for (register int i = 0; i < 81; i++) {
		ChessPoint _temp_CP = _CP.at(i);

		if(_temp_CP.Index.x == p_after.y && _temp_CP.Index.y == p_after.x){
			CP_after = cvPoint(((_temp_CP.Cordinate.x + ROI_X) + (_CP.at(i+10).Cordinate.x + ROI_X))/2, ((_temp_CP.Cordinate.y + ROI_Y) + (_CP.at(i+10).Cordinate.y + ROI_Y))/2);
		}

		if(_temp_CP.Index.x == p_before.y && _temp_CP.Index.y == p_before.x){
			CP_before = cvPoint(((_temp_CP.Cordinate.x + ROI_X) + (_CP.at(i+10).Cordinate.x + ROI_X))/2, ((_temp_CP.Cordinate.y + ROI_Y) + (_CP.at(i+10).Cordinate.y + ROI_Y))/2);
		}

		if (after_complete == true && before_complete == true)
			break;
	}

	cvDrawCircle(Source, CP_before, 10, cvScalar(0,0,255), 2);
	//cvQuiver(Source, CP_before, CP_after, cvScalar(0,0,255), 3, 3);
	drawArrow(Source, CP_before, CP_after, 10, 30, cvScalar(0,0,255), 3, 8);
	//cvDrawLine(Source, CP_before, CP_after, cvScalar(0,0,255), 3);
#if defined(DEBUG_MODE)
	cvShowImage("source", Source);
#endif
}

bool ChessGame::Return_errorFlag() {
	// 최근 움직임이 Invalild move라면 return true
	return _error_move._Flag;
}

#pragma endregion Public Functions
