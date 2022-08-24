#pragma once

namespace Fox {

	namespace Graphics {

		namespace DirectX {

			struct Direct3DBuffer
			{
				Microsoft::WRL::ComPtr<ID3D12Resource> resource;
				D3D12_CPU_DESCRIPTOR_HANDLE handleForCPU;
				D3D12_GPU_DESCRIPTOR_HANDLE handleForGPU;
			};
			
			class FOX_API DirectXRaytracing {
			public:

				DirectXRaytracing() = default;

				VOID CreateDeviceDependentResources(Fox::Graphics::DirectX::Direct3D& direct3D);
				VOID CreateWindowSizeDependentResources(Fox::Graphics::DirectX::Direct3D& direct3D);
				VOID ReleaseDeviceDependentResources();
				VOID ReleaseWindowSizeDependentResources();

				VOID SetupMaterialConstantBuffer();
				VOID SetupMaterialConstantBufferLights(Fox::Graphics::DirectX::Direct3D& direct3D);
				VOID UpdatePerFrameConstantBuffer(Fox::Graphics::DirectX::Direct3D& direct3D, const PerFrame& data);

				VOID Update(Fox::Graphics::DirectX::Direct3D& direct3D, FLOAT dt);

			private:

				VOID CreateRaytracingInterfaces(const Fox::Graphics::DirectX::Direct3D& direct3D);
				VOID CreateShaderRootSignatures(const Fox::Graphics::DirectX::Direct3D& direct3D);
				VOID CreateRaytracingPipelineStateObject(const Fox::Graphics::DirectX::Direct3D& direct3D);
				VOID CreateDescriptorHeap(const Fox::Graphics::DirectX::Direct3D& direct3D);
				VOID BuildVertexAndIndexBuffers(const Fox::Graphics::DirectX::Direct3D& direct3D);				
				VOID BuildRaytracingAccelerationStructuresForGeometry(Fox::Graphics::DirectX::Direct3D& direct3D);
				VOID CreateConstantBuffers(Fox::Graphics::DirectX::Direct3D& direct3D);
				VOID BuildShaderTables(Fox::Graphics::DirectX::Direct3D& direct3D);
				VOID CreateRaytracingOutputTexture(Fox::Graphics::DirectX::Direct3D& direct3D);

				UINT CreateShaderResourceViewForBuffer(const Fox::Graphics::DirectX::Direct3D& direct3D, Direct3DBuffer* buffer, UINT numElements, UINT elementSize);
				UINT AllocateDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE* cpuDescriptor, UINT descriptorIndexToUse = UINT_MAX);

				VOID CreateLocalRootSignatureSubobjects(CD3DX12_STATE_OBJECT_DESC* raytracingPipelineStateDesc);

				// constant buffers
				union PerFrameConstantBuffer {
					PerFrame perFrameConstantBuffer;
					uint8_t alignmentPadding[D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT];
				}
;
				Microsoft::WRL::ComPtr<ID3D12Resource> perFrameConstants;
				PerFrameConstantBuffer* perFrameConstantData;


				// DirectX Raytracing (DXR) attributes
				Microsoft::WRL::ComPtr<ID3D12Device5> dxrDevice;
				Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList5> dxrCommandList;
				Microsoft::WRL::ComPtr<ID3D12StateObject> dxrPipelineState;

				// Raytracing output
				Microsoft::WRL::ComPtr<ID3D12Resource> raytracingOutputTexture;
				D3D12_GPU_DESCRIPTOR_HANDLE raytracingOutputGPUHandle;
				UINT raytracingOutputResourceUAVDescriptorHeapIndex;

				// Root signatures
				Microsoft::WRL::ComPtr<ID3D12RootSignature> raytracingGlobalRootSignature;
				Microsoft::WRL::ComPtr<ID3D12RootSignature> raytracingLocalRootSignature;

				// Constant buffers
				CubeData cubeConstantBuffer;
				PerFrame perFrameConstantBuffer[Fox::Graphics::Buffering::MAX_NUMBER_OF_BUFFERS];

				// Descriptor heap
				Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
				UINT descriptorSize;
				UINT numDescriptorsAllocated = 0u;

				// Geometry
				Direct3DBuffer indexBuffer;
				Direct3DBuffer vertexBuffer;

				// Acceleration structure
				Microsoft::WRL::ComPtr<ID3D12Resource> bottomLevelAccelerationStructure;
				Microsoft::WRL::ComPtr<ID3D12Resource> topLevelAccelerationStructure;

				// Shader tables
				Microsoft::WRL::ComPtr<ID3D12Resource> missShaderTable;
				Microsoft::WRL::ComPtr<ID3D12Resource> hitGroupShaderTable;
				Microsoft::WRL::ComPtr<ID3D12Resource> rayGenerationShaderTable;
			};
		}
	}
}