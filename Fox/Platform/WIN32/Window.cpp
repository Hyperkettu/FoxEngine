#include "Fox.h"
#include "Window.h"

namespace Fox {

	namespace Platform {

		namespace Win32 {

			Window::Window(std::wstring className, std::wstring title, HICON hicon, INT width, INT height) : 
				Fox::Platform::Win32::SubObject(className, title, hicon), width(width), height(height) {
			}

			Window::~Window() {
			
			}

			VOID Window::Initialize() {
			
				RECT desktopRect;
				const HWND hDesktop = GetDesktopWindow();
				GetWindowRect(hDesktop, &desktopRect);

				RECT rect = { 0, 0, width, height };
				AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
				int w = rect.right - rect.left;
				int h = rect.bottom - rect.top;

				handle = CreateWindow(className.c_str(), title.c_str(), WS_POPUP,
					desktopRect.right / 2 - w / 2, desktopRect.bottom / 2 - h / 2, width, height, nullptr, nullptr, GetHInstance(), static_cast<void*>(this));

				ShowWindow(handle, SW_SHOW);
				UpdateWindow(handle);
			}
		}
	}
}