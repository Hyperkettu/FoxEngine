#include "Fox.h"

namespace Fox {
	
	namespace Graphics {
	
		namespace DirectX {

			// initialize the raytracing pipeline
			VOID DirectXRaytracing::CreateDeviceDependentResources(const Fox::Graphics::DirectX::Direct3D& direct3D) {
				CreateRaytracingInterfaces(direct3D);
				CreateShaderRootSignatures(direct3D);
			}

			VOID DirectXRaytracing::CreateWindowSizeDependentResources(const Fox::Graphics::DirectX::Direct3D& direct3D) {
				
			}

			VOID DirectXRaytracing::ReleaseDeviceDependentResources() {
			
			}

			VOID DirectXRaytracing::ReleaseWindowSizeDependentResources() {
			
			}

			VOID DirectXRaytracing::CreateRaytracingInterfaces(const Fox::Graphics::DirectX::Direct3D& direct3D) {
				ThrowIfFailed(direct3D.GetDirect3DDevice()->QueryInterface(IID_PPV_ARGS(&dxrDevice)), L"Could not create DirectX 12 Raytracing device.");
				ThrowIfFailed(direct3D.GetMainCommandList()->QueryInterface(IID_PPV_ARGS(&dxrCommandList)), L"Could not create DirectX 12 Raytracing command list.");
#ifdef _DEBUG 
				Logger::PrintLog(L"DirectX 12 Raytracing interfaces created successfully.\n");
#endif
			}

			VOID DirectXRaytracing::CreateShaderRootSignatures(const Fox::Graphics::DirectX::Direct3D& direct3D) {
				ID3D12Device* device = direct3D.GetDirect3DDevice();
				// Create global root signature that is shared across all raytracing shaders
				{
					CD3DX12_DESCRIPTOR_RANGE ranges[2]; // order from most frequently used for performance
					ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0); // output texture to register u0;
					ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 1); // static shader and vertex buffers to registers t1, t2

					CD3DX12_ROOT_PARAMETER rootParameters[4];
					rootParameters[0].InitAsDescriptorTable(1, &ranges[0]); // output texture to register u0
					rootParameters[1].InitAsShaderResourceView(0); // raytracing acceleration structure to register t0
					rootParameters[2].InitAsConstantBufferView(0); // PerFrame constant buffer to register b0
					rootParameters[3].InitAsDescriptorTable(1, &ranges[1]); // vertex and index buffers to register t1 and t2
					CD3DX12_ROOT_SIGNATURE_DESC globalRootSignatureDesc(ARRAYSIZE(rootParameters), rootParameters);
					Microsoft::WRL::ComPtr<ID3DBlob> blob;
					Microsoft::WRL::ComPtr<ID3DBlob> error;

					ThrowIfFailed(D3D12SerializeRootSignature(&globalRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &error),
						error ? static_cast<wchar_t*>(error->GetBufferPointer()) : nullptr);

					ThrowIfFailed(device->CreateRootSignature(1, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&raytracingGlobalRootSignature)));
				}

				// Create local root signature
				// This is a root signature that enables a shader to have unique arguments that come from shader tables.
				{
					CD3DX12_ROOT_PARAMETER rootParameters[1];
					rootParameters[0].InitAsConstants(SizeOfInUint32(cubeConstantBuffer), 1); // cube constant buffer to register b1
					CD3DX12_ROOT_SIGNATURE_DESC localRootSignatureDesc(ARRAYSIZE(rootParameters), rootParameters);
					localRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
					Microsoft::WRL::ComPtr<ID3DBlob> blob;
					Microsoft::WRL::ComPtr<ID3DBlob> error;

					ThrowIfFailed(D3D12SerializeRootSignature(&localRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &error),
						error ? static_cast<wchar_t*>(error->GetBufferPointer()) : nullptr);

					ThrowIfFailed(device->CreateRootSignature(1, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&raytracingLocalRootSignature)));
				}

#ifdef _DEBUG
				Logger::PrintLog(L"Created shader root signatures successfully.\n");
#endif
			}
		}
	}
}