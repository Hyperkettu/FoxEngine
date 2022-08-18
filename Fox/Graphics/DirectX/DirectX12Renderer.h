#pragma once

namespace Fox {

	namespace Graphics {

		namespace DirectX {

			class FOX_API DirectX12Renderer : virtual public Fox::Graphics::IRenderer, public Fox::Graphics::DirectX::IDeviceNotify {
			public:
				DirectX12Renderer(Fox::Graphics::RendererConfig& rendererConfig) : Fox::Graphics::IRenderer(rendererConfig) {}
				virtual ~DirectX12Renderer() {}
				BOOL Initialize() override;
				VOID Render(FLOAT dt) override;
				BOOL Resize(UINT width, UINT height, BOOL minimized);

				RECT GetFullscreenWindowRectangle() const;

				VOID OnDeviceLost() override;
				VOID OnDeviceRestored() override;

				BOOL IsTearingSupported() const;

			private: 
				std::unique_ptr<Fox::Graphics::DirectX::Direct3D> direct3D;
			};
		}
	}
}