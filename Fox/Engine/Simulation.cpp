#include "Fox.h"
#include "Simulation.h"

#include "Fox/Engine/SplashScreen.h"

namespace Fox {

	namespace Engine {
	
		Simulation::Simulation() : 
			Fox::Platform::Win32::Window(L"MainApplication", NULL)
		{

		}

		Simulation::~Simulation()
		{
		}

		VOID Simulation::PreInitialize() {

			Logger::PrintDebugSeparator();
			Logger::PrintLog(L"Application starting...\n");
			Logger::PrintLog(L"Game Name: %s\n", GameSettings::GameName());
			Logger::PrintLog(L"Boot Time: %s\n", GameSettings::BootTime());
			Logger::PrintLog(L"Engine Mode: %s\n", Fox::Engine::EngineModeToString().c_str());
			Logger::PrintDebugSeparator();

			Fox::Engine::SplashScreen::Open();

			Fox::Platform::Win32::Window::RegisterNewClass();
			Fox::Platform::Win32::Window::Initialize();

		}

		LRESULT Simulation::MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

			switch (message) {
				case WM_WINDOW_HANDLE_SET: { InitializeRenderer(); } break;
				case WM_SYSKEYDOWN:
					// ALT+ENTER:
					if ((wParam == VK_RETURN) && (lParam & (1 << 29)))
					{
						if (renderer->IsTearingSupported())
						{
							ToggleWindowFullscreen();
							return 0;
						}
					}
					break;
				case WM_SIZE:
					{
						RECT clientRect = {};
						GetClientRect(hWnd, &clientRect);
						OnResizeWindow(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, wParam == SIZE_MINIMIZED);
					}
					return 0;
			}

			return Window::MessageHandler(hWnd, message, wParam, lParam);
		}

		VOID Simulation::InitializeRenderer() {
			std::unique_ptr<Fox::Graphics::IRenderer> selectedRenderer(Fox::Graphics::GetRenderer(Fox::Graphics::GraphicsAPI::DirectX12Ultimate));
			renderer = std::move(selectedRenderer);

			Fox::Platform::WindowHandle windowHandle(handle);

			if (renderer->Initialize(windowHandle, width, height)) {
#ifdef _DEBUG
				Logger::PrintLog(L"Renderer initialized successfully\n");
#endif
			} else {
				throw std::exception("Failed to initialize selected renderer.");
			}
		}

		VOID Simulation::Render(FLOAT dt) {
			if (renderer) {
				renderer->Render(dt);
			}
		}

		VOID Simulation::OnKeyDown(UINT8 keyCode) {
			switch (keyCode) {
				case 27: { PostMessage(handle, WM_QUIT, 0, 0); } break; // close window on ESC key press
				case 'F': { ToggleWindowFullscreen(); } break;
			}
		}

		VOID Simulation::OnKeyUp(UINT8 keyCode) {
		
		}

		VOID Simulation::ToggleWindowFullscreen() {
			if (isFullscreen) {
				// Restore the window's attributes and size.
				SetWindowLong(handle, GWL_STYLE, windowStyle);

				SetWindowPos(
					handle,
					HWND_NOTOPMOST,
					windowRect.left,
					windowRect.top,
					windowRect.right - windowRect.left,
					windowRect.bottom - windowRect.top,
					SWP_FRAMECHANGED | SWP_NOACTIVATE);

				ShowWindow(handle, SW_NORMAL);
			} else {
				// Save the old window rect so we can restore it when exiting fullscreen mode.
				GetWindowRect(handle, &windowRect);

				// Make the window borderless so that the client area can fill the screen.
				SetWindowLong(handle, GWL_STYLE, windowStyle & ~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME));

				RECT fullscreenWindowRect = renderer->GetFullscreenWindowRectangle();

				SetWindowPos(
					handle,
					HWND_TOPMOST,
					fullscreenWindowRect.left,
					fullscreenWindowRect.top,
					fullscreenWindowRect.right,
					fullscreenWindowRect.bottom,
					SWP_FRAMECHANGED | SWP_NOACTIVATE);


				ShowWindow(handle, SW_MAXIMIZE);
			}

			Window::ToggleWindowFullscreen();
		}

		VOID Simulation::OnResizeWindow(UINT width, UINT height, BOOL minimized) {
			if (!renderer || !renderer->Resize(width, height, minimized)) {
				return;
			}

			Window::OnResizeWindow(width, height, minimized);
		}

	}
}
