#pragma once

namespace Fox {

    namespace Graphics {
    
        namespace DirectX {
        
            // Returns bool whether the device supports DirectX Raytracing tier.
            inline bool FOX_API IsDirectXRaytracingSupported(IDXGIAdapter1* adapter)
            {
                Microsoft::WRL::ComPtr<ID3D12Device> testDevice;
                D3D12_FEATURE_DATA_D3D12_OPTIONS5 featureSupportData = {};

                return SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&testDevice)))
                    && SUCCEEDED(testDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &featureSupportData, sizeof(featureSupportData)))
                    && featureSupportData.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED;
            }
        }
    }
}
