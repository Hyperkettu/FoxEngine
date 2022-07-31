#include "Fox.h"
#include "Window.h"

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
		}
	}
}