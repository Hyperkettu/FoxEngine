#include "Fox/Fox.h"

#include "Fox/Platform/WIN32/IApplication.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPreviousInstance, LPSTR lpCmdLine, int nCmdShow) {

	auto entryApplication = Fox::EntryApplication();
	entryApplication->Init();

	MSG msg = { 0 };

	while (msg.message != WM_QUIT) {

		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			entryApplication->Update(); 
		}
	}

	return 0;
}