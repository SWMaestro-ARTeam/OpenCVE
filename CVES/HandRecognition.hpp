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

#include "CVESDependent.hpp"

#if defined(WINDOWS_SYS)
#include <Windows.h>
#elif defined(POSIX_SYS)
#endif

#include <iostream>

class HandRecognition {
private:
	int _Image_Width, _Image_Height;

	unsigned char _Plane_B_Value, _Plane_G_Value, _Plane_R_Value;
	unsigned char _Plane_Y_Value, _Plane_Cr_Value, _Plane_Cb_Value;
	unsigned char _Plane_H_Value, _Plane_S_Value, _Plane_V_Value;

	IplImage *_Image_YCrCb, *_Image_HSV; // YCrCb, HSV 색상계.
	IplImage *_Image_Previous, *_Image_Now; // 이전 영상, 현재 영상. 차영상을 통해 차이를 구할 예정.

	bool RangeDetectionOfRGBSkinColour(int Value_R, int Value_G, int Value_B);
	bool RangeDetectionOfYCrCbSkinColour(float Value_Y, float Value_Cr, float Value_Cb);
	bool RangeDetectionOfHSVSkinColour(float Value_H, float Value_S, float Value_V);

public:
	HandRecognition();
	~HandRecognition();

	// 초기화.
	void Initialize_HandRecognition(int Width, int Height);
	// 차영상 초기화.
	void Initialize_Differential_Image();
	// 피부색 Detection.
	void Detect_SkinColour(IplImage *Source, IplImage *Destination);
	// 손검출 판단 - 면적 기반.
	bool IsHand(IplImage *Source);
	// 이전 프레임과의 차영상.
	void Sub_prevFrame(IplImage *Source, IplImage *Destination, bool first);
};
#endif