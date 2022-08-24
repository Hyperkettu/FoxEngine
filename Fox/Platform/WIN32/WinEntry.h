#include "Fox/Fox.h"

#include "Fox/Platform/Win32/IApplication.h"
#include "Fox/Common/CmdLineArgs.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPreviousInstance, LPSTR lpCmdLine, int nCmdShow) {

	auto entryApplication = Fox::EntryApplication();

	Fox::GameSettings gameSettings;
	entryApplication->SetupGameSettings();

	Fox::Common::ReadCommandLineArguments();

	Fox::Logger logger;

	entryApplication->PreInitialize();
	entryApplication->Init();

	MSG msg = { 0 };

	while (msg.message != WM_QUIT) {

		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			entryApplication->UpdateInternal();
			entryApplication->Update(); 
		}
	}

	entryApplication->Destroy();

#ifdef _DEBUG
	logger.PrintLog(L"Application exited.\n");
#endif

	return 0;
}