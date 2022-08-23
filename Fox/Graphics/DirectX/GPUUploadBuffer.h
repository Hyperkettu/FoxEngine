#pragma once

namespace Fox {

    namespace Graphics {

        namespace DirectX {

            class FOX_API GPUUploadBuffer
            {
            public:
                Microsoft::WRL::ComPtr<ID3D12Resource> GetResource() { return resource; }

            protected:
                Microsoft::WRL::ComPtr<ID3D12Resource> resource;

                GPUUploadBuffer() {}
                ~GPUUploadBuffer();

                void Allocate(ID3D12Device* device, UINT bufferSize, LPCWSTR resourceName = nullptr); 

                // write only buffer by CPU
                uint8_t* GetBuffer();
            };
        }
    }
}