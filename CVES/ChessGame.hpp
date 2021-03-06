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

using namespace std;

typedef struct __MoveFormat {
	char *_Movement;
	bool _TurnFlag;
} MoveFormat;

typedef struct __BeforeMove {
	int _Piece;
	CvPoint _Position;
} BeforeMove;

typedef struct __ErrorMove{
	bool _Flag;
	CvPoint _Before, _After;
} ErrorMove;

enum {			//체스말
	Ground, 
	W_King, W_Queen, W_Rook, W_Bishop, W_Knight, W_Pawn,
	B_King, B_Queen, B_Rook, B_Bishop, B_Knight, B_Pawn,
};

class ChessGame {

private:
	int _Board[8][8];
	bool _Turn;
	char _RecentMove[6]; //가장 최근 움직임을 저장

	ErrorMove _error_move;
	CvPoint _Before, _After;
	CvPoint _BeforeMove_Pawn;
	BeforeMove _BeforeMove;
	MoveFormat _TempMove;
	IplImage *_ChessBoard_Image;
	IplImage *_ChessPiece;

	bool _W_PawnMove[8], _B_PawnMove[8];

	FILE *_Movement_Log;		//체스 이동경로 Log를 남길 파일포인터.
	std::queue<MoveFormat> _Chess_Movement;

	void Make_UCICoordinate(CvPoint Before, CvPoint After);
	char Mapping_UCICharacter(int Position); // 글자 좌표로 매핑
	int Mapping_UCIUnCharacter(char Position);	// UCI해석
	// MODE 판독 함수 

	// 각 체스 룰을 다루는 함수
	void Moving_Castling(CvPoint Moving_Input[]);
	void Moving_Enpassant(CvPoint Moving_Input[]);
	void Moving_Default(CvPoint Moving_Input[]);

	bool Rule_DefaultMove(CvPoint before, CvPoint after); // 기본 이동 체스룰.
	bool Check_Return(CvPoint move_input[]);
public:
	ChessGame();
	~ChessGame();

	// chess board의 말 움직임 진행 함수.
	// MOVE_MODE : CASTLING_MOVE - 캐슬링 detect, ENPASSANT_MOVE - 앙파상 detect, other - 두 가지 말의 이동만을 체크함.
	bool Chess_Process(CvPoint input1[], int MOVE_MODE, bool AI_Mode);
	void Show_ChessBoard(); // cmd에 체스판을 그림.
	void Show_ChessImage(); // chess UI를 Draw
	bool Check_InvalidMove(IplImage *Source, vector<ChessPoint> _CP, CvPoint _out[], int ROI_X, int ROI_Y);
	string Get_RecentMove(); // list에 저장된 가장 최근 movement를 return; Dequeue와 같음.
	void Get_ChessBoard(int Board[][8]);
	void Set_ChessBoard(char *UCI_Moves);
	int Read_Mode();
	void Draw_InvalidMove(IplImage *Source, vector<ChessPoint> _CP, int ROI_X, int ROI_Y); // 이미지에 잘못된 움직임을 정정하라는 UI를 그림
	bool Return_errorFlag();		// 최근 움직임이 Invalild move라면 return true;
	void Draw_Arrow(IplImage *img, CvPoint pStart, CvPoint pEnd, int len, int alpha, CvScalar color, int thickness, int lineType); // 체스 화살표 그리기
};
#endif
