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

				VOID RenderBegin(D3D12_RESOURCE_STATES beforeState = D3D12_RESOURCE_STATE_PRESENT);
				VOID RenderEnd(D3D12_RESOURCE_STATES beforeState = D3D12_RESOURCE_STATE_RENDER_TARGET);

				VOID RegisterDeviceNotify(IDeviceNotify* deviceNotify);

				VOID InitializeDXGIAdapter();
				VOID SetAdapterOverride(UINT adapterId) { adapterIdOverride = adapterId; }
				VOID CreateDeviceResources();
				VOID CreateWindowSizeDependentResources();

				BOOL WindowSizeChanged(UINT width, UINT height, BOOL minimized);
				VOID HandleLostGraphicsDevice();

				VOID ExecuteMainCommandList();
				VOID WaitForGpu() noexcept;

				VOID SetWindow(HWND handle, UINT width, UINT height) { windowHandle = handle; screenWidth = width; screenHeight = height; }


				BOOL IsTearingSupported() const { return options & allowTearing; }
				BOOL IsWindowVisible() const { return isWindowVisible; }



				// Direct3D Accessors.
				IDXGIAdapter1* GetAdapter() const { return adapter.Get(); }
				ID3D12Device* GetDirect3DDevice() const { return direct3dDevice.Get(); }
				IDXGIFactory4* GetDXGIFactory() const { return dxgiFactory.Get(); }
				IDXGISwapChain3* GetSwapChain() const { return swapChain.Get(); }
				D3D_FEATURE_LEVEL GetDeviceFeatureLevel() const { return direct3DFeatureLevel; }
				ID3D12Resource* GetCurrentBackBufferRenderTarget() const { return backBufferRenderTargets[backBufferIndex].Get(); }
				ID3D12Resource* GetDepthStencil() const { return depthStencil.Get(); }
				ID3D12CommandQueue* GetMainCommandQueue() const { return mainCommandQueue.Get(); }
				ID3D12CommandAllocator* GetCurrentCommandAllocator() const { return commandAllocators[backBufferIndex].Get(); }
				ID3D12GraphicsCommandList* GetMainCommandList() const { return mainCommandList.Get(); }
				DXGI_FORMAT GetBackBufferFormat() const { return backBufferFormat; }
				DXGI_FORMAT GetDepthStencilBufferFormat() const { return depthStencilBufferFormat; }
				D3D12_VIEWPORT GetScreenViewport() const { return screenViewport; }
				D3D12_RECT GetScissorRect() const { return scissorRect; }
				UINT GetCurrentFrameIndex() const { return backBufferIndex; }
				UINT GetPreviousFrameIndex() const { return backBufferIndex == 0 ? backBufferCount - 1 : backBufferIndex - 1; }
				UINT GetBackBufferCount() const { return backBufferCount; }
				unsigned int GetDeviceOptions() const { return options; }
				LPCWSTR GetAdapterDescription() const { return adapterDescription.c_str(); }
				UINT GetAdapterID() const { return adapterId; }

				CD3DX12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView() const
				{
					return CD3DX12_CPU_DESCRIPTOR_HANDLE(renderTargetViewDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), backBufferIndex, renderTargetViewDescriptorSize);
				}
				CD3DX12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const
				{
					return CD3DX12_CPU_DESCRIPTOR_HANDLE(depthStencilViewDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
				}

			private:
				VOID MoveToNextFrame();
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
				Microsoft::WRL::ComPtr<ID3D12Resource> backBufferRenderTargets[MAX_BACKBUFFER_COUNT];
				Microsoft::WRL::ComPtr<ID3D12Resource> depthStencil;
				
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
				HWND windowHandle;

				UINT options;
				BOOL isWindowVisible;

				IDeviceNotify* deviceNotify;

				UINT screenWidth;
				UINT screenHeight;

			};
		}
	}
}