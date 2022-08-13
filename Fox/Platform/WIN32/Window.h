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

				VOID RedrawWindow();

				VOID PaintCaption(HDC hDC);

				VOID OnButtonClick();

				VOID OnGetWindowMinMaxInfo(MINMAXINFO* info);
				VOID OnExitSizeMove();

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
			};
		}
	}
}