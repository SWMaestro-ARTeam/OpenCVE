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

#ifndef _CVECDependent_hpp_
#define _CVECDependent_hpp_

typedef struct _EngineOptions {
	const char *_VariableName;
	// Is it Option Viewing?
	bool _OptionEnable;
	const char *_InitializeValue;
	// It's variables for Windows(Winboard)/Linux(xboard)
	const char *_ControlerType;
	const char *_VariableOptionString;
	const char *_CurrentVariable;

	_EngineOptions() {
		EOInitialize();
	}

	_EngineOptions(const char *_VariableName_, bool _OptionEnable_,
		const char *_InitializeValue_, const char *_ControlerType_,
		const char *_VariableOptionString_, const char *_ExtraVariable_) {
			EOInitialize();
			_VariableName = _VariableName_;
			_OptionEnable = _OptionEnable_;
			_InitializeValue = _InitializeValue_;
			_ControlerType = _ControlerType_;
			_VariableOptionString = _VariableOptionString_;
			_CurrentVariable = _ExtraVariable_;
	}

	void EOInitialize() {
		_VariableName = new char[BUFFER_MAX_1024];
		_OptionEnable = false;
		_InitializeValue = new char[BUFFER_MAX_1024];
		_ControlerType = new char[BUFFER_MAX_1024];
		_CurrentVariable = new char[BUFFER_MAX_1024];
	}
} EngineOptions;

#endif