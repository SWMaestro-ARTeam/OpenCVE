#include "showprojector.h"

#include <QImage>              //QImage를 사용하기 위한 라이브러리
#include <QPixmap>             //QPixmap를 사용하기 위한 라이브러리
#include <QLabel>              //QLabel를 사용하기 위한 라이브러리
#include <QApplication>

ShowProjector *G_ShowProjector;

ShowProjector::ShowProjector(/*list<CvPoint> _ChessDisplayCalcultionPoint8*/){

    // 영상 초기화
    white_board = cvCreateImage(cvSize(1600,800), IPL_DEPTH_8U, 3);
    draw_board = cvCloneImage(white_board);
    Chess_RGB = cvCreateImage(cvSize(512,512), IPL_DEPTH_8U, 3);
    tempgame_board = cvCreateImage(cvSize(512,512), IPL_DEPTH_8U, 4);
    warp_mat = cvCreateMat(3, 3, CV_32FC1);

    // Window 생성(전체화면)
    cvNamedWindow("CVEO_test", CV_WINDOW_NORMAL);
    cvSetWindowProperty("CVEO_test", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);

    G_ShowProjector = this;

    _complete = false;
    _count = 0;
}

ShowProjector::~ShowProjector(){

    // 객체 소멸시 모든 이미지 Release
    cvReleaseImage(&white_board);
    cvReleaseImage(&draw_board);
    cvReleaseImage(&tempgame_board);
    cvReleaseImage(&Chess_RGB);
    G_ShowProjector = NULL;
}

void ShowProjector::mymouse(int event, int x, int y, int flags, void* param){

    switch(event){
    case CV_EVENT_LBUTTONDOWN:
        if(G_ShowProjector->_complete == false){

            // 마우스가 클릭되면 현재 클릭된 위치를 position에 저장
            G_ShowProjector->_position[G_ShowProjector->_count] = cvPoint(x,y);
            printf("click x : %d, y :%d\n", G_ShowProjector->_position[G_ShowProjector->_count].x, G_ShowProjector->_position[G_ShowProjector->_count].y);
            G_ShowProjector->_count++;

            // 입력 받은 마우스의 point가 4개가 되면 투영변환을 위해 _dstTri에 입력받은 정점들을 입력해준다
            if(G_ShowProjector->_count == 4){
                G_ShowProjector->_complete = true;

                G_ShowProjector->_srcTri[0].x = 0;              // X1
                G_ShowProjector->_srcTri[0].y = 0;
                G_ShowProjector->_srcTri[1].x = G_ShowProjector->tempgame_board->width-1;    // Y1
                G_ShowProjector->_srcTri[1].y = 0;
                G_ShowProjector->_srcTri[2].x = 0;              // Z1
                G_ShowProjector->_srcTri[2].y = G_ShowProjector->tempgame_board->height - 1;
                G_ShowProjector->_srcTri[3].x = G_ShowProjector->tempgame_board->width-1;
                G_ShowProjector->_srcTri[3].y = G_ShowProjector->tempgame_board->height - 1;

                G_ShowProjector->_dstTri[0].x = G_ShowProjector->_position[0].x;
                G_ShowProjector->_dstTri[0].y = G_ShowProjector->_position[0].y;
                G_ShowProjector->_dstTri[1].x = G_ShowProjector->_position[1].x;   // Y2
                G_ShowProjector->_dstTri[1].y = G_ShowProjector->_position[1].y;
                G_ShowProjector->_dstTri[2].x = G_ShowProjector->_position[2].x;   // Z2
                G_ShowProjector->_dstTri[2].y = G_ShowProjector->_position[2].y;
                G_ShowProjector->_dstTri[3].x = G_ShowProjector->_position[3].x;   // Z2
                G_ShowProjector->_dstTri[3].y = G_ShowProjector->_position[3].y;


                // 투영변환 함수
                cvGetPerspectiveTransform(G_ShowProjector->_srcTri, G_ShowProjector->_dstTri, G_ShowProjector->warp_mat);

            }
        }
        break;
    }
}

void ShowProjector::SettingChessBoard(){
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
}

void ShowProjector::ShowChessImage(){

    QString qs = QApplication::applicationDirPath();

    const char *str;
    QByteArray ba = qs.toLatin1();
    str = ba.data();
    //printf("the string path will be:%s\n", str);

    // 해당 이미지의 버퍼를 저장할 임시변수
    char temp_buf[360];
    memset(temp_buf, NULL, sizeof(temp_buf));

    for (register int i = 0; i < 8; i++) {
        for (register int j = 0; j < 8; j++) {
            // White turn일 때는 흰색말에 해당 할 때만, Black turn일 떄에는 검은색말에 해당 할 때만 이미지를 그려준다
            if((_Turn == WHITE_TURN && _Board[i][j] >= W_King && _Board[i][j] <= W_Pawn) || (_Turn == BLACK_TURN && _Board[i][j] >= B_King && _Board[i][j] <= B_Pawn)){
                if(_Board[i][j] != 0){
                    sprintf(temp_buf, "%s/%d.png", str,_Board[i][j]);
                    IplImage *Chess_Piece = cvLoadImage(temp_buf, CV_LOAD_IMAGE_UNCHANGED);

                    if(Chess_Piece != NULL){
                        ImageRotate(Chess_Piece, _Turn);
                        //cvSetImageROI(chessboard_img, cvRect(j*64, i*64, 64, 64));
                        // 알파값 추가 처리.
                        for (register int k = 0; k < Chess_Piece->width; k++){
                            for (register int l = 0; l < Chess_Piece->height; l++){
                                unsigned char B = (unsigned char)Chess_Piece->imageData[(k*4) + (l*Chess_Piece->widthStep) + 0];
                                unsigned char G = (unsigned char)Chess_Piece->imageData[(k*4) + (l*Chess_Piece->widthStep) + 1];
                                unsigned char R = (unsigned char)Chess_Piece->imageData[(k*4) + (l*Chess_Piece->widthStep) + 2];
                                unsigned char A = (unsigned char)Chess_Piece->imageData[(k*4) + (l*Chess_Piece->widthStep) + 3];


                                // 알파값이 0인 부분은 처리를 해줄 필요가 없으므로 건너뛴다
                                if(A > 100){
                                    tempgame_board->imageData[((j*64)+k)*4 + (i*64+l)*tempgame_board->widthStep + 0] = B;
                                    tempgame_board->imageData[((j*64)+k)*4 + (i*64+l)*tempgame_board->widthStep + 1] = G;
                                    tempgame_board->imageData[((j*64)+k)*4 + (i*64+l)*tempgame_board->widthStep + 2] = R;
                                }
                            }
                        }
                    }
                    cvReleaseImage(&Chess_Piece);
                }
            }
        }
    }
}

void ShowProjector::ImageRotate(IplImage *src, bool turn){

    double angle;
    double scale  = 1.0;
    CvPoint2D32f Image_center = cvPoint2D32f( src->width/2.0, src->height/2.0);
    CvMat *rotatie_mat = cvCreateMat( 2, 3, CV_32FC1);

    // 흰색 턴일땐 오른쪽으로 검은색 턴일땐 왼쪽으로 회전해준다
    if(turn)
        angle = 90.0;
    else
        angle = -90.0;

    // Image Rotation 함수
    cv2DRotationMatrix( Image_center, angle, scale, rotatie_mat);
    cvWarpAffine( src, src, rotatie_mat, CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS);
}

void ShowProjector::ShowProjector_process(/*int up_Board[8][8]*/){

    cvSet(white_board, cvScalarAll(255));
    cvSet(tempgame_board, cvScalarAll(0));

    cvSetMouseCallback("CVEO_test", mymouse);

    SettingChessBoard();


    // Chess Process가 돌땐 while문 해제
    while(1){
        cvCopy(white_board, draw_board);

        // 정점 4개가 다 받아지지 않았을 경우에는 처리를 하지 않는다
        if(!_complete){
            for(int i = 0; i < _count; i++)
                cvDrawCircle(draw_board, _position[i], 5, cvScalar(0,0,255), -1);
        }
        // 정점이 4개 모두 받아 졌을 경우 처리를 시작한다
        else if(_complete){
            cvCvtColor(tempgame_board,  Chess_RGB, CV_BGRA2BGR);

            // _Board에 저장된 현재 상황을 표기해 주기 위해 이미지를 그려준다
            ShowChessImage();

            //UpdateBoard(up_Board[8][8]);

            // 투영변환한 행렬값을 이용해 draw_board에 Chess_RGB에 그려져있는 UI를 변환한다
            cvWarpPerspective(Chess_RGB, draw_board, warp_mat);
            for(int i = 0; i < _count; i++)
                cvDrawCircle(draw_board, _position[i], 5, cvScalar(0,0,255), -1);
        }

        cvShowImage("CVEO_test", draw_board);

        if(cvWaitKey(10) == 27)
            break;
   }
}

void ShowProjector::UpdateBoard(int up_Board[8][8]){

    // 입력받은 현재 Board상황을 업데이트 해준
    for(int i=0;i<8;i++){
        for(int j=0;j<9;j++){
            _Board[i][j] = up_Board[i][j];
        }
    }
}
