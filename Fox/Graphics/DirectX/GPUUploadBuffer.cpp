#include "Fox.h"

namespace Fox {

    namespace Graphics {

        namespace DirectX {

            GPUUploadBuffer::~GPUUploadBuffer() {
                if (resource.Get())
                {
                    resource->Unmap(0, nullptr);
                }
            }

            void GPUUploadBuffer::Allocate(ID3D12Device* device, UINT bufferSize, LPCWSTR resourceName)
            {
                auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

                auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
                ThrowIfFailed(device->CreateCommittedResource(
                    &uploadHeapProperties,
                    D3D12_HEAP_FLAG_NONE,
                    &bufferDesc,
                    D3D12_RESOURCE_STATE_GENERIC_READ,
                    nullptr,
                    IID_PPV_ARGS(&resource)));
                resource->SetName(resourceName);
            }

            uint8_t* GPUUploadBuffer::GetBuffer()
            {
                uint8_t* mappedData;
                CD3DX12_RANGE readRange(0, 0);
                ThrowIfFailed(resource->Map(0, &readRange, reinterpret_cast<void**>(&mappedData)));
                return mappedData;
            }

        }
    }
}