#pragma once

namespace Fox {

	namespace Graphics {

		class FOX_API IRenderer;

		enum GraphicsAPI : INT {
			DirectX12Ultimate = 0,
			Vulkan
		};

		IRenderer* GetRenderer(Fox::Graphics::GraphicsAPI api);

		class FOX_API IRenderer {
		public:
			virtual BOOL Initialize(Fox::Platform::WindowHandle& windowHandle, UINT screenWidth, UINT screenHeight) = 0;
			virtual VOID Render(FLOAT dt) = 0;
			virtual BOOL Resize(UINT width, UINT height, BOOL minimized) = 0;

			virtual RECT GetFullscreenWindowRectangle() const = 0;

			virtual BOOL IsTearingSupported() const = 0;
		};
	}
}