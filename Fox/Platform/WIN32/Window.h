#pragma once 

namespace Fox {

	namespace Platform {

		namespace Win32 {

			class FOX_API Window : public Fox::Platform::Win32::SubObject {
			public:
				Window(std::wstring className, std::wstring title, HICON hicon, INT width, INT height);
				~Window();

				virtual VOID Initialize() override;

				HWND GetHandle() {
					return handle;
				}

			protected:
				INT width;
				INT height;
				HWND handle;
			};
		}
	}
}