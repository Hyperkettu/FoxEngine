#include "Fox.h"

#pragma comment(lib, "d3d12.lib") 
#pragma comment(lib, "dxgi.lib") 
#pragma comment(lib, "dxguid.lib")

namespace Fox {

	namespace Graphics {

		namespace DirectX {

			Direct3D::Direct3D(DXGI_FORMAT backbufferFormat,
				DXGI_FORMAT depthStencilBufferFormat,
				UINT backbufferCount,
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

			// Wait for pending GPU work to complete.
			VOID Direct3D::WaitForGpu() noexcept
			{
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
		}
	}
}