#pragma once

namespace Fox {

	namespace Graphics {

		namespace DirectX {

			class FOX_API DirectX12Renderer : virtual public Fox::Graphics::IRenderer, public Fox::Graphics::DirectX::IDeviceNotify {
			public:
				DirectX12Renderer(Fox::Graphics::RendererConfig& rendererConfig) : Fox::Graphics::IRenderer(rendererConfig) {}
				virtual ~DirectX12Renderer();
				BOOL Initialize() override;
				BOOL InitializeScene() override;
				VOID Destroy() override;

				VOID Update(FLOAT dt) override;
				VOID Render(FLOAT dt) override;
				BOOL Resize(UINT width, UINT height, BOOL minimized) override;

				VOID SetupCamera();
				VOID UpdateCamera();

				RECT GetFullscreenWindowRectangle() const;

				VOID OnDeviceLost() override;
				VOID OnDeviceRestored() override;

				BOOL IsTearingSupported() const;

			private: 
				std::unique_ptr<Fox::Graphics::DirectX::Direct3D> direct3D;
				std::unique_ptr<Fox::Graphics::DirectX::DirectXRaytracing> directXRaytracing;

				XMVECTOR cameraPosition;
				XMVECTOR cameraLookAtPosition;
				XMVECTOR cameraUp;

			};
		}
	}
}