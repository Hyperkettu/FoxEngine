#include "Fox.h"

namespace Fox {

	namespace Graphics {

		namespace DirectX {

			BOOL DirectX12Renderer::Initialize(Fox::Platform::WindowHandle& windowHandle, UINT screenWidth, UINT screenHeight) {
				UINT numBackbuffers = 3u;
				direct3D = std::make_unique<Fox::Graphics::DirectX::Direct3D>(
					DXGI_FORMAT_R8G8B8A8_UNORM,
					DXGI_FORMAT_UNKNOWN,
					numBackbuffers,
					D3D_FEATURE_LEVEL_12_1,
					Fox::Graphics::DirectX::Direct3D::requireTearingSupport
					);

				HWND windowsHandle = static_cast<HWND>(windowHandle.GetHandle());

				direct3D->RegisterDeviceNotify(this);
				direct3D->SetWindow(windowsHandle, screenWidth, screenHeight);
				direct3D->InitializeDXGIAdapter();

				Fox::Graphics::DirectX::ThrowIfFailed(Fox::Graphics::DirectX::IsDirectXRaytracingSupported(direct3D->GetAdapter()),
					L"ERROR: DirectX Raytracing is not supported by your OS, GPU and/or driver.\n\n");
				direct3D->CreateDeviceResources();
				direct3D->CreateWindowSizeDependentResources();

#ifdef _DEBUG
				Logger::PrintLog(L"DirectX 12 Ultimate Device initialized.\n");
#endif
				return TRUE;
			}

			BOOL DirectX12Renderer::Resize(UINT width, UINT height, BOOL minimized) {
				return direct3D ? direct3D->Resize(width, height, minimized) : FALSE;
			}

			BOOL DirectX12Renderer::IsTearingSupported() const {
				return direct3D ? direct3D->IsTearingSupported() : FALSE;
			}


			VOID DirectX12Renderer::OnDeviceLost() {
				//	ReleaseWindowSizeDependentResources();
				//	ReleaseDeviceDependentResources();
			}

			VOID DirectX12Renderer::OnDeviceRestored() {
				//CreateDeviceDependentResources();
				//CreateWindowSizeDependentResources();
			}

			RECT DirectX12Renderer::GetFullscreenWindowRectangle() const {
				RECT fullscreenWindowRect;
				try
				{
					if (direct3D && direct3D->GetSwapChain())
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

				return fullscreenWindowRect;
			}

			VOID DirectX12Renderer::Render(FLOAT dt) {
				if (!direct3D || !direct3D->IsWindowVisible()) {
					return;
				}

				direct3D->RenderBegin();
				FLOAT color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
				direct3D->GetMainCommandList()->ClearRenderTargetView(direct3D->GetRenderTargetView(), color, 0, nullptr);
				direct3D->RenderEnd();
			}


		}
	}
}