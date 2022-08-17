#include "Fox.h"
#include "Window.h"

#define DCX_USESSTYLE 0x00010000

namespace Fox {

	namespace Platform {

		namespace Win32 {

			Window::Window(std::wstring title, HICON hicon, WindowType windowType) :
				Fox::Platform::Win32::SubObject(title, title, hicon), type(windowType), isFullscreen(false) {
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
					case WM_NCLBUTTONDOWN: { OnButtonClick(); } break;
					case WM_NCLBUTTONDBLCLK: { Fox::Platform::Win32::Utils::MaximizeWindow(handle); } return FALSE;
					case WM_GETMINMAXINFO: { OnGetWindowMinMaxInfo(reinterpret_cast<MINMAXINFO*>(lParam)); } return FALSE;
					case WM_EXITSIZEMOVE: { OnExitSizeMove(); } break;
					case WM_PAINT: { OnPaint(); } break;
					case WM_TIMER: { RedrawWindow(); } break;
					case WM_KEYDOWN: { OnKeyDown(static_cast<UINT8>(wParam)); } break;
					case WM_KEYUP: { OnKeyUp(static_cast<UINT8>(wParam)); } break;
					case WM_MOUSEMOVE:
					///	if (pSample && static_cast<UINT8>(wParam) == MK_LBUTTON)
						{
							UINT x = LOWORD(lParam);
							UINT y = HIWORD(lParam);
							OnMouseMoved(x, y);
						}
						return FALSE;
					case WM_MOVE: { 
						UINT x = static_cast<UINT>(LOWORD(lParam));
						UINT y = static_cast<UINT>(HIWORD(lParam));
						OnWindowMoved(x, y);
					} return FALSE;
					case WM_LBUTTONDOWN:
					{
						UINT x = LOWORD(lParam);
						UINT y = HIWORD(lParam);
						OnLeftMouseButtonDown(x, y);
					}
					return FALSE;

					case WM_LBUTTONUP:
					{
						UINT x = LOWORD(lParam);
						UINT y = HIWORD(lParam);
						OnLeftMouseButtonUp(x, y);
					}
					return FALSE;
					case WM_RBUTTONDOWN:
					{
						UINT x = LOWORD(lParam);
						UINT y = HIWORD(lParam);
						OnRightMouseButtonDown(x, y);
					}
					return FALSE;

					case WM_RBUTTONUP:
					{
						UINT x = LOWORD(lParam);
						UINT y = HIWORD(lParam);
						OnRightMouseButtonUp(x, y);
					}
					return FALSE;
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

				RECT fillRect = RECT{ 0, 0, w, h };

				FillRect(hDC, &fillRect, brush);
				DeleteObject(brush);

				if (isActive && !Fox::Platform::Win32::Utils::IsWindowFullscreen(handle)) {
					brush = CreateSolidBrush(RGB(255, 255, 255));
					FrameRect(hDC, &fillRect, brush);
					DeleteObject(brush);
				}

				PaintCaption(hDC);

				BitBlt(hDC, 0, 0, w, h, hDC, 0, 0, SRCCOPY);
				SelectObject(hDC, handleOld);
				DeleteObject(hBitmap);

				ReleaseDC(handle, hDC);
			}

			VOID Window::OnNonClientCreate() {
				SetTimer(handle, 1, 100, NULL);
				SetWindowTheme(handle, L"", L"");

				WindowCaption::AddCaptionButton(new CaptionButton(L"X", CaptionButton::Command::CLOSE));
				WindowCaption::AddCaptionButton(new CaptionButton(L"🗖", CaptionButton::Command::MAXIMIZE));
				WindowCaption::AddCaptionButton(new CaptionButton(L"_", CaptionButton::Command::MINIMIZE));
			}

			VOID Window::RedrawWindow() {
				SetWindowPos(handle, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_DRAWFRAME | SWP_FRAMECHANGED); // reset window
				SendMessage(handle, WM_PAINT, 0, 0);
			}

			VOID Window::OnNonClientActivate(BOOL active) {
				isActive = active;
			}

			VOID Window::PaintCaption(HDC hDC) {
				RECT rect;
				GetWindowRect(handle, &rect);

				INT w = rect.right - rect.left;
				INT h = rect.bottom - rect.top;

				if (ShowTitle()) {
					rect = { 0, 0, w, 30 };

					SetBkMode(hDC, TRANSPARENT);
					SetTextColor(hDC, isActive ? RGB(255, 255, 255) : RGB(92, 92, 92));

					DrawText(hDC, title.c_str(), wcslen(title.c_str()), &rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
				}

				int spacing = 0;

				POINT mousePosition;
				GetCursorPos(&mousePosition);
				GetWindowRect(handle, &rect);

				mousePosition.x -= rect.left;
				mousePosition.y -= rect.top;

				for (auto& button : WindowCaption::CaptionButtons()) {
					spacing += button->width;

					button->rect = { w - spacing, 0, w - spacing + button->width, 30 };

					if (mousePosition.x > button->rect.left && mousePosition.x < button->rect.right &&
						mousePosition.y > button->rect.top && mousePosition.y < button->rect.bottom) {
						HBRUSH brush = CreateSolidBrush(RGB(92, 92, 92));
						FillRect(hDC, &button->rect, brush);
						DeleteObject(brush);

					}

					if (button->text.compare(L"🗖") == 0 && Fox::Platform::Win32::Utils::IsWindowFullscreen(handle)) {
						button->text = L"⧉";
					} else if (button->text.compare(L"⧉") == 0 && !Fox::Platform::Win32::Utils::IsWindowFullscreen(handle)) {
						button->text = L"🗖";
					}

					DrawText(hDC, button->text.c_str(), wcslen(button->text.c_str()), &button->rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
				}

			}

			VOID Window::OnButtonClick() {
				RECT rect;
				POINT mousePosition;
				GetCursorPos(&mousePosition);
				GetWindowRect(handle, &rect);

				mousePosition.x -= rect.left;
				mousePosition.y -= rect.top;

				for (auto& button : WindowCaption::CaptionButtons()) {

					if (mousePosition.x > button->rect.left && mousePosition.x < button->rect.right &&
						mousePosition.y > button->rect.top && mousePosition.y < button->rect.bottom) {
						switch (button->command) {
						case CaptionButton::Command::CLOSE: { SendMessage(handle, WM_CLOSE, 0, 0); }  break;
						case CaptionButton::Command::MINIMIZE: { ShowWindow(handle, SW_MINIMIZE); }  break;
						case CaptionButton::Command::MAXIMIZE: { Fox::Platform::Win32::Utils::MaximizeWindow(handle); }  break;
						}
					}
				}
			}

			VOID Window::OnKeyDown(UINT8 keyCode) {
			
			}

			VOID Window::OnKeyUp(UINT8 keyCode) {
			
			}

			VOID Window::OnGetWindowMinMaxInfo(MINMAXINFO* info) {
				RECT workArea;
				SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
				info->ptMaxSize.x = workArea.right - workArea.left;
				info->ptMaxSize.y = workArea.bottom - workArea.top;
				info->ptMaxPosition.x = workArea.left;
				info->ptMaxPosition.y = workArea.top;
				info->ptMinTrackSize.x = 400;
				info->ptMinTrackSize.y = 300;
			}

			VOID Window::OnExitSizeMove() {
				RECT rect;
				GetWindowRect(handle, &rect);
				RECT workArea;
				SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
				if (rect.top < workArea.top + 10 && !Fox::Platform::Win32::Utils::IsWindowFullscreen(handle)) {
					Fox::Platform::Win32::Utils::MaximizeWindow(handle);
				}
			}

			VOID Window::OnPaint() {
				PAINTSTRUCT paintStruct;
				HDC hDC = BeginPaint(handle, &paintStruct);

				RECT rect;
				GetClientRect(handle, &rect);
				HBRUSH brush = CreateSolidBrush(RGB(36, 36, 36));
				FillRect(hDC, &rect, brush);
				DeleteObject(brush);

				EndPaint(handle, &paintStruct);
			}

			VOID Window::ToggleWindowFullscreen() {
				if (isFullscreen) {
					isFullscreen = FALSE;
				} else {
					isFullscreen = TRUE;
				}
			}

			VOID Window::OnResizeWindow(UINT width, UINT height, BOOL minimized) {
				this->width = width;
				this->height = height;
			}

			VOID Window::OnWindowMoved(UINT x, UINT y) {
#ifdef _DEBUG
				Logger::PrintLog(L"Window moved to: %u %u\n", x, y);
#endif
			}

			VOID Window::OnMouseMoved(UINT x, UINT y) {
#ifdef _DEBUG
				Logger::PrintLog(L"Mouse moved to: %u %u\n", x, y);
#endif
			}

			VOID Window::OnLeftMouseButtonDown(UINT x, UINT y) {
#ifdef _DEBUG
				Logger::PrintLog(L"Left mouse button clicked at: %u %u\n", x, y);
#endif	
			}

			VOID Window::OnRightMouseButtonDown(UINT x, UINT y) {
#ifdef _DEBUG
				Logger::PrintLog(L"Right mouse button clicked at: %u %u\n", x, y);
#endif	
			}

			VOID Window::OnLeftMouseButtonUp(UINT x, UINT y) {
#ifdef _DEBUG
				Logger::PrintLog(L"Left mouse button up at: %u %u\n", x, y);
#endif	
			}

			VOID Window::OnRightMouseButtonUp(UINT x, UINT y) {
#ifdef _DEBUG
				Logger::PrintLog(L"Right mouse button up at: %u %u\n", x, y);
#endif	
			}
		}
	}
}