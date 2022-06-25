#pragma once

#include "Fox/Platform/Win32/Window.h"

namespace Fox {

	namespace Engine {

		namespace SplashScreen {

			VOID FOX_API Open();
			VOID FOX_API Close();
			VOID FOX_API SetMessage(const WCHAR* message);

			class FOX_API SplashWindow : public Fox::Platform::Win32::Window {

			public: 
				SplashWindow();
				~SplashWindow();

				LRESULT MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;

			private: 
				WCHAR outputMessage[MAX_STRING_NAME];
			};
		}
	}
}