#include "pch.h"

/**
* Global variables
*/

#pragma region WindowVariables

WCHAR windowClassName[MAX_STRING_NAME];
WCHAR windowTitle[MAX_STRING_NAME];

INT windowWidth;
INT windowHeight;

HICON hIcon;

#pragma endregion

/**
* Pre-declarations
*/

#pragma region Pre-Declarations

void initVariables();
void createWindowClass();
void initWindow();
void handleMessages();

LRESULT CALLBACK WindowProcess(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

#pragma endregion

/**
* Windows specific functions
 */

#pragma region WindowsSpecific

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPreviousInstance, LPSTR lpCmdLine, int nCmdShow) {

	// init globals 
	initVariables();

	// create window
	createWindowClass();

	// display window
	initWindow();

	//  message listening loop
	handleMessages();

	return 0;
}

LRESULT CALLBACK WindowProcess(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

#pragma endregion

/*
* Functions
**/

#pragma region Functions

void initVariables() {
	LoadString(GetHInstance(), IDS_WINDOW_CLASS, windowClassName, MAX_STRING_NAME);
	LoadString(GetHInstance(), IDS_GAME_NAME, windowTitle, MAX_STRING_NAME);

	windowWidth = 1280;
	windowHeight = 800;

	hIcon = LoadIcon(GetHInstance(), MAKEINTRESOURCE(IDI_MAINICON));
}

void createWindowClass() {
	WNDCLASSEX window;
	window.cbSize = sizeof(WNDCLASSEX);
	window.style = CS_VREDRAW | CS_HREDRAW;
	window.cbClsExtra = 0;
	window.cbWndExtra = 0;

	window.hCursor = LoadCursor(nullptr, IDC_ARROW);
	window.hbrBackground = static_cast<HBRUSH> (GetStockObject(NULL_BRUSH));

	window.hIcon = hIcon;
	window.hIconSm = hIcon;

	window.lpszClassName = windowClassName;
	window.lpszMenuName = nullptr;

	window.hInstance = GetHInstance();

	window.lpfnWndProc = WindowProcess;

	RegisterClassEx(&window);
}

void initWindow() {
	HWND hWnd = CreateWindow(windowClassName, windowTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, windowWidth, windowHeight, nullptr, nullptr, GetHInstance(), nullptr);

	if (!hWnd) {
		MessageBox(nullptr, L"Failed to create window.", 0, 0);
		PostQuitMessage(0);
	}
	else {
		ShowWindow(hWnd, SW_SHOW);
	}
}

void handleMessages() {
	MSG msg = { 0 };

	while (msg.message != WM_QUIT) {

		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

#pragma endregion