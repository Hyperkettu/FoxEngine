#pragma once 

namespace Fox {

	namespace Graphics {

		enum GraphicsAPI : INT {
			DirectX12Ultimate = 0,
			Vulkan
		};

		enum Buffering {
			SINGLE_BUFFER = 0,
			DOUBLE_BUFFERING,
			TRIPLE_BUFFERING,
			MAX_NUMBER_OF_BUFFERS
		};

		enum DepthStencilBufferFormat {
			UNKNOWN,
			DEPTH32_FLOATING_POINT,
			DEPTH24_UNORM_STENCIL8_UINT
		};

		enum BackBufferFormat {
			R8G8B8A8_UNORM
		};
	
		struct FOX_API RendererConfig {
			RendererConfig() : windowHandle(nullptr) {}

			Fox::Graphics::GraphicsAPI API = Fox::Graphics::GraphicsAPI::DirectX12Ultimate;
			Fox::Platform::WindowHandle windowHandle;
			UINT screenWidth = 1024u;
			UINT screenHeight = 800u;
			BOOL allowTearing = TRUE;
			BOOL requireTearingSupport = TRUE;
			BOOL fullscreenEnabled = FALSE;
			Buffering buffering = Buffering::TRIPLE_BUFFERING;
			DepthStencilBufferFormat depthStencilBufferFormat = DepthStencilBufferFormat::UNKNOWN;
			BackBufferFormat backBufferFormat = BackBufferFormat::R8G8B8A8_UNORM;
		};
	}
}