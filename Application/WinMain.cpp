#include "pch.h"

#include <string>

#define MAX_STRING_NAME 256
#define GetHInstance() GetModuleHandle(nullptr) 

WCHAR windowClassName[MAX_STRING_NAME];
WCHAR windowTitle[MAX_STRING_NAME];

INT windowWidth;
INT windowHeight;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPreviousInstance, LPSTR lpCmdLine, int nCmdShow) {

	// init globals 
	wcscpy_s(windowClassName, TEXT("DirectXWindowClass"));
	wcscpy_s(windowTitle, TEXT("DirectX 12 Game"));

	windowWidth = 1280;
	windowHeight = 800;

	// create window
	WNDCLASSEX window;
	window.cbSize = sizeof(WNDCLASSEX);
	window.style = CS_VREDRAW | CS_HREDRAW;
	window.cbClsExtra = 0;
	window.cbWndExtra = 0;

	window.hCursor = LoadCursor(nullptr, IDC_ARROW);
	window.hbrBackground = static_cast<HBRUSH> (GetStockObject(NULL_BRUSH));

	window.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	window.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

	window.lpszClassName = windowClassName;
	window.lpszMenuName = nullptr;

	window.hInstance = hInstance;

	window.lpfnWndProc = DefWindowProc;

	RegisterClassEx(&window);

	// display window
	HWND hWnd = CreateWindow(windowClassName, windowTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, windowWidth, windowHeight, nullptr, nullptr, GetHInstance(), nullptr);

	if (!hWnd) {
		MessageBox(nullptr, L"Failed to create window.", 0, 0);
		return 0;
	}

	ShowWindow(hWnd, SW_SHOW);

	//  message listening loop

	MSG msg = { 0 };
	UINT prev = 0;


	while (msg.message != WM_QUIT) {

		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}