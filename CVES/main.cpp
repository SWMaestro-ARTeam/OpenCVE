#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include <Windows.h>
#include <time.h>

#include "Hand_recognition.hpp"
#include "Chess_recognition.hpp"
#include "BlobLabeling.hpp"
#include "chess_game.hpp"
#include "Img_Process.hpp"

#define WIDTH 640
#define HEIGHT 480

int main(){

	Img_Process IP;
	IP.Init_process();
	while(1){
		IP.Do_imgprocess();

		if(IP.Check_Exit())	break;
	}

	IP.Exit_imgProcess();

	return 0;
}