#pragma once 

namespace Fox {

	namespace Platform {

		namespace Win32 {

			class FOX_API Window : public Fox::Platform::Win32::SubObject {
			public:
				Window(std::wstring title, HICON hicon, WindowType windowType = RESIZABLE);
				~Window();

				virtual VOID Initialize() override;

				HWND GetHandle() {
					return handle;
				}

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

				HWND handle;
			};
		}
	}
}