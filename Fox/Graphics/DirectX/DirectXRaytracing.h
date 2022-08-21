#pragma once

namespace Fox {

	namespace Graphics {

		namespace DirectX {
			
			class FOX_API DirectXRaytracing {
			public:

				DirectXRaytracing() = default;

				VOID CreateDeviceDependentResources(const Fox::Graphics::DirectX::Direct3D& direct3D);
				VOID CreateWindowSizeDependentResources(const Fox::Graphics::DirectX::Direct3D& direct3D);
				VOID ReleaseDeviceDependentResources();
				VOID ReleaseWindowSizeDependentResources();

			private:

				VOID CreateRaytracingInterfaces(const Fox::Graphics::DirectX::Direct3D& direct3D);
				VOID CreateShaderRootSignatures(const Fox::Graphics::DirectX::Direct3D& direct3D);
				VOID CreateRaytracingPipelineStateObject(const Fox::Graphics::DirectX::Direct3D& direct3D);

				VOID CreateLocalRootSignatureSubobjects(CD3DX12_STATE_OBJECT_DESC* raytracingPipelineStateDesc);


				// DirectX Raytracing (DXR) attributes
				Microsoft::WRL::ComPtr<ID3D12Device5> dxrDevice;
				Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> dxrCommandList;
				Microsoft::WRL::ComPtr<ID3D12StateObject> dxrPipelineState;

				// Root signatures
				Microsoft::WRL::ComPtr<ID3D12RootSignature> raytracingGlobalRootSignature;
				Microsoft::WRL::ComPtr<ID3D12RootSignature> raytracingLocalRootSignature;

				// Constant buffers
				CubeData cubeConstantBuffer;
			};
		}
	}
}