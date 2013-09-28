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
	// Test용 Engine Value.
	// get Value at Fruit.
	_EngineOptionValues.push_back(EO("Hash", true, "16", "spin", "min 4 max 1024", NULL));

	_EngineOptionValues.push_back(EO("Ponder", true, "false", "check", "", NULL));

	_EngineOptionValues.push_back(EO("OwnBook", true, "true", "check",  "", NULL));
	_EngineOptionValues.push_back(EO("BookFile", true, "book_small.bin", "string", "", NULL));

	_EngineOptionValues.push_back(EO("NullMove Pruning", true, "Fail High", "combo", "var Always var Fail High var Never", NULL));
	_EngineOptionValues.push_back(EO("NullMove Reduction", true, "3", "spin", "min 1 max 3", NULL));
	_EngineOptionValues.push_back(EO("Verification Search", true, "Endgame", "combo", "var Always var Endgame var Never", NULL));
	_EngineOptionValues.push_back(EO("Verification Reduction", true, "5", "spin", "min 1 max 6", NULL));

	_EngineOptionValues.push_back(EO("History Pruning", true, "true", "check", "", NULL));
	_EngineOptionValues.push_back(EO("History Threshold", true, "60", "spin", "min 0 max 100", NULL));

	_EngineOptionValues.push_back(EO("Futility Pruning", true, "false", "check", "", NULL));
	_EngineOptionValues.push_back(EO("Futility Margin", true, "100", "spin", "min 0 max 500", NULL));

	_EngineOptionValues.push_back(EO("Delta Pruning", true, "false", "check", "", NULL));
	_EngineOptionValues.push_back(EO("Delta Margin", true, "50", "spin", "min 0 max 500", NULL));

	_EngineOptionValues.push_back(EO("Quiescence Check Plies", true, "1", "spin", "min 0 max 2", NULL));

	_EngineOptionValues.push_back(EO("Material", true, "100", "spin", "min 0 max 400", NULL));
	_EngineOptionValues.push_back(EO("Piece Activity", true, "100", "spin", "min 0 max 400", NULL));
	_EngineOptionValues.push_back(EO("King Safety", true, "100", "spin", "min 0 max 400", NULL));
	_EngineOptionValues.push_back(EO("Pawn Structure", true, "100", "spin", "min 0 max 400", NULL));
	_EngineOptionValues.push_back(EO("Passed Pawns", true, "100", "spin", "min 0 max 400", NULL));

	list<EO>::iterator _TEOIter;
	for (_TEOIter = _EngineOptionValues.begin();
		_TEOIter != _EngineOptionValues.end(); _TEOIter++) {
		_TEOIter->_CurrentVariable = _TEOIter->_InitializeValue;
	}
}

void Option::ClearEngineOptionValues() {
	_EngineOptionValues.clear();
}

void Option::SetEngineValues(EO _EngineOptions) {
	list<EO>::iterator _TEOIter;
	bool _IsInGameOption = false;
	for (_TEOIter = _EngineOptionValues.begin();
		_TEOIter != _EngineOptionValues.end(); _TEOIter++) {
		if (_TEOIter->_VariableName == _EngineOptions._VariableName) {
			*_TEOIter = _EngineOptions;
			_IsInGameOption = true;
		}
	}

	// EngineOptionValues에 포함이 되어 있지 않다면..
	if (_IsInGameOption == false)
		_EngineOptionValues.push_back(_EngineOptions);
}

list<EO> Option::GetEngineValues() {
	return _EngineOptionValues;
}

void Option::ReadOptionToINIFile() {

}
