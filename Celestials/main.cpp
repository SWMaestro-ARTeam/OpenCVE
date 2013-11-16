//////////////////////////////////////////////////////////////////////////////////////////////
//	The OpenCVE Project.
//
//	The MIT License (MIT)
//	Copyright ⓒ 2013 {Doohoon Kim, Sungpil Moon, Kyuhong Choi} at AR Team of SW Maestro 4th
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

#include "SystemDependency.hpp"

//#if MAKE_CELESTIALS == 1
#include "AdapterC.hpp"
#include "AdapterS.hpp"
#include "AdapterO.hpp"
//#include "AdapterA.hpp"

#include <string.h>
#include <string>

void Celestials_Manual() {
	// 여기에 Manual을 만들어야 한다.
	fprintf(stdout, "Hello~!\n");
}

// 통합 Engine 구현(CVEC, CVES, CVEO).
int main(int argc, char *argv[]) {
	int _TApplicationReturnValue = 0;

	//if (argc > 0/*1*/) {
	if (argc > 1) {
		//STRING_SWITCH_BEGIN(string("/Observer"))
		STRING_SWITCH_BEGIN(string((char *)argv[1]))
		{
			CASE("/Server")
				AdapterS _TAdapterS;
				_TApplicationReturnValue = _TAdapterS.Go_EngineS(argc, argv);
				break;
			CASE("/Client")
				AdapterC _TAdapterC;
				_TApplicationReturnValue = _TAdapterC.Go_EngineC();
				break;
			CASE("/Observer")
				AdapterO _TAdapterO;
				_TApplicationReturnValue = _TAdapterO.Go_EngineO(argc, argv);
				break;
			CASE("/AIAdapter")
//				AdapterA _TAdapterA;
//				_TApplicationReturnValue = _TAdapterA.Go_EngineA();
				break;
			CASE("--help")
				Celestials_Manual();
				break;
			DEFAULT()
				fprintf(stdout, "Invaild Command.\n");
		}
		STRING_SWITCH_END()
	}
	else {
		fprintf(stdout, "Using *Celestials --help*\n");
	}
	return _TApplicationReturnValue;
}
//#endif
