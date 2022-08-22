#pragma once

#define SizeOfInUint32(obj) ((sizeof(obj) - 1) / sizeof(UINT32) + 1)

namespace Fox {

    namespace Graphics {
    
        namespace DirectX {

            VOID FOX_API AllocateUnorderedAccessViewBuffer(ID3D12Device* pDevice, UINT64 bufferSize, ID3D12Resource** ppResource, 
                D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_COMMON, const wchar_t* resourceName = nullptr);


            inline VOID FOX_API AllocateUploadBuffer(ID3D12Device* pDevice, VOID* pData, UINT64 dataSize, ID3D12Resource** ppResource, const wchar_t* resourceName = nullptr) {
                auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
                auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(dataSize);
                ThrowIfFailed(pDevice->CreateCommittedResource(
                    &uploadHeapProperties,
                    D3D12_HEAP_FLAG_NONE,
                    &bufferDesc,
                    D3D12_RESOURCE_STATE_GENERIC_READ,
                    nullptr,
                    IID_PPV_ARGS(ppResource)));
                if (resourceName)
                {
                    (*ppResource)->SetName(resourceName);
                }
                void* pMappedData;
                (*ppResource)->Map(0, nullptr, &pMappedData);
                memcpy(pMappedData, pData, dataSize);
                (*ppResource)->Unmap(0, nullptr);
            }

            inline VOID FOX_API PrintPipelineStateObjectDesc(const D3D12_STATE_OBJECT_DESC* desc);
        
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
