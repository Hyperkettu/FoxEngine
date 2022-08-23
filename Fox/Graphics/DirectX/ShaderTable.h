#pragma once 

#include <unordered_map>

namespace Fox {

	namespace Graphics {
	
		namespace DirectX {
		
            class FOX_API ShaderTable : public Fox::Graphics::DirectX::GPUUploadBuffer {

            public:
                ShaderTable(ID3D12Device* device, UINT numShaderRecords, UINT shaderRecordSize, LPCWSTR resourceName = nullptr);

                VOID AddRecord(const Fox::Graphics::DirectX::ShaderRecord& shaderRecord);
                UINT GetShaderRecordSize();
                VOID DebugPrint(std::unordered_map<void*, std::wstring> map);


            private:
                uint8_t* shaderRecordData;
                UINT shaderRecordSize;

                std::wstring name;
                std::vector<Fox::Graphics::DirectX::ShaderRecord> shaderRecords;

                ShaderTable() {}
            };
		}
	 }
}