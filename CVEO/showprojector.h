#ifndef SHOWPROJECTOR_H
#define SHOWPROJECTOR_H

#include "CVEODependent.hpp"
#include "../CVES/ChessGame.hpp"

#include <stdio.h>
#include <list>

class ShowProjector
{
private:

    // AI의 포지션을 정하는 변수
    //bool Turn = WHITE_TURN;
    bool _Turn = BLACK_TURN;

    // Projector상의 4개의 정점을 입력받는 변수
    CvPoint _position[4];

    // position의 정점 count
    int _count;

    // 점이 4개가 모두 찍혔는지 검사하는 변수
    bool _complete;

    // 현재 체스판의 상황의 정보를 가지고 있는 변수
    int _Board[8][8];

    // 투영변환을 위한 변수
    CvPoint2D32f _srcTri[4], _dstTri[4];

    IplImage *white_board;
    IplImage *draw_board;
    IplImage *Chess_RGB;
    IplImage *tempgame_board;
    CvMat* warp_mat;

    // opencv window상의 마우스 입력을 받는 함수
    static void mymouse(int event, int x, int y, int flags, void* param);
    // _Board 초기화 함수
    void SettingChessBoard();

    // _Board의 정보를 UI상에서 띄워주는 함수
    void ShowChessImage();

    // Projector로 띄워줄 체스말의 위치에 따라 회전해주는 함수
    // Black : 왼쪽 White : 오른쪽
    void ImageRotate(IplImage *src, bool turn);

    // _Board 갱신함수
    void UpdateBoard(int up_Board[8][8]);

public:
    ShowProjector();
    ~ShowProjector();

    void ShowProjector_process();
};

#endif // SHOWPROJECTOR_H
