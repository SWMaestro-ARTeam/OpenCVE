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

#ifndef _HandRecognition_hpp_
#define _HandRecognition_hpp_

#include "Common.hpp"

#if WINDOWS_SYS
#include <Windows.h>
#elif POSIX_SYS
#endif

#include <iostream>

#include <cv.h>

class HandRecognition {
private:
	int img_width, img_height;

	unsigned char B, G, R;
	unsigned char Y, Cr, Cb;
	unsigned char H, S, V;

	bool R1(int R, int G, int B);
	bool R2(float Y, float Cr, float Cb);
	bool R3(float H, float S, float V);

	IplImage *img_YCrCb, *img_HSV; //YCrCb, HSV 색상계
	IplImage *prev_ground, *present_ground; //차영상을 통해 차이를 구할 예정

public:
	HandRecognition();
	~HandRecognition();

	//초기화
	void Init(int width, int height);
	//피부색 Detection
	void Detect_Skin(IplImage *src, IplImage *dst);
	//손검출 판단 - 면적 기반
	bool is_Hand(IplImage *src);
	//이전 프레임과의 차영상
	void Sub_prevFrame(IplImage *src, IplImage *dst, bool first);
	//차영상 초기화
	void Init_diff();
};
#endif