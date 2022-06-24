#include "Fox.h"
#include "CmdLineArgs.h"

#include <algorithm>

namespace Fox {

	namespace Common {
	
		VOID ReadCommandLineArguments() {
			INT argc = 0;
			LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

			for (INT i = 1; i < argc; i++) {
				std::wstring arg = argv[i];
				if (arg[0] == '-') {
					arg.erase(0, 1);
					std::transform(arg.begin(), arg.end(), arg.begin(), ::tolower);
					ReadCommandLineArgument(arg.c_str());
				}
			}
		}

		VOID ReadCommandLineArgument(CONST WCHAR* argument) {
			if (wcscmp(argument, L"editor") == 0) {
				Fox::Engine::SetMode(Fox::Engine::EngineMode::EDITOR);
			}
			if (wcscmp(argument, L"debug") == 0) {
				Fox::Engine::SetMode(Fox::Engine::EngineMode::DEBUG);
			}
			if (wcscmp(argument, L"server") == 0) {
				Fox::Engine::SetMode(Fox::Engine::EngineMode::SERVER);
			}
		}

	}

}