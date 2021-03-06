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

#include "Time.hpp"

// is local time
char *Time::GetASCTime() {
	// using c++ reference.
	time(&_RawTime);
	TimeInformation = localtime(&_RawTime);
	return asctime(TimeInformation);
}

char *Time::GetCTime() {
	time(&_RawTime);
	// or asctime(localtime(_RawTime));
	return ctime(&_RawTime);
}

char *Time::GetUTCTime() {
	char _TTimeStr[BUFFER_MAX_128];

	memset(_TTimeStr, ZERO_, sizeof(_TTimeStr));
	time(&_RawTime);
	TimeInformation = gmtime(&_RawTime);
	sprintf(_TTimeStr, "%2d:%02d:%02d", (TimeInformation->tm_hour % 24), TimeInformation->tm_min, TimeInformation->tm_sec);

	return _TTimeStr;
}

// '초' 단위 실제 시간이 나온다(Sec).
double Time::GetRealTime() {
	double _TRealTime = 0.0f;
#if defined(WINDOWS_SYS)
	_TRealTime = GetTickCount() / 1000.0f;
#elif defined(POSIX_SYS)
	timeval _TTimeVal[1];
	timezone _TTimeZone[1];

	_TTimeZone->tz_minuteswest = 0;
	_TTimeZone->tz_dsttime = 0;

	if (gettimeofday(_TTimeVal, _TTimeZone) != -1)
		_TRealTime = _TTimeVal->tv_sec + (_TTimeVal->tv_usec * 1E-6);
#endif
	return _TRealTime;
}

double Time::GetNowCPUTimeOfProcessStarted() {
	double _TCPUTime = 0.0f;
	// Process 하나가 실행된 이후에 걸린 총 시간이 대한 시간을 구한다.
	// 유휴 상태(wait 함수, sleep 함수 등)에서는 clock을 쓰면 안된다는 점에 주의.
	// 고로 GetNowCPUTimeOfProcessStarted 함수는, 유휴 상태에서는 쓸 수 없다는 점에 유의해라.
	double _TNowClock = clock();
#if defined(WINDOWS_SYS)
	_TCPUTime = _TNowClock / double(CLOCKS_PER_SEC);
#elif defined(POSIX_SYS)
	rusage _TRusage[1];

	if (getrusage(RUSAGE_SELF, _TRusage) != -1)
		_TCPUTime = (&_TRusage->ru_utime)->tv_sec + ((&_TRusage->ru_utime)->tv_usec * 1E-6);
#endif
	return _TCPUTime;
}

int Time::GetTick() {
	int _TTick;
#if defined(WINDOWS_SYS)
	_TTick = GetTickCount();
#elif defined(POSIX_SYS)
	timeval _TTimeVal[1];
	timezone _TTimeZone[1];

	_TTimeZone->tz_minuteswest = 0;
	_TTimeZone->tz_dsttime = 0;

	if (gettimeofday(_TTimeVal, _TTimeZone) != -1)
		_TTick = (_TTimeVal->tv_usec * 1E-6);
#endif
	return _TTick;
}

bool SystemControlsOfTime::WaitSecondsUntilSwitch(int Seconds, bool &KillSwitch) {
	time_t _TNow = time(NULL);
	time_t _TEnd = _TNow + Seconds;

	while (KillSwitch != true && _TNow < _TEnd) {
		_TNow = time(NULL);
	}

	if (_TNow < _TEnd && KillSwitch == true)
		return false;

	return true;
}


void Timer::SetTimeSeconds(int Seconds) {

}

void Timer::SetTimeMilliSeconds(int MilliSeconds) {

}

void Timer::SetTimeMicroSeconds(int MicroSeconds) {

}

void Timer::Start() {
	_IsStart = true;
	_Thread.StartThread(TimerProcessingThread, this);
	/*
	HANDLE _TThreadHandle = (HANDLE)_beginthreadex(NULL, 0, TimerProcessingThread, this, 0, NULL);
	*/
}

void Timer::End() {
	_IsStart = false;
}

#if defined(WINDOWS_SYS)
UINT WINAPI
	//DWORD WINAPI
#elif defined(POSIX_SYS)
// using pthread
void *
#endif
	Timer::TimerProcessingThread(
#if defined(WINDOWS_SYS)
	LPVOID
#elif defined(POSIX_SYS)
	void * 
#endif 
	Param) {
	Timer *_TTimer = (Timer *)Param;
		
	while (_TTimer->_IsStart) {
		//if ()
	}

#if defined(WINDOWS_SYS)
	_endthread();
#elif defined(POSIX_SYS)

#endif
	return 0;
}
