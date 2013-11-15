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

	_BeforeMove.Piece = -1;
	_BeforeMove.position = cvPoint(-1,-1);

	for(int i=0;i<8;i++){
		_W_PawnMove[i] = false;
		_B_PawnMove[i] = false;
	}

	_TempMove._Movement = new char[32];
	memset(_TempMove._Movement, NULL, sizeof(_TempMove._Movement));
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
					_BeforeMove.Piece = W_King;
					_BeforeMove.position = _T_Ground[1];
				}
				else if (_T_Ground[0].x > _T_Ground[1].x) {
					_V_SWAP(_Board[_T_Ground[0].x][_T_Ground[0].y], _Board[_T_King.x][_T_King.y]);
					_V_SWAP(_Board[_T_Ground[1].x][_T_Ground[1].y], _Board[_T_Rook.x][_T_Rook.y]);

					Make_UCICoordinate(_T_King, _T_Ground[0]);
					_BeforeMove.Piece = W_King;
					_BeforeMove.position = _T_Ground[0];
				}
			}
			else if (_T_King.x > _T_Rook.x) {
				if (_T_Ground[0].x < _T_Ground[1].x) {
					_V_SWAP(_Board[_T_Ground[0].x][_T_Ground[0].y], _Board[_T_King.x][_T_King.y]);
					_V_SWAP(_Board[_T_Ground[1].x][_T_Ground[1].y], _Board[_T_Rook.x][_T_Rook.y]);

					Make_UCICoordinate(_T_King, _T_Ground[0]);
					_BeforeMove.Piece = W_King;
					_BeforeMove.position = _T_Ground[0];
				}
				else if (_T_Ground[0].x > _T_Ground[1].x) {
					_V_SWAP(_Board[_T_Ground[0].x][_T_Ground[0].y], _Board[_T_Rook.x][_T_Rook.y]);
					_V_SWAP(_Board[_T_Ground[1].x][_T_Ground[1].y], _Board[_T_King.x][_T_King.y]);

					Make_UCICoordinate(_T_King, _T_Ground[1]);
					_BeforeMove.Piece = W_King;
					_BeforeMove.position = _T_Ground[1];
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
					_BeforeMove.Piece = W_King;
					_BeforeMove.position = _T_Ground[1];
				}
				else if (_T_Ground[0].x > _T_Ground[1].x) {
					_V_SWAP(_Board[_T_Ground[0].x][_T_Ground[0].y], _Board[_T_King.x][_T_King.y]);
					_V_SWAP(_Board[_T_Ground[1].x][_T_Ground[1].y], _Board[_T_Rook.x][_T_Rook.y]);

					Make_UCICoordinate(_T_King, _T_Ground[0]);
					_BeforeMove.Piece = W_King;
					_BeforeMove.position = _T_Ground[0];
				}
			}
			else if (_T_King.x > _T_Rook.x) {
				if(_T_Ground[0].x < _T_Ground[1].x){
					_V_SWAP(_Board[_T_Ground[0].x][_T_Ground[0].y], _Board[_T_King.x][_T_King.y]);
					_V_SWAP(_Board[_T_Ground[1].x][_T_Ground[1].y], _Board[_T_Rook.x][_T_Rook.y]);

					Make_UCICoordinate(_T_King, _T_Ground[0]);
					_BeforeMove.Piece = W_King;
					_BeforeMove.position = _T_Ground[0];
				}
				else if (_T_Ground[0].x > _T_Ground[1].x) {
					_V_SWAP(_Board[_T_Ground[0].x][_T_Ground[0].y], _Board[_T_Rook.x][_T_Rook.y]);
					_V_SWAP(_Board[_T_Ground[1].x][_T_Ground[1].y], _Board[_T_King.x][_T_King.y]);

					Make_UCICoordinate(_T_King, _T_Ground[1]);
					_BeforeMove.Piece = W_King;
					_BeforeMove.position = _T_Ground[1];
				}
			}
		}
		else {
			Moving_Default(Moving_Input);
			return;
		}
	}

	strcpy(_RecentMove, _TempMove._Movement);
	_Chess_Movement.push(_TempMove);
}

void ChessGame::Moving_Enpassant(CvPoint Moving_Input[]) {
	int _TValue1, _TValue2, _TValue3;

	_TValue1 = _Board[Moving_Input[0].x][Moving_Input[0].y];
	_TValue2 = _Board[Moving_Input[1].x][Moving_Input[1].y];
	_TValue3 = _Board[Moving_Input[2].x][Moving_Input[2].y];

	if (_Turn == WHITE_TURN) {
		_TempMove._TurnFlag = WHITE_TURN;

		if (_TValue1 == Ground && Moving_Input[0].y - 1 == _BeforeMove.position.y) {
			if (_TValue2 == W_Pawn) {
				// 1. 폰이 이동할 예정인 빈공간과 현재 이동한 폰의 위치를 스왑한다
				// 2. 나머지 하나는 다른 색 폰이므로 제거를 해주기 위해 Ground로 바꾸어준다

				_V_SWAP(_Board[Moving_Input[0].x][Moving_Input[0].y],_Board[Moving_Input[1].x][Moving_Input[1].y]);
				_Board[Moving_Input[2].x][Moving_Input[2].y] = Ground;

				Make_UCICoordinate(Moving_Input[1], Moving_Input[0]);
				_BeforeMove.Piece = W_Pawn;
				_BeforeMove.position = Moving_Input[0];
			}
			else if (_TValue3 == W_Pawn) {
				_V_SWAP(_Board[Moving_Input[0].x][Moving_Input[0].y],_Board[Moving_Input[2].x][Moving_Input[2].y]);
				_Board[Moving_Input[1].x][Moving_Input[1].y] = Ground;

				Make_UCICoordinate(Moving_Input[2], Moving_Input[0]);
				_BeforeMove.Piece = W_Pawn;
				_BeforeMove.position = Moving_Input[0];
			}
		}
		else if (_TValue2 == Ground && Moving_Input[1].y - 1 == _BeforeMove.position.y) {
			if (_TValue1 == W_Pawn) {
				_V_SWAP(_Board[Moving_Input[1].x][Moving_Input[1].y],_Board[Moving_Input[0].x][Moving_Input[0].y]);
				_Board[Moving_Input[2].x][Moving_Input[2].y] = Ground;

				Make_UCICoordinate(Moving_Input[0], Moving_Input[1]);
				_BeforeMove.Piece = W_Pawn;
				_BeforeMove.position = Moving_Input[1];
			}
			else if (_TValue3 == W_Pawn) {
				_V_SWAP(_Board[Moving_Input[1].x][Moving_Input[1].y],_Board[Moving_Input[2].x][Moving_Input[2].y]);
				_Board[Moving_Input[0].x][Moving_Input[0].y] = Ground;

				Make_UCICoordinate(Moving_Input[2], Moving_Input[1]);
				_BeforeMove.Piece = W_Pawn;
				_BeforeMove.position = Moving_Input[1];
			}
		}
		else if (_TValue3 == Ground && Moving_Input[2].y - 1 == _BeforeMove.position.y) {
			if (_TValue1 == W_Pawn){
				_V_SWAP(_Board[Moving_Input[2].x][Moving_Input[2].y],_Board[Moving_Input[0].x][Moving_Input[0].y]);
				_Board[Moving_Input[1].x][Moving_Input[1].y] = Ground;

				Make_UCICoordinate(Moving_Input[0], Moving_Input[2]);
				_BeforeMove.Piece = W_Pawn;
				_BeforeMove.position = Moving_Input[2];
			}
			else if (_TValue2 == W_Pawn) {
				_V_SWAP(_Board[Moving_Input[2].x][Moving_Input[2].y],_Board[Moving_Input[1].x][Moving_Input[1].y]);
				_Board[Moving_Input[0].x][Moving_Input[0].y] = Ground;

				Make_UCICoordinate(Moving_Input[1], Moving_Input[2]);
				_BeforeMove.Piece = W_Pawn;
				_BeforeMove.position = Moving_Input[2];
			}
		}
		else {
			Moving_Default(Moving_Input);
			return;
		}
	}
	else if (_Turn == BLACK_TURN) {
		_TempMove._TurnFlag = BLACK_TURN;

		if(_TValue1 == Ground && Moving_Input[0].y + 1 == _BeforeMove.position.x){
			if(_TValue2 == B_Pawn){
				_V_SWAP(_Board[Moving_Input[0].x][Moving_Input[0].y],_Board[Moving_Input[1].x][Moving_Input[1].y]);
				_Board[Moving_Input[2].x][Moving_Input[2].y] = Ground;

				Make_UCICoordinate(Moving_Input[1], Moving_Input[0]);
				_BeforeMove.Piece = B_Pawn;
				_BeforeMove.position = Moving_Input[0];
			}
			else if (_TValue3 == B_Pawn) {
				_V_SWAP(_Board[Moving_Input[0].x][Moving_Input[0].y],_Board[Moving_Input[2].x][Moving_Input[2].y]);
				_Board[Moving_Input[1].x][Moving_Input[1].y] = Ground;

				Make_UCICoordinate(Moving_Input[2], Moving_Input[0]);
				_BeforeMove.Piece = B_Pawn;
				_BeforeMove.position = Moving_Input[0];
			}
		}
		else if (_TValue2 == Ground && Moving_Input[1].y + 1 == _BeforeMove_Pawn.x) {
			if (_TValue1 == B_Pawn) {
				_V_SWAP(_Board[Moving_Input[1].x][Moving_Input[1].y],_Board[Moving_Input[0].x][Moving_Input[0].y]);
				_Board[Moving_Input[2].x][Moving_Input[2].y] = Ground;

				Make_UCICoordinate(Moving_Input[0], Moving_Input[1]);
				_BeforeMove.Piece = B_Pawn;
				_BeforeMove.position = Moving_Input[1];
			}
			else if (_TValue3 == B_Pawn) {
				_V_SWAP(_Board[Moving_Input[1].x][Moving_Input[1].y],_Board[Moving_Input[2].x][Moving_Input[2].y]);
				_Board[Moving_Input[0].x][Moving_Input[0].y] = Ground;

				Make_UCICoordinate(Moving_Input[2], Moving_Input[1]);
				_BeforeMove.Piece = B_Pawn;
				_BeforeMove.position = Moving_Input[1];
			}
		}
		else if (_TValue3 == Ground && Moving_Input[2].y + 1 == _BeforeMove_Pawn.x) {
			if (_TValue1 == B_Pawn) {
				_V_SWAP(_Board[Moving_Input[2].x][Moving_Input[2].y],_Board[Moving_Input[0].x][Moving_Input[0].y]);
				_Board[Moving_Input[1].x][Moving_Input[1].y] = Ground;

				Make_UCICoordinate(Moving_Input[0], Moving_Input[2]);
				_BeforeMove.Piece = B_Pawn;
				_BeforeMove.position = Moving_Input[2];
			}
			else if (_TValue2 == B_Pawn) {
				_V_SWAP(_Board[Moving_Input[2].x][Moving_Input[2].y],_Board[Moving_Input[1].x][Moving_Input[1].y]);
				_Board[Moving_Input[0].x][Moving_Input[0].y] = Ground;

				Make_UCICoordinate(Moving_Input[1], Moving_Input[2]);
				_BeforeMove.Piece = B_Pawn;
				_BeforeMove.position = Moving_Input[2];
			}
		}
		else {
			Moving_Default(Moving_Input);
			return;
		}
	}

	strcpy(_RecentMove, _TempMove._Movement);
	_Chess_Movement.push(_TempMove);
}

void ChessGame::Moving_Default(CvPoint Moving_Input[]) {
	int _TValue1, _TValue2;

	memset(_TempMove._Movement, NULL, sizeof(_TempMove._Movement));
	_TValue1 = _Board[Moving_Input[0].x][Moving_Input[0].y];
	_TValue2 = _Board[Moving_Input[1].x][Moving_Input[1].y];

	// 이전 움직임 초기화
	_BeforeMove.position = cvPoint(-1,-1);
	_BeforeMove.Piece = -1;
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

				Make_UCICoordinate(Moving_Input[0], Moving_Input[1]);
				_BeforeMove.Piece = _TValue1;
				_BeforeMove.position = Moving_Input[1];
			}
			else {
				printf("error! return (%d, %d) -> (%d, %d)", Moving_Input[1].x, Moving_Input[1].y, Moving_Input[0].x, Moving_Input[0].y);
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
				_BeforeMove.Piece = _TValue2;
				_BeforeMove.position = Moving_Input[0];
			}
			else {
				printf("error! return (%d, %d) -> (%d, %d)", Moving_Input[0].x, Moving_Input[0].y, Moving_Input[1].x, Moving_Input[1].y);
				return;
			}
		}
		else {
			//자신의 턴이 아닐때 체스말을 움직임.
			strcpy(_TempMove._Movement, "Invalid");
			// 턴을 안옮기기 위해서 반전시킴
			_Turn = !_Turn;
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
				_BeforeMove.Piece = _TValue1;
				_BeforeMove.position = Moving_Input[1];
			}
			else {
				printf("error! return (%d, %d) -> (%d, %d)", Moving_Input[1].x, Moving_Input[1].y, Moving_Input[0].x, Moving_Input[0].y);
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
				_BeforeMove.Piece = _TValue2;
				_BeforeMove.position = Moving_Input[0];
			}
			else {
				printf("error! return (%d, %d) -> (%d, %d)", Moving_Input[0].x, Moving_Input[0].y, Moving_Input[1].x, Moving_Input[1].y);
				return;
			}
		}
		else {
			//자신의 턴이 아닐때 체스말을 움직임.
			strcpy(_TempMove._Movement, "Invalid");
			// 턴을 안옮기기 위해서 반전시킴
			_Turn = !_Turn;
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
#pragma endregion Private Functions

#pragma region Public Functions
bool ChessGame::Chess_process(CvPoint Input[], int MOVE_MODE) {
	CvPoint _TMove[4];

	switch(MOVE_MODE){
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

	//
	_Turn = !_Turn;

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
	char temp_buf[32];
	IplImage *tempgame_board;
	tempgame_board = cvCloneImage(_ChessBoard_Image);

	for (register int i = 0; i < 8; i++) {
		for (register int j = 0; j < 8; j++) {
			if(_Board[i][j] != 0){
				sprintf(temp_buf, "./Chess_UI/%d.png", _Board[i][j]);
				_ChessPiece = cvLoadImage(temp_buf, CV_LOAD_IMAGE_UNCHANGED);

				//cvSetImageROI(chessboard_img, cvRect(j*64, i*64, 64, 64));
				// 알파값 추가 처리.
				for (register int k = 0; k < _ChessPiece->width; k++){
					for (register int l = 0; l < _ChessPiece->height; l++){
						unsigned char B = (unsigned char)_ChessPiece->imageData[(k*4) + (l*_ChessPiece->widthStep) + 0];
						unsigned char G = (unsigned char)_ChessPiece->imageData[(k*4) + (l*_ChessPiece->widthStep) + 1];
						unsigned char R = (unsigned char)_ChessPiece->imageData[(k*4) + (l*_ChessPiece->widthStep) + 2];
						unsigned char A = (unsigned char)_ChessPiece->imageData[(k*4) + (l*_ChessPiece->widthStep) + 3];

						// 아이콘이 불투명한 영역만 붙여 넣기.
						if(A > 100){
							tempgame_board->imageData[(j*64+k)*4 + (i*64+l)*tempgame_board->widthStep + 0] = B;
							tempgame_board->imageData[(j*64+k)*4 + (i*64+l)*tempgame_board->widthStep + 1] = G;
							tempgame_board->imageData[(j*64+k)*4 + (i*64+l)*tempgame_board->widthStep + 2] = R;
						}
					}
				}

				//cvResetImageROI(chessboard_img);

				cvReleaseImage(&_ChessPiece);
			}
		}
	}
#if !defined(USING_QT)
	cvShowImage("ChessGame", tempgame_board);
#endif
	cvReleaseImage(&tempgame_board);
}

string ChessGame::Get_RecentMove() {
	return string(_RecentMove);
}

int ChessGame::Read_Mode() {
	static int W_King_MOVED = false;
	static int B_King_MOVED = false;
	static int W_Rook_MOVED = false;
	static int B_Rook_MOVED = false;

	// 이전 움직임 좌표를 확인하여 flag를 지움
	if (_BeforeMove.Piece == W_King)
		W_King_MOVED = true;
	else if (_BeforeMove.Piece == B_King)
		B_King_MOVED = true;
	else if (_BeforeMove.Piece == W_Rook)
		W_Rook_MOVED = true;
	else if (_BeforeMove.Piece == B_Rook)
		B_Rook_MOVED = true;

	if (_BeforeMove.position.x == -1 || _BeforeMove.position.y == -1) {
		return DEFAULT_MOVE;
	}
	else {
		// 앙파상 & 캐슬링 판정부
		if (_Turn == WHITE_TURN) {
			if (W_King_MOVED == false && W_Rook_MOVED == false) {
				if ((_Board[1][0] == Ground && _Board[2][0] == Ground && _Board[3][0] == Ground) || (_Board[5][0] == Ground && _Board[6][0] == Ground))
					return CASTLING_MOVE;
			}
			
			if (_BeforeMove.Piece == B_Pawn)
				return ENPASSANT_MOVE;
			else
				return DEFAULT_MOVE;
		}
		else if (_Turn == BLACK_TURN) {
			if (B_King_MOVED == false && B_Rook_MOVED == false) {
				if ((_Board[1][7] == Ground && _Board[2][7] == Ground && _Board[3][7] == Ground) || (_Board[5][7] == Ground && _Board[6][7] == Ground))
					return CASTLING_MOVE;
			}
			
			if (_BeforeMove.Piece == W_Pawn) {
				return ENPASSANT_MOVE;
			}
			else
				return DEFAULT_MOVE;
		}
	}
}
#pragma endregion Public Functions