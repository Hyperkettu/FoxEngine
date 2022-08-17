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
						if (direct3D->IsTearingSupported())
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

		VOID Simulation::OnDeviceLost() {
		//	ReleaseWindowSizeDependentResources();
		//	ReleaseDeviceDependentResources();
		}

		VOID Simulation::OnDeviceRestored() {
			//CreateDeviceDependentResources();
			//CreateWindowSizeDependentResources();
		}

		VOID Simulation::InitializeRenderer() {
			UINT numBackbuffers = 3u;
			direct3D = std::make_unique<Fox::Graphics::DirectX::Direct3D>(
				DXGI_FORMAT_R8G8B8A8_UNORM,
				DXGI_FORMAT_UNKNOWN,
				numBackbuffers,
				D3D_FEATURE_LEVEL_12_1,
				Fox::Graphics::DirectX::Direct3D::requireTearingSupport
			);

			direct3D->RegisterDeviceNotify(this);
			direct3D->SetWindow(handle, width, height);
			direct3D->InitializeDXGIAdapter();

			Fox::Graphics::DirectX::ThrowIfFailed(Fox::Graphics::DirectX::IsDirectXRaytracingSupported(direct3D->GetAdapter()),
				L"ERROR: DirectX Raytracing is not supported by your OS, GPU and/or driver.\n\n");
			direct3D->CreateDeviceResources();
			direct3D->CreateWindowSizeDependentResources();
			Logger::PrintLog(L"DirectX 12 Device inited.\n");
		}

		VOID Simulation::Render(FLOAT dt) {
			if (!direct3D) {
				return;
			}

			direct3D->RenderBegin();
			FLOAT color[4] = { 1.0f, 0.0f, 1.0f, 1.0f };
			direct3D->GetMainCommandList()->ClearRenderTargetView(direct3D->GetRenderTargetView(), color, 0, nullptr);
			direct3D->RenderEnd();
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

				RECT fullscreenWindowRect;
				try
				{
					if (direct3D->GetSwapChain())
					{
						// Get the settings of the display on which the app's window is currently displayed
						Microsoft::WRL::ComPtr<IDXGIOutput> pOutput;
						Fox::Graphics::DirectX::ThrowIfFailed(direct3D->GetSwapChain()->GetContainingOutput(&pOutput));
						DXGI_OUTPUT_DESC Desc;
						Fox::Graphics::DirectX::ThrowIfFailed(pOutput->GetDesc(&Desc));
						fullscreenWindowRect = Desc.DesktopCoordinates;
					} else
					{
						// Fallback to EnumDisplaySettings implementation
						throw Fox::Graphics::DirectX::HRException(S_FALSE);
					}
				} catch (Fox::Graphics::DirectX::HRException& e)
				{
					UNREFERENCED_PARAMETER(e);

					// Get the settings of the primary display
					DEVMODE devMode = {};
					devMode.dmSize = sizeof(DEVMODE);
					EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &devMode);

					fullscreenWindowRect = {
						devMode.dmPosition.x,
						devMode.dmPosition.y,
						devMode.dmPosition.x + static_cast<LONG>(devMode.dmPelsWidth),
						devMode.dmPosition.y + static_cast<LONG>(devMode.dmPelsHeight)
					};
				}

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
			if (!direct3D || !direct3D->Resize(width, height, minimized)) {
				return;
			}

			Window::OnResizeWindow(width, height, minimized);
		}

	}
}
