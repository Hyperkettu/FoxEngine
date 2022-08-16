#include "Fox.h"
#include "SubObject.h"

namespace Fox {

	namespace Platform {

		namespace Win32 {
			
			SubObject::SubObject(std::wstring className, std::wstring title, HICON hicon) : 
				className(className), title(title), hIcon(hicon)
			{
			
			}
			
			SubObject::~SubObject() {}

			VOID SubObject::RegisterNewClass() { 
				
				WNDCLASSEX window;
				window.cbSize = sizeof(WNDCLASSEX);
				window.style = CS_VREDRAW | CS_HREDRAW;
				window.cbClsExtra = 0;
				window.cbWndExtra = 0;

				window.hCursor = LoadCursor(nullptr, IDC_ARROW);
				window.hbrBackground = static_cast<HBRUSH> (CreateSolidBrush(RGB(36, 36, 36)));

				window.hIcon = hIcon;
				window.hIconSm = hIcon;

				window.lpszClassName = className.c_str();
				window.lpszMenuName = nullptr;

				window.hInstance = GetHInstance();

				window.lpfnWndProc = SetupMessageHandler;

				RegisterClassEx(&window);
			}

			LRESULT CALLBACK SubObject::SetupMessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

				if (message == WM_NCCREATE) {
				
					const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
					Fox::Platform::Win32::SubObject* const pWnd = static_cast<Fox::Platform::Win32::SubObject*>(pCreate->lpCreateParams);
					SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
					SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Fox::Platform::Win32::SubObject::AssignMessageHandler));
					pWnd->Handle(hWnd);
					PostMessage(hWnd, WM_WINDOW_HANDLE_SET, 0, 0);
					return pWnd->MessageHandler(hWnd, message, wParam, lParam);
				}

				return DefWindowProc(hWnd, message, wParam, lParam);
			}

			LRESULT SubObject::AssignMessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
				Fox::Platform::Win32::SubObject* const pWnd = reinterpret_cast<Fox::Platform::Win32::SubObject*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
				return pWnd->MessageHandler(hWnd, message, wParam, lParam);
			}

			LRESULT SubObject::MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
				return DefWindowProc(hWnd, message, wParam, lParam);
			}

		}
	}
}