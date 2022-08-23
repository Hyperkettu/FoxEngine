#include "Fox.h"

#include <sstream>

namespace Fox {

	namespace Graphics {

		namespace DirectX {
		
            ShaderTable::ShaderTable(ID3D12Device* device, UINT numShaderRecords, UINT shaderRecordSize, LPCWSTR resourceName)
                : name(resourceName)
            {
                shaderRecordSize = Fox::Graphics::DirectX::Align(shaderRecordSize, D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT);
                shaderRecords.reserve(numShaderRecords);
                UINT bufferSize = numShaderRecords * shaderRecordSize;
                Allocate(device, bufferSize, resourceName);
                shaderRecordData = GetBuffer();
            }

            VOID ShaderTable::AddRecord(const Fox::Graphics::DirectX::ShaderRecord& shaderRecord) {
                ThrowIfFalse(shaderRecords.size() < shaderRecords.capacity());
                shaderRecords.push_back(shaderRecord);
                shaderRecord.Copy(shaderRecordData);
                shaderRecordData += shaderRecordSize;
            }

            UINT ShaderTable::GetShaderRecordSize() { return shaderRecordSize; }

            // Pretty-print the shader records.
            VOID ShaderTable::DebugPrint(std::unordered_map<void*, std::wstring> map)
            {
                std::wstringstream stringStream;
                stringStream << L"|--------------------------------------------------------------------\n";
                stringStream << L"|Shader table - " << name.c_str() << L": "
                    << shaderRecordSize << L" | "
                    << shaderRecords.size() * shaderRecordSize << L" bytes\n";

                for (UINT i = 0; i < shaderRecords.size(); i++)
                {
                    stringStream << L"| [" << i << L"]: ";
                    stringStream << map[shaderRecords[i].shaderIdentifier.ptr] << L", ";
                    stringStream << shaderRecords[i].shaderIdentifier.size << L" + " << shaderRecords[i].localRootArguments.size << L" bytes \n";
                }
                stringStream << L"|--------------------------------------------------------------------\n";
                stringStream << L"\n";
                Logger::PrintLog((stringStream.str().c_str()));
            }
		
		}
	}
}