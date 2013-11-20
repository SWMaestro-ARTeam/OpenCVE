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

#include "Option.hpp"

Option::Option() {
	InitializeOptionValues();
}

Option::~Option() {
	ClearEngineOptionValues();
}

void Option::InitializeOptionValues() {
	// Default Engine Settings.
	_EngineOptionValues.push_back(EngineOptions("Hash", true, "16", "spin", "min 4 max 1024", (const char *)NULL));

	_EngineOptionValues.push_back(EngineOptions("Ponder", true, "false", "check", "", (const char *)NULL));

	_EngineOptionValues.push_back(EngineOptions("Save Notation", true, "Notation.non ", "string", "", (const char *)NULL));
	_EngineOptionValues.push_back(EngineOptions("Auto Recovery", true, "true", "check", "", (const char *)NULL));
	_EngineOptionValues.push_back(EngineOptions("Observer Enable", true, "true", "check", "", (const char *)NULL));
	_EngineOptionValues.push_back(EngineOptions("AI Enable", true, "false", "check", "", (const char *)NULL));
	_EngineOptionValues.push_back(EngineOptions("AR Enable", true, "false", "check", "", (const char *)NULL));
	_EngineOptionValues.push_back(EngineOptions("TicTok", true, "true", "check", "", (const char *)NULL));
	_EngineOptionValues.push_back(EngineOptions("Video Enable", true, "true", "check", "", (const char *)NULL));

	for_IterToEnd(list, EngineOptions, _EngineOptionValues) {
		_TVal->_CurrentVariable = _TVal->_InitializeValue;
	}
}

void Option::ClearEngineOptionValues() {
	_EngineOptionValues.clear();
}

void Option::SetEngineValue(EngineOptions AnyEngineOptions) {
	bool _IsInGameOption = false;

	for_IterToEnd(list, EngineOptions, _EngineOptionValues) {
		if (strcmp(_TVal->_VariableName, AnyEngineOptions._VariableName) == 0) {
			//*_TVal = AnyEngineOptions;

			if (AnyEngineOptions._ControlerType != NULL)
				_TVal->_ControlerType = AnyEngineOptions._ControlerType;
			if (AnyEngineOptions._CurrentVariable != NULL)
				_TVal->_CurrentVariable = AnyEngineOptions._CurrentVariable;
			if (AnyEngineOptions._InitializeValue != NULL)
				_TVal->_InitializeValue = AnyEngineOptions._InitializeValue;
			if (AnyEngineOptions._VariableName != NULL)
				_TVal->_VariableName = AnyEngineOptions._VariableName;
			if (AnyEngineOptions._VariableOptionString != NULL)
				_TVal->_VariableOptionString = AnyEngineOptions._VariableOptionString;
			_TVal->_OptionEnable = AnyEngineOptions._OptionEnable;
			_IsInGameOption = true;
		}		
	}
	
	// EngineOptionValues에 포함이 되어 있지 않다면..
	if (_IsInGameOption == false)
		_EngineOptionValues.push_back(AnyEngineOptions);
}

list<EngineOptions> Option::GetEngineValues() {
	return _EngineOptionValues;
}

bool Option::FindEngineOptionName(const char *VariableName) {
	for_IterToEnd(list, EngineOptions, _EngineOptionValues) {
		if (strcmp(_TVal->_VariableName, VariableName) == 0)
			return true;
	}
	return false;
}

void Option::ReadOptionToINIFile() {

}
