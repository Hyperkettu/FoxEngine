#pragma once

namespace Fox {

	namespace Graphics {
		
		namespace DirectX {

			interface IDeviceNotify {
				virtual VOID OnDeviceLost() = 0;
				virtual VOID OnDeviceRestored() = 0;
			};
		
			class FOX_API Direct3D {
			public: 

				static const UINT allowTearing = 0x1u;
				static const UINT requireTearingSupport = 0x2u;

				Direct3D(DXGI_FORMAT backbufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM,
					DXGI_FORMAT depthStencilBufferFormat = DXGI_FORMAT_D32_FLOAT,
					UINT backbufferCount = 3,
					D3D_FEATURE_LEVEL minFeatureLevel = D3D_FEATURE_LEVEL_12_1,
					UINT flags = 0,
					UINT adapterIdOverride = UINT_MAX);

				~Direct3D();

				VOID InitializeDXGIAdapter();
				VOID SetAdapterOverride(UINT adapterId) { adapterIdOverride = adapterId; }
				VOID CreateDeviceResources();


				VOID WaitForGpu() noexcept;

				IDXGIAdapter1* GetAdapter() const { return adapter.Get(); }

			private:

				VOID InitializeAdapter(IDXGIAdapter1** ppAdapter);


				static const UINT MAX_BACKBUFFER_COUNT = 3u;

				UINT adapterIdOverride;
				UINT backBufferIndex;
				Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
				UINT adapterId;
				std::wstring adapterDescription;

				// Direct3D 
				Microsoft::WRL::ComPtr<ID3D12Device> direct3dDevice;
				Microsoft::WRL::ComPtr<ID3D12CommandQueue> mainCommandQueue;
				Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mainCommandList;
				Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocators[MAX_BACKBUFFER_COUNT];

				// Swap chain
				Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
				Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain;
				Microsoft::WRL::ComPtr<IDXGIResource> backBufferRenderTargets[MAX_BACKBUFFER_COUNT];
				Microsoft::WRL::ComPtr<IDXGIResource> depthStencil;
				
				// Presentation fence objects 
				Microsoft::WRL::ComPtr<ID3D12Fence> fence;
				UINT64 fenceValues[MAX_BACKBUFFER_COUNT];
				Microsoft::WRL::Wrappers::Event fenceEvent;

				// Direct3D rendering
				Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> renderTargetViewDescriptorHeap;
				Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> depthStencilViewDescriptorHeap;
				UINT renderTargetViewDescriptorSize;
				D3D12_VIEWPORT	screenViewport;
				D3D12_RECT scissorRect;

				// Direct3D properties
				DXGI_FORMAT backBufferFormat;
				DXGI_FORMAT depthStencilBufferFormat;
				UINT backBufferCount;
				D3D_FEATURE_LEVEL minimumFeatureLevel;

				// cached properties
				D3D_FEATURE_LEVEL direct3DFeatureLevel;

				UINT options;

				IDeviceNotify* deviceNotify;

			};
		}
	}
}