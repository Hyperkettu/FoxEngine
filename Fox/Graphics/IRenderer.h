#pragma once

namespace Fox {

	namespace Graphics {

		class FOX_API IRenderer;

		IRenderer* GetRenderer(Fox::Graphics::RendererConfig& rendererConfig);

		class FOX_API IRenderer {
		public:

			IRenderer(Fox::Graphics::RendererConfig& rendererConfig) : config(rendererConfig) {}

			virtual BOOL Initialize() = 0;
			virtual VOID Render(FLOAT dt) = 0;
			virtual BOOL Resize(UINT width, UINT height, BOOL minimized) = 0;

			virtual RECT GetFullscreenWindowRectangle() const = 0;

			virtual BOOL IsTearingSupported() const = 0;

		protected: 
			Fox::Graphics::RendererConfig config;
		};
	}
}