#pragma once

#define DEFAULT_WIDTH 1024
#define DEFAULT_HEIGHT 800

namespace Fox {

	namespace Platform {

		namespace Win32 {

			enum WindowType : DWORD {
				STATIC = WS_OVERLAPPED,
				RESIZABLE = WS_SIZEBOX,
				POPUP = WS_POPUP
			};
		
			namespace Utils {
			
				BOOL FOX_API AddBitmap(CONST WCHAR* szFileName, HDC hWinDC, INT x = 0, INT y = 0);
				BOOL FOX_API IsWindowFullscreen(HWND hWnd);
				VOID FOX_API MaximizeWindow(HWND hWnd);
				VOID FOX_API SetWindowZorderToTopMost(HWND windowHandle, BOOL setToTopMost); 
			}
		}
	}
}