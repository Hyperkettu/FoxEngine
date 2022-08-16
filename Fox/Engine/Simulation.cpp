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

	}
}
