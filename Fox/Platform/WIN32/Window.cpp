#include "Fox.h"
#include "Window.h"

#define DCX_USESSTYLE 0x00010000

namespace Fox {

	namespace Platform {

		namespace Win32 {

			Window::Window(std::wstring title, HICON hicon, WindowType windowType) : 
				Fox::Platform::Win32::SubObject(title, title, hicon), type(windowType) {
				Size(DEFAULT_WIDTH, DEFAULT_HEIGHT);
			}

			Window::~Window() {
			
			}

			VOID Window::Initialize() {
			
				RECT desktopRect;
				const HWND hDesktop = GetDesktopWindow();
				GetWindowRect(hDesktop, &desktopRect);

				RECT rect = { 0, 0, width, height };
				AdjustWindowRect(&rect, type, false);
				int w = rect.right - rect.left;
				int h = rect.bottom - rect.top;

				handle = CreateWindow(className.c_str(), title.c_str(), type,
					desktopRect.right / 2 - width / 2, desktopRect.bottom / 2 - height / 2, width, height, nullptr, nullptr, GetHInstance(), static_cast<void*>(this));

				ShowWindow(handle, SW_SHOW);
				UpdateWindow(handle);
			}

			LRESULT Window::MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
			
				switch (message)
				{
				case WM_NCCREATE: { OnNonClientCreate(); } return TRUE;
				case WM_NCPAINT: { OnNonClientPaint(reinterpret_cast<HRGN>(wParam)); } return FALSE;
				case WM_NCACTIVATE: { OnNonClientActivate(LOWORD(wParam) != WA_INACTIVE); } return TRUE;
				case WM_TIMER: { RedrawWindow(); } break;

				default:
					break;
				}

				return SubObject::MessageHandler(hWnd, message, wParam, lParam);
			}

			VOID Window::OnNonClientPaint(HRGN region) {
				HDC hDC = GetDCEx(handle, region, DCX_WINDOW | DCX_INTERSECTRGN | DCX_USESSTYLE);

				RECT rect;
				GetWindowRect(handle, &rect);

				INT w = rect.right - rect.left;
				INT h = rect.bottom - rect.top;

				HBITMAP hBitmap = CreateCompatibleBitmap(hDC, w, h);
				HANDLE handleOld = SelectObject(hDC, hBitmap);

				HBRUSH brush = CreateSolidBrush(RGB(46, 46, 46));

				RECT fillRect = RECT { 0, 0, w, h };

				FillRect(hDC, &fillRect, brush);
				DeleteObject(brush);

				if (isActive) {
					brush = CreateSolidBrush(RGB(255, 255, 255));
					FrameRect(hDC, &fillRect, brush);
					DeleteObject(brush);
				}

				BitBlt(hDC, 0, 0, w, h, hDC, 0, 0, SRCCOPY);
				SelectObject(hDC, handleOld);
				DeleteObject(hBitmap);

				ReleaseDC(handle, hDC);
			}

			VOID Window::OnNonClientCreate() {
				SetTimer(handle, 1, 100, NULL);
				SetWindowTheme(handle, L"", L"");
			}

			VOID Window::RedrawWindow() {
				SetWindowPos(handle, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_DRAWFRAME | SWP_FRAMECHANGED); // reset window
				SendMessage(handle, WM_PAINT, 0, 0);
			}

			VOID Window::OnNonClientActivate(BOOL active) {
				isActive = active;
			}
		}
	}
}