#include "Fox.h"
#include "Win32Utils.h"

namespace Fox {

	namespace Platform {

		namespace Win32 {

			namespace Utils {

				BOOL AddBitmap(CONST WCHAR* szFileName, HDC hWinDC, INT x, INT y) {
					
					BITMAP qBitmap; 
					HDC hLocalDC = CreateCompatibleDC(hWinDC);

					HBITMAP hBitmap = static_cast<HBITMAP>(LoadImage(NULL, szFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));
					if (hBitmap == NULL) {
						MessageBox(NULL, L"LoadImage failed.", L"Error", MB_OK);
						return false;
					}

					GetObject(reinterpret_cast<HGDIOBJ>(hBitmap), sizeof(BITMAP), reinterpret_cast<LPVOID>(&qBitmap));
					SelectObject(hLocalDC, hBitmap);

					if (!BitBlt(hWinDC, x, y, qBitmap.bmWidth, qBitmap.bmHeight, hLocalDC, 0, 0, SRCCOPY)) {
						MessageBox(NULL, L"Blit failed.", L"Error", MB_OK);
						return false;
					}

					::DeleteDC(hLocalDC);
					::DeleteObject(hBitmap);
					
					return true;
				}

				BOOL IsWindowFullscreen(HWND hWnd) {
					WINDOWPLACEMENT placement;
					GetWindowPlacement(hWnd, &placement);
					if (placement.showCmd == SW_MAXIMIZE)
						return TRUE;
					return FALSE;
				}

				VOID MaximizeWindow(HWND hWnd) {
					WINDOWPLACEMENT placement;
					GetWindowPlacement(hWnd, &placement);
					if (placement.showCmd == SW_MAXIMIZE) {
						ShowWindow(hWnd, SW_NORMAL);
					} else {
						ShowWindow(hWnd, SW_MAXIMIZE);
					}
				}

				VOID SetWindowZorderToTopMost(HWND hWnd, BOOL setToTopMost) {
					RECT windowRect;
					GetWindowRect(hWnd, &windowRect);

					SetWindowPos(
						hWnd,
						(setToTopMost) ? HWND_TOPMOST : HWND_NOTOPMOST,
						windowRect.left,
						windowRect.top,
						windowRect.right - windowRect.left,
						windowRect.bottom - windowRect.top,
						SWP_FRAMECHANGED | SWP_NOACTIVATE);
				}
			}
		}
	}
}