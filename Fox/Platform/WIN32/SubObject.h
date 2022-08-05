#pragma once 

namespace Fox { 

	namespace Platform {
	
		namespace Win32 {
			
			class FOX_API SubObject {
			public:
				SubObject(std::wstring className, std::wstring title, HICON hicon);
				~SubObject();

				virtual VOID RegisterNewClass();
				virtual VOID Initialize() = 0; 

				HWND GetHandle() {
					return handle;
				}

				VOID Handle(HWND hwnd) { handle = hwnd; }

				static LRESULT CALLBACK SetupMessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
				static LRESULT AssignMessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

				virtual LRESULT MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


			protected: 
				std::wstring className;
				std::wstring title;
				HICON hIcon;
				HWND handle;
			};
		}
	}
}