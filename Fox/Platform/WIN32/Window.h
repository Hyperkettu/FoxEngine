#pragma once 

#include <Uxtheme.h>
#pragma comment(lib, "uxtheme.lib")

namespace Fox {

	namespace Platform {

		namespace Win32 {

			class FOX_API Window : public Fox::Platform::Win32::SubObject, public Fox::Platform::Win32::WindowCaption {
			public:
				Window(std::wstring title, HICON hicon, WindowType windowType = RESIZABLE);
				~Window();

				virtual VOID Initialize() override;

				virtual LRESULT MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;

				VOID OnNonClientCreate();
				VOID OnNonClientPaint(HRGN region); 
				VOID OnNonClientActivate(BOOL active);
				virtual VOID OnResizeWindow(UINT width, UINT height, BOOL minimized);
				virtual VOID OnWindowMoved(UINT x, UINT y);

				VOID RedrawWindow();

				VOID PaintCaption(HDC hDC);

				VOID OnButtonClick();
				virtual VOID OnKeyDown(UINT8 keyCode);
				virtual VOID OnKeyUp(UINT8 keyCode);
				virtual VOID OnMouseMoved(UINT x, UINT y);
				virtual VOID OnLeftMouseButtonDown(UINT x, UINT y);
				virtual VOID OnRightMouseButtonDown(UINT x, UINT y);
				virtual VOID OnLeftMouseButtonUp(UINT x, UINT y);
				virtual VOID OnRightMouseButtonUp(UINT x, UINT y);

				VOID OnGetWindowMinMaxInfo(MINMAXINFO* info);
				VOID OnExitSizeMove();

				virtual VOID ToggleWindowFullscreen();

				VOID OnPaint();

				VOID Size(SIZE windowSize) { size = windowSize; width = windowSize.cx; height = windowSize.cy; }
				VOID Size(INT windowWidth, INT windowHeight) { size.cx = windowWidth; size.cy = windowHeight; width = windowWidth; height = windowHeight; }

				SIZE Size() const { return size; }
				INT Width() const { return width; }
				INT Height() const { return height; }

			protected:
				INT width;
				INT height;
				SIZE size;

				WindowType type;
				BOOL isActive;
				BOOL isFullscreen;
			};
		}
	}
}