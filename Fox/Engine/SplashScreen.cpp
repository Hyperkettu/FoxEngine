#include "Fox.h"
#include "SplashScreen.h"

#include "Fox/Platform/Win32/Win32Utils.h"

namespace Fox {

	namespace Engine {

		namespace SplashScreen {

			#define WM_OUTPUTMESSAGE (WM_USER + 0x0001)

			SplashWindow* splashWindow;

			VOID Open() {
				if (splashWindow != nullptr) {
					return;
				}

				splashWindow = new SplashWindow();
			}
			
			VOID Close() {
			
			}
			
			VOID SetMessage(const WCHAR* message) {
				PostMessage(splashWindow->GetHandle(), WM_OUTPUTMESSAGE, reinterpret_cast<WPARAM>(message), 0);
			}


			SplashWindow::SplashWindow() 
				: Fox::Platform::Win32::Window(L"SplashScreen", L"SplashScreen", NULL, 500, 560) {
				wcscpy_s(outputMessage, L"SplashScreen Starting...");
				Fox::Platform::Win32::Window::RegisterNewClass();
				Fox::Platform::Win32::Window::Initialize();
			}

			SplashWindow::~SplashWindow() {}

			LRESULT SplashWindow::MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

				switch (message) {

				case WM_PAINT: {

						HBITMAP hBitmap;
						HDC hDC, hMemDC;
						PAINTSTRUCT paintStruct;

						hDC = BeginPaint(hWnd, &paintStruct);

						Fox::Platform::Win32::Utils::AddBitmap(GameSettings::GetSplashURL(), hDC);

						SetBkMode(hDC, TRANSPARENT);
						SetTextColor(hDC, RGB(255, 255, 255));

						if (Fox::Engine::GetMode() != Fox::Engine::EngineMode::RELEASE) {
							std::wstring modeText = Fox::Engine::EngineModeToString() + L" Mode";
							SetTextAlign(hDC, TA_RIGHT);
							TextOut(hDC, width - 15, 15, modeText.c_str(), wcslen(modeText.c_str()));
						}


						SetTextAlign(hDC, TA_CENTER);
						TextOut(hDC, width / 2, height - 30, outputMessage, wcslen(outputMessage));
						EndPaint(hWnd, &paintStruct);
					}
						break;

					case WM_OUTPUTMESSAGE: {
						WCHAR* msg = reinterpret_cast<WCHAR*>(wParam);
						wcscpy_s(outputMessage, msg);
						RedrawWindow(hWnd, nullptr, nullptr, RDW_INVALIDATE);
						return 0;
					}
				}
				
				return CommonMessageHandler(hWnd, message, wParam, lParam);
			}
		}
	}
}