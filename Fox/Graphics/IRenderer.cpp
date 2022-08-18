#include "Fox.h"

namespace Fox { 

	namespace Graphics {

		IRenderer* GetRenderer(Fox::Graphics::RendererConfig& rendererConfig) {
			Fox::Graphics::IRenderer* renderer = nullptr;

			switch (rendererConfig.API)
			{
			case Fox::Graphics::DirectX12Ultimate:
				renderer = new Fox::Graphics::DirectX::DirectX12Renderer(rendererConfig);
				break;
			case Fox::Graphics::Vulkan:
				break;
			default:
				throw std::exception("Unsupported renderer API");
				break;
			}
			return renderer;
		}
	}
}