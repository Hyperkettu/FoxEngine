#include "Fox.h"

namespace Fox {

	namespace Graphics {

		namespace DirectX {

			BOOL DirectX12Renderer::Initialize() {
				direct3D = std::make_unique<Fox::Graphics::DirectX::Direct3D>(config);
				directXRaytracing = std::make_unique<Fox::Graphics::DirectX::DirectXRaytracing>();

				HWND windowsHandle = static_cast<HWND>(config.windowHandle.GetHandle());

				direct3D->RegisterDeviceNotify(this);
				direct3D->SetWindow(windowsHandle, config.screenWidth, config.screenHeight);
				direct3D->InitializeDXGIAdapter();

				Fox::Graphics::DirectX::ThrowIfFailed(Fox::Graphics::DirectX::IsDirectXRaytracingSupported(direct3D->GetAdapter()),
					L"ERROR: DirectX Raytracing is not supported by your OS, GPU and/or driver.\n\n");
				direct3D->CreateDeviceResources();
				direct3D->CreateWindowSizeDependentResources();

#ifdef _DEBUG
				Logger::PrintLog(L"DirectX 12 Ultimate Device initialized.\n");
#endif

				InitializeScene();

				directXRaytracing->CreateDeviceDependentResources(*direct3D.get());
				directXRaytracing->CreateWindowSizeDependentResources(*direct3D.get());
				UpdateCamera();

				return TRUE;
			}

			BOOL DirectX12Renderer::Resize(UINT width, UINT height, BOOL minimized) {
				return direct3D ? direct3D->Resize(width, height, minimized) : FALSE;
			}

			BOOL DirectX12Renderer::IsTearingSupported() const {
				return direct3D ? direct3D->IsTearingSupported() : FALSE;
			}


			VOID DirectX12Renderer::OnDeviceLost() {
				directXRaytracing->ReleaseWindowSizeDependentResources();
				directXRaytracing->ReleaseDeviceDependentResources();
			}

			VOID DirectX12Renderer::OnDeviceRestored() {
				directXRaytracing->CreateDeviceDependentResources(*direct3D.get());
				directXRaytracing->CreateWindowSizeDependentResources(*direct3D.get());
				UpdateCamera();
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

			BOOL DirectX12Renderer::InitializeScene() {
				// Setup materials.
				{
					directXRaytracing->SetupMaterialConstantBuffer();
				}

				// Setup camera.
				{
					SetupCamera();
				}

				return TRUE;
			}

			VOID DirectX12Renderer::SetupCamera() {
				// Initialize the view and projection inverse matrices.
				cameraPosition = { 0.0f, 2.0f, -5.0f, 1.0f };
				cameraLookAtPosition = { 0.0f, 0.0f, 0.0f, 1.0f };
				XMVECTOR right = { 1.0f, 0.0f, 0.0f, 0.0f };

				XMVECTOR direction = XMVector4Normalize(cameraLookAtPosition - cameraPosition);
				cameraUp = XMVector3Normalize(XMVector3Cross(direction, right));

				// Rotate camera around Y axis.
				XMMATRIX rotate = XMMatrixRotationY(XMConvertToRadians(45.0f));
				cameraPosition = XMVector3Transform(cameraPosition, rotate);
				cameraUp = XMVector3Transform(cameraUp, rotate);

				UpdateCamera();
			}

			VOID DirectX12Renderer::UpdateCamera(){ 
				PerFrame data;
				FLOAT fovAngleY = 45.0f;

				XMMATRIX view = XMMatrixLookAtLH(cameraPosition, cameraLookAtPosition, cameraUp);
				XMMATRIX projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(fovAngleY), direct3D->GetAspectRatio(), 0.1f, 1000.0f);
				XMMATRIX viewProjection = view * projection;

				data.cameraPosition = cameraPosition;
				data.inverseProjection = XMMatrixInverse(nullptr, viewProjection);

				directXRaytracing->UpdatePerFrameConstantBuffer(*direct3D.get(), data);
			}



		}
	}
}