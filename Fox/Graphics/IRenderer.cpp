#include "Fox.h"

namespace Fox { 

	namespace Graphics {

		IRenderer* GetRenderer(Fox::Graphics::GraphicsAPI api) {
			Fox::Graphics::IRenderer* renderer = nullptr;

			switch (api)
			{
			case Fox::Graphics::DirectX12Ultimate:
				renderer = new Fox::Graphics::DirectX::DirectX12Renderer();
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