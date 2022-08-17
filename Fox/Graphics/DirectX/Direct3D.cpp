#include "Fox.h"

#pragma comment(lib, "d3d12.lib") 
#pragma comment(lib, "dxgi.lib") 
#pragma comment(lib, "dxguid.lib")

namespace Fox {

	namespace Graphics {

		namespace DirectX {

			inline DXGI_FORMAT NoSRGB(DXGI_FORMAT format)
			{
				switch (format)
				{
				case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:   return DXGI_FORMAT_R8G8B8A8_UNORM;
				case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:   return DXGI_FORMAT_B8G8R8A8_UNORM;
				case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:   return DXGI_FORMAT_B8G8R8X8_UNORM;
				default:                                return format;
				}
			}

			Direct3D::Direct3D(DXGI_FORMAT backBufferFormat,
				DXGI_FORMAT depthStencilBufferFormat,
				UINT backBufferCount,
				D3D_FEATURE_LEVEL minFeatureLevel,
				UINT flags,
				UINT adapterIdOverride) : 
				backBufferIndex(0),
				fenceValues{},
				renderTargetViewDescriptorHeap(0),
				screenViewport{},
				scissorRect{},
				backBufferFormat(backBufferFormat),
				depthStencilBufferFormat(depthStencilBufferFormat),
				backBufferCount(backBufferCount),
				minimumFeatureLevel(minFeatureLevel),
				direct3DFeatureLevel(D3D_FEATURE_LEVEL_12_1),
				options(flags),
				isWindowVisible(true),
				deviceNotify(nullptr),
				adapterIdOverride(adapterIdOverride),
				adapterId(UINT_MAX)
			{
				if (backBufferCount > MAX_BACKBUFFER_COUNT)
				{
					throw std::out_of_range("backBufferCount too large");
				}

				if (minFeatureLevel < D3D_FEATURE_LEVEL_11_0)
				{
					throw std::out_of_range("minFeatureLevel too low");
				}
				if (options & requireTearingSupport)
				{
					options |= allowTearing;
				}

			}

			Direct3D::~Direct3D()
			{
				// Ensure that the GPU is no longer referencing resources that are about to be destroyed.
				WaitForGpu();
			}

			VOID Direct3D::RegisterDeviceNotify(IDeviceNotify* deviceNotify)
			{
				this->deviceNotify = deviceNotify;

				// On RS4 and higher, applications that handle device removal 
				// should declare themselves as being able to do so
				__if_exists(DXGIDeclareAdapterRemovalSupport)
				{
					if (deviceNotify)
					{
						if (FAILED(DXGIDeclareAdapterRemovalSupport()))
						{
							Logger::PrintLog(L"Warning: application failed to declare adapter removal support.\n");
						}
					}
				}
			}

			VOID Direct3D::InitializeDXGIAdapter() {
				bool debugDXGI = false;

#if defined(_DEBUG)
				Microsoft::WRL::ComPtr<ID3D12Debug> debugController;

				if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
					debugController->EnableDebugLayer();
				}
				else {
					Logger::PrintLog(L"Direct3D Debug layer is not available\n");
				}

				Microsoft::WRL::ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
				if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiInfoQueue)))) {
					debugDXGI = true;
					ThrowIfFailed(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&dxgiFactory)));

					dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
					dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
				}
#endif

				if (!debugDXGI) {
					ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)));
				}

				// check if tearing is supported on fullscreen 
				if (options & (allowTearing | requireTearingSupport)) {
					BOOL isTearingAllowed = FALSE;
					Microsoft::WRL::ComPtr<IDXGIFactory5> factory5;
					HRESULT hr = dxgiFactory.As<IDXGIFactory5>(&factory5);

					if (SUCCEEDED(hr)) {
						hr = factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &isTearingAllowed, sizeof(isTearingAllowed));
					}

					if (FAILED(hr) || !isTearingAllowed)
					{
						Logger::PrintLog(L"WARNING: Variable refresh rate displays are not supported.\n");
						if (options & requireTearingSupport)
						{
							ThrowIfFailed(false, L"Error: Sample must be run on an OS with tearing support.\n");
						}
						options &= ~allowTearing;
					}
				}

				InitializeAdapter(&adapter);
			}

			VOID Direct3D::InitializeAdapter(IDXGIAdapter1** ppAdapter) {
				*ppAdapter = nullptr;

				Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
				Microsoft::WRL::ComPtr<IDXGIFactory6> factory6;
				HRESULT hr = dxgiFactory.As(&factory6);
				if (FAILED(hr))
				{
					throw std::exception("DXGI 1.6 not supported");
				}

				for (UINT adapterId = 0u; factory6->EnumAdapterByGpuPreference(adapterId, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)) 
					!= DXGI_ERROR_NOT_FOUND; adapterId++) {
				
					// force override adapter
					if (adapterIdOverride != UINT_MAX && adapterId != adapterIdOverride) {
						continue;
					}	

					DXGI_ADAPTER_DESC1 adapterDesc;
					adapter->GetDesc1(&adapterDesc);

					// skip software renderer
					if (adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE) {
						continue;
					}

					// find if selected adapter supports DirectX 12 without creating the device yet but save the adapter
					if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), minimumFeatureLevel, __uuidof(ID3D12Device), nullptr))) {
						this->adapterId = adapterId;
						adapterDescription = adapterDesc.Description;

#ifdef _DEBUG
						Logger::PrintLog(L"Direct3D Adapter (%u): VID:%04X, PID:%04X - %ls\n", 
							adapterId, adapterDesc.VendorId, adapterDesc.DeviceId, adapterDesc.Description);
#endif
						break;
					}

#if !defined(NDEBUG)
					if (!adapter && adapterIdOverride == UINT_MAX)
					{
						// Try WARP12 instead
						if (FAILED(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&adapter))))
						{
							throw std::exception("WARP12 not available. Enable the 'Graphics Tools' optional feature");
						}

						Logger::PrintLog(L"Direct3D Adapter - WARP12\n");
					}
#endif
					if (!adapter)
					{
						if (adapterIdOverride != UINT_MAX)
						{
							throw std::exception("Unavailable adapter requested.");
						}
						else
						{
							throw std::exception("Unavailable adapter.");
						}
					}

					*ppAdapter = adapter.Detach();
				}
			}

			VOID Direct3D::CreateDeviceResources() {
				// Create DirectX 12 API Device
				ThrowIfFailed(D3D12CreateDevice(adapter.Get(), minimumFeatureLevel, IID_PPV_ARGS(&direct3dDevice)));

#ifndef NDEBUG 
				Microsoft::WRL::ComPtr<ID3D12InfoQueue> direct3DInfoQueue;
				
				if (SUCCEEDED(direct3dDevice.As(&direct3DInfoQueue))) {
					// if configure debug device if active
#ifdef _DEBUG 
					direct3DInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
					direct3DInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
#endif
					D3D12_MESSAGE_ID hide[] =
					{
						D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
						D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE
					};
					D3D12_INFO_QUEUE_FILTER filter = {};
					filter.DenyList.NumIDs = _countof(hide);
					filter.DenyList.pIDList = hide;
					direct3DInfoQueue->AddStorageFilterEntries(&filter);
				}
#endif
				// Determine maximum supported feature level for this device
				static const D3D_FEATURE_LEVEL s_featureLevels[] =
				{
					D3D_FEATURE_LEVEL_12_1,
					D3D_FEATURE_LEVEL_12_0,
					D3D_FEATURE_LEVEL_11_1,
					D3D_FEATURE_LEVEL_11_0,
				};

				D3D12_FEATURE_DATA_FEATURE_LEVELS featureLevels =
				{
					_countof(s_featureLevels), s_featureLevels, D3D_FEATURE_LEVEL_11_0
				};

				HRESULT hr = direct3dDevice->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featureLevels, sizeof(featureLevels));

				if (SUCCEEDED(hr))
				{
					direct3DFeatureLevel = featureLevels.MaxSupportedFeatureLevel;
				}
				else
				{
					direct3DFeatureLevel = minimumFeatureLevel;
				}

				// Create the main command queue.
				D3D12_COMMAND_QUEUE_DESC queueDesc = {};
				queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
				queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

				ThrowIfFailed(direct3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mainCommandQueue)));

				// Create descriptor heaps for render target views and depth stencil views.
				D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc = {};
				rtvDescriptorHeapDesc.NumDescriptors = backBufferCount;
				rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

				ThrowIfFailed(direct3dDevice->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(&renderTargetViewDescriptorHeap)));

				renderTargetViewDescriptorSize = direct3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

				if (depthStencilBufferFormat != DXGI_FORMAT_UNKNOWN)
				{
					D3D12_DESCRIPTOR_HEAP_DESC dsvDescriptorHeapDesc = {};
					dsvDescriptorHeapDesc.NumDescriptors = 1;
					dsvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

					ThrowIfFailed(direct3dDevice->CreateDescriptorHeap(&dsvDescriptorHeapDesc, IID_PPV_ARGS(&depthStencilViewDescriptorHeap)));
				}

				// Create a command allocator for each back buffer that will be rendered to.
				for (UINT i = 0u; i < backBufferCount; i++)
				{
					ThrowIfFailed(direct3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocators[i])));
				}

				// Create a command list for recording graphics commands.
				ThrowIfFailed(direct3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocators[0].Get(), nullptr, IID_PPV_ARGS(&mainCommandList)));
				ThrowIfFailed(mainCommandList->Close());

				// Create a fence for tracking GPU execution progress.
				ThrowIfFailed(direct3dDevice->CreateFence(fenceValues[backBufferIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
				fenceValues[backBufferIndex]++;

				fenceEvent.Attach(CreateEvent(nullptr, FALSE, FALSE, nullptr));
				if (!fenceEvent.IsValid())
				{
					ThrowIfFailed(E_FAIL, L"CreateEvent failed.\n");
				}
			}

			VOID Direct3D::CreateWindowSizeDependentResources() {
				if (!windowHandle) {
					ThrowIfFailed(E_HANDLE, L"Call SetWindow with a valid Win32 window handle.\n");
				}

				// Wait until all previous GPU work is complete.
				WaitForGpu();

				// Release resources that are tied to the swap chain and update fence values.
				for (UINT i = 0u; i < backBufferCount; i++) {
					backBufferRenderTargets[i].Reset();
					fenceValues[i] = fenceValues[backBufferIndex];
				}

				DXGI_FORMAT backBufferFormat = NoSRGB(this->backBufferFormat);
				
				// if swapchain exists, resize it, otherwise create it
				if (swapChain) {
					HRESULT hr = swapChain->ResizeBuffers(backBufferCount, screenWidth, screenHeight, 
						backBufferFormat, (options & allowTearing) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0);

					if (hr == ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) {
#ifdef _DEBUG
						Logger::PrintLog(L"Device Lost on ResizeBuffers: Reason code 0x%08X\n", (hr == DXGI_ERROR_DEVICE_REMOVED) ?
							direct3dDevice->GetDeviceRemovedReason() : hr);
#endif // _DEBUG
						// If the device was removed for any reason, a new device and swap chain will need to be created.
						HandleLostGraphicsDevice();

						// Everything is set up now. Do not continue execution of this method. HandleDeviceLost will reenter this method 
						// and correctly set up the new device.
						return;
					
					} else {
						ThrowIfFailed(hr);
					}

				} else { // swapchain does not exist so create one 
					DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
					swapChainDesc.Width = screenWidth;
					swapChainDesc.Height = screenHeight;
					swapChainDesc.Format = backBufferFormat;
					swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
					swapChainDesc.BufferCount = backBufferCount;
					swapChainDesc.SampleDesc.Count = 1;
					swapChainDesc.SampleDesc.Quality = 0;
					swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
					swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
					swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
					swapChainDesc.Flags = (options & allowTearing) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
					
					DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullScreenDesc = {};
					swapChainFullScreenDesc.Windowed = TRUE;

					// create swap chain
					Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;

					// DXGI does not allow creating a swapchain targeting a window which has fullscreen styles(no border + topmost).
					// Temporarily remove the topmost property for creating the swapchain.
					BOOL isFullscreen = Fox::Platform::Win32::Utils::IsWindowFullscreen(windowHandle);
					if (isFullscreen)
					{
						Fox::Platform::Win32::Utils::SetWindowZorderToTopMost(windowHandle, false);
					}

					ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(mainCommandQueue.Get(), windowHandle, &swapChainDesc,
						&swapChainFullScreenDesc, nullptr, &swapChain));

					if (isFullscreen) {
						Fox::Platform::Win32::Utils::SetWindowZorderToTopMost(windowHandle, true);
					}
					
					ThrowIfFailed(swapChain.As(&this->swapChain));

					// With tearing support enabled we will handle ALT+Enter key presses in the
					// window message loop rather than let DXGI handle it by calling SetFullscreenState.
					if (IsTearingSupported())
					{
						dxgiFactory->MakeWindowAssociation(windowHandle, DXGI_MWA_NO_ALT_ENTER);
					}
				}

				// Obtain the back buffers for this window which will be the final render targets
				// and create render target views for each of them.
				for (UINT i = 0u; i < backBufferCount; i++)
				{
					ThrowIfFailed(swapChain->GetBuffer(i, IID_PPV_ARGS(&backBufferRenderTargets[i])));

					wchar_t name[25] = {};
					swprintf_s(name, L"Render target %u", i);
					backBufferRenderTargets[i]->SetName(name);

					D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
					rtvDesc.Format = backBufferFormat;
					rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

					CD3DX12_CPU_DESCRIPTOR_HANDLE rtvDescriptor(renderTargetViewDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), i, renderTargetViewDescriptorSize);
					direct3dDevice->CreateRenderTargetView(backBufferRenderTargets[i].Get(), &rtvDesc, rtvDescriptor);
				}

				// Reset the index to the current back buffer.
				backBufferIndex = swapChain->GetCurrentBackBufferIndex();

				// create depth stencil buffer
				if (depthStencilBufferFormat != DXGI_FORMAT_UNKNOWN) {
					// Allocate a 2-D surface as the depth/stencil buffer and create a depth/stencil view
					// on this surface.
					CD3DX12_HEAP_PROPERTIES depthHeapProperties(D3D12_HEAP_TYPE_DEFAULT);
					D3D12_RESOURCE_DESC depthStencilDesc = CD3DX12_RESOURCE_DESC::Tex2D(depthStencilBufferFormat, screenWidth, screenHeight, 1, 1);

					depthStencilDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

					D3D12_CLEAR_VALUE clearValue = {};
					clearValue.Format = depthStencilBufferFormat;
					clearValue.DepthStencil.Depth = 1.0f;
					clearValue.DepthStencil.Stencil = 0;

					direct3dDevice->CreateCommittedResource(
						&depthHeapProperties,
						D3D12_HEAP_FLAG_NONE,
						&depthStencilDesc,
						D3D12_RESOURCE_STATE_DEPTH_WRITE,
						&clearValue,
						IID_PPV_ARGS(&depthStencil));

					depthStencil->SetName(L"Depth stencil");

					D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
					dsvDesc.Format = depthStencilBufferFormat;
					dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

					direct3dDevice->CreateDepthStencilView(depthStencil.Get(), &dsvDesc, depthStencilViewDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

				}

				// Set the 3D rendering viewport and scissor rectangle to target the entire window.
				screenViewport.TopLeftX = screenViewport.TopLeftY = 0.f;
				screenViewport.Width = static_cast<float>(screenWidth);
				screenViewport.Height = static_cast<float>(screenHeight);
				screenViewport.MinDepth = D3D12_MIN_DEPTH;
				screenViewport.MaxDepth = D3D12_MAX_DEPTH;

				scissorRect.left = scissorRect.top = 0;
				scissorRect.right = screenWidth;
				scissorRect.bottom = screenHeight;
			}

			VOID Direct3D::HandleLostGraphicsDevice() {
				if (deviceNotify)
				{
					deviceNotify->OnDeviceLost();
				}

				// reset all device resources
				for (UINT i = 0u; i < backBufferCount; i++)
				{
					commandAllocators[i].Reset();
					backBufferRenderTargets[i].Reset();
				}

				depthStencil.Reset();
				mainCommandQueue.Reset();
				mainCommandList.Reset();
				fence.Reset();
				renderTargetViewDescriptorHeap.Reset();
				depthStencilViewDescriptorHeap.Reset();
				swapChain.Reset();
				direct3dDevice.Reset();
				dxgiFactory.Reset();
				adapter.Reset();
			
#ifdef _DEBUG
				{
					Microsoft::WRL::ComPtr<IDXGIDebug1> dxgiDebug;
					if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
					{
						dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
					}
				}
#endif
				// restore device resources
				InitializeDXGIAdapter();
				CreateDeviceResources();
				CreateWindowSizeDependentResources();


				if (deviceNotify) {
					deviceNotify->OnDeviceRestored();
				}
			}

			// This method is called when the Win32 window changes size.
			// It returns true if window size change was applied.
			BOOL Direct3D::Resize(UINT width, UINT height, BOOL minimized) {
				if (minimized || width == 0 || height == 0) {
					return FALSE;
				}

				// don't recreate resources if screen size did not really change
				if (screenWidth == width && screenHeight == height) {
					return FALSE;
				}

				screenWidth = width;
				screenHeight = height;
				aspectRatio = static_cast<FLOAT>(screenWidth) / static_cast<FLOAT>(screenHeight);

#ifdef _DEBUG
				Logger::PrintLog(L"Window resized: %u %u\n", screenWidth, screenHeight);
#endif
				CreateWindowSizeDependentResources();

				return TRUE;
			
}

			VOID Direct3D::ExecuteMainCommandList() {
				ThrowIfFailed(mainCommandList->Close());
				ID3D12CommandList* commandLists[] = { mainCommandList.Get() };
				mainCommandQueue->ExecuteCommandLists(ARRAYSIZE(commandLists), commandLists);
			}

			VOID Direct3D::RenderBegin(D3D12_RESOURCE_STATES beforeState) {
				// Reset command list and allocator.
				ThrowIfFailed(commandAllocators[backBufferIndex]->Reset());
				ThrowIfFailed(mainCommandList->Reset(commandAllocators[backBufferIndex].Get(), nullptr));

				if (beforeState != D3D12_RESOURCE_STATE_RENDER_TARGET)
				{
					// Transition the render target into the correct state to allow for drawing into it.
					D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(backBufferRenderTargets[backBufferIndex].Get(), 
						beforeState, D3D12_RESOURCE_STATE_RENDER_TARGET);
					mainCommandList->ResourceBarrier(1, &barrier);
				}
			}

			VOID Direct3D::RenderEnd(D3D12_RESOURCE_STATES beforeState) {
				if (beforeState != D3D12_RESOURCE_STATE_PRESENT) {
					// Transition the render target to the state that allows it to be presented to the display.
					D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(backBufferRenderTargets[backBufferIndex].Get(), beforeState, D3D12_RESOURCE_STATE_PRESENT);
					mainCommandList->ResourceBarrier(1, &barrier);
				}

				ExecuteMainCommandList();
				
				HRESULT hr;
				if (options & allowTearing) {
					// Recommended to always use tearing if supported when using a sync interval of 0.
					// Note this will fail if in true 'fullscreen' mode.
					hr = swapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING);
				} else {
					// The first argument instructs DXGI to block until VSync, putting the application
					// to sleep until the next VSync. This ensures we don't waste any cycles rendering
					// frames that will never be displayed to the screen.
					hr = swapChain->Present(1, 0);
				}

				// If the device was reset we must completely reinitialize the renderer.
				if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) {
#ifdef _DEBUG
					Logger::PrintLog(L"Device Lost on Present: Reason code 0x%08X\n", (hr == DXGI_ERROR_DEVICE_REMOVED) ? 
						direct3dDevice->GetDeviceRemovedReason() : hr);
#endif
					HandleLostGraphicsDevice();
				} else {
					ThrowIfFailed(hr);
					MoveToNextFrame();
				}	
			}


			// Wait for pending GPU work to complete.
			VOID Direct3D::WaitForGpu() noexcept {
				if (mainCommandQueue && fence && fenceEvent.IsValid())
				{
					// Schedule a signal command in the GPU queue.
					UINT64 fenceValue = fenceValues[backBufferIndex];
					if (SUCCEEDED(mainCommandQueue->Signal(fence.Get(), fenceValue)))
					{
						// Wait until the signal has been processed.
						if (SUCCEEDED(fence->SetEventOnCompletion(fenceValue, fenceEvent.Get())))
						{
							WaitForSingleObjectEx(fenceEvent.Get(), INFINITE, FALSE);

							// Increment the fence value for the current frame.
							fenceValues[backBufferIndex]++;
						}
					}
				}
			}

			VOID Direct3D::MoveToNextFrame() {
				// Schedule a Signal command in the queue.
				const UINT64 currentFenceValue = fenceValues[backBufferIndex];
				ThrowIfFailed(mainCommandQueue->Signal(fence.Get(), currentFenceValue));

				// Update the back buffer index.
				backBufferIndex = swapChain->GetCurrentBackBufferIndex();

				// If the next frame is not ready to be rendered yet, wait until it is ready.
				if (fence->GetCompletedValue() < fenceValues[backBufferIndex])
				{
					ThrowIfFailed(fence->SetEventOnCompletion(fenceValues[backBufferIndex], fenceEvent.Get()));
					WaitForSingleObjectEx(fenceEvent.Get(), INFINITE, FALSE);
				}

				// Set the fence value for the next frame.
				fenceValues[backBufferIndex] = currentFenceValue + 1;
			}

		}
	}
}