#ifndef SHOWPROJECTOR_H
#define SHOWPROJECTOR_H

#include "CVEODependent.hpp"
#include "../CVES/ChessGame.hpp"

#include <stdio.h>
#include <list>

//#define WHITE_TURN 1
//#define BLACK_TURN 0

//enum {			//체스말
//    Ground,
//    W_King, W_Queen, W_Rook, W_Bishop, W_Knight, W_Pawn,
//    B_King, B_Queen, B_Rook, B_Bishop, B_Knight, B_Pawn,
//};

class ShowProjector
{
private:
    //bool Turn = WHITE_TURN;
    bool _Turn = BLACK_TURN;

    CvPoint _position[4];
    int _count;
    bool _complete;
    int _Board[8][8];

    list<CvPoint> _before_ChessSquarePoint;

    CvPoint2D32f _srcTri[4], _dstTri[4];

    IplImage *white_board;
    IplImage *draw_board;
    IplImage *Chess_RGB;
    IplImage *tempgame_board;
    CvMat* warp_mat;
    static void mymouse(int event, int x, int y, int flags, void* param);
    void SettingChessBoard();
    void ShowChessImage();
    void ImageRotate(IplImage *src, bool turn);
    void findDisplayCalcultionPoint(CvPoint t_move[4]);
    void UpdateBoard(int up_Board[8][8]);

public:
    ShowProjector();
    ~ShowProjector();

    void ShowProjector_process();
};

#endif // SHOWPROJECTOR_H
