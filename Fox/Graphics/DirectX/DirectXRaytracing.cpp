#include "Fox.h"

namespace Fox {
	
	namespace Graphics {
	
		namespace DirectX {

			// initialize the raytracing pipeline
			VOID DirectXRaytracing::CreateDeviceDependentResources(Fox::Graphics::DirectX::Direct3D& direct3D) {
				CreateRaytracingInterfaces(direct3D);
				CreateShaderRootSignatures(direct3D);
				CreateRaytracingPipelineStateObject(direct3D);
				CreateDescriptorHeap(direct3D);
				BuildVertexAndIndexBuffers(direct3D);
				BuildRaytracingAccelerationStructuresForGeometry(direct3D);
				CreateConstantBuffers(direct3D);
				BuildShaderTables(direct3D);
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

			//Create a raytracing pipeline state object(RTPSO).
			// An RTPSO represents a full set of shaders reachable by a DispatchRays() call,
			// with all configuration options resolved, such as local signatures and other state.
			VOID DirectXRaytracing::CreateRaytracingPipelineStateObject(const Fox::Graphics::DirectX::Direct3D& direct3D) {
				CD3DX12_STATE_OBJECT_DESC raytracingPipelineStateDesc = { D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE };
				CD3DX12_DXIL_LIBRARY_SUBOBJECT* dxilLibrary = raytracingPipelineStateDesc.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
				std::vector<uint8_t> shaderBytes = Fox::Core::ResourceSystem::ReadBinaryFile("Assets\\Shaders\\PathTracer.cso");
				D3D12_SHADER_BYTECODE shaderByteCode = CD3DX12_SHADER_BYTECODE(static_cast<void*>(&shaderBytes[0]), shaderBytes.size());
				dxilLibrary->SetDXILLibrary(&shaderByteCode);

				{
					dxilLibrary->DefineExport(L"MyRayGenerationShader");
					dxilLibrary->DefineExport(L"MyClosestHitShader");
					dxilLibrary->DefineExport(L"MyMissShader");
				}

				CD3DX12_HIT_GROUP_SUBOBJECT* hitGroup = raytracingPipelineStateDesc.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
				hitGroup->SetClosestHitShaderImport(L"MyClosestHitShader");
				hitGroup->SetHitGroupExport(L"MyHitGroup");
				hitGroup->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);

				CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT* shaderConfig = raytracingPipelineStateDesc.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
				UINT payloadSize = sizeof(XMFLOAT4);    // float4 pixelColor
				UINT attributeSize = sizeof(XMFLOAT2);  // float2 barycentrics
				shaderConfig->Config(payloadSize, attributeSize);

				CreateLocalRootSignatureSubobjects(&raytracingPipelineStateDesc);

				CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT* globalRootSignature = raytracingPipelineStateDesc.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
				globalRootSignature->SetRootSignature(raytracingGlobalRootSignature.Get());

				CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT* pipelineConfig = raytracingPipelineStateDesc.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
				UINT maxRecursionDepth = 1;
				pipelineConfig->Config(maxRecursionDepth);

#if _DEBUG
				Fox::Graphics::DirectX::PrintPipelineStateObjectDesc(raytracingPipelineStateDesc);
#endif

				ThrowIfFailed(dxrDevice->CreateStateObject(raytracingPipelineStateDesc, IID_PPV_ARGS(&dxrPipelineState)), 
					L"Couldn't create DirectX Raytracing pipeline state object.\n");

#ifdef _DEBUG
				Logger::PrintLog(L"Created raytracing pipeline state object successfully.\n");
#endif
			}

			VOID DirectXRaytracing::CreateLocalRootSignatureSubobjects(CD3DX12_STATE_OBJECT_DESC* raytracingPipelineStateDesc) {
				CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT* localRootSignature = raytracingPipelineStateDesc->CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
				localRootSignature->SetRootSignature(raytracingLocalRootSignature.Get());

				{
					CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT* rootSignatureAssociation = 
						raytracingPipelineStateDesc->CreateSubobject<CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
					rootSignatureAssociation->SetSubobjectToAssociate(*localRootSignature);
					rootSignatureAssociation->AddExport(L"MyHitGroup");
				}
			}

			VOID DirectXRaytracing::CreateDescriptorHeap(const Fox::Graphics::DirectX::Direct3D& direct3D) {
				auto device = direct3D.GetDirect3DDevice();

				D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
				// Allocate a heap for 3 descriptors:
				// 2 - vertex and index buffer SRVs
				// 1 - raytracing output texture SRV
				descriptorHeapDesc.NumDescriptors = 3;
				descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
				descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
				descriptorHeapDesc.NodeMask = 0;
				device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
				descriptorHeap->SetName(L"Shader Resources Descritor Heap"); // TODO don't name when Relase version

				descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

#ifdef _DEBUG
				Logger::PrintLog(L"Created shader resources descriptor heap successfully.\n");
#endif
			}

			VOID DirectXRaytracing::BuildVertexAndIndexBuffers(const Fox::Graphics::DirectX::Direct3D& direct3D) {

				auto device = direct3D.GetDirect3DDevice();

				// Cube indices.
				Index indices[] =
				{
					3,1,0,
					2,1,3,

					6,4,5,
					7,4,6,

					11,9,8,
					10,9,11,

					14,12,13,
					15,12,14,

					19,17,16,
					18,17,19,

					22,20,21,
					23,20,22
				};

				// Cube vertices positions and corresponding triangle normals.
				Vertex vertices[] =
				{
					{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
					{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
					{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
					{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },

					{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
					{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
					{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
					{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },

					{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
					{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
					{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
					{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },

					{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
					{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
					{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
					{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },

					{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
					{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
					{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
					{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },

					{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
					{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
					{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
					{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
				};

				AllocateUploadBuffer(device, indices, sizeof(indices), &indexBuffer.resource);
				AllocateUploadBuffer(device, vertices, sizeof(vertices), &vertexBuffer.resource);

				// Vertex buffer is passed to the shader along with index buffer as a descriptor table.
				// Vertex buffer descriptor must follow index buffer descriptor in the descriptor heap.
				UINT descriptorIndexIB = CreateShaderResourceViewForBuffer(direct3D, &indexBuffer, sizeof(indices) / 4, 0);
				UINT descriptorIndexVB = CreateShaderResourceViewForBuffer(direct3D, &vertexBuffer, ARRAYSIZE(vertices), sizeof(vertices[0]));
				ThrowIfFalse(descriptorIndexVB == descriptorIndexIB + 1, L"Vertex Buffer descriptor index must follow that of Index Buffer descriptor index!");

#ifdef _DEBUG
				Logger::PrintLog(L"Built geometry successfully.\n");
#endif // _DEBUG

			}

			UINT DirectXRaytracing::CreateShaderResourceViewForBuffer(const Fox::Graphics::DirectX::Direct3D& direct3D, Direct3DBuffer* buffer, UINT numElements, UINT elementSize) {
				auto device = direct3D.GetDirect3DDevice();
		
				D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
				srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				srvDesc.Buffer.NumElements = numElements;

				if (elementSize == 0) {
					srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
					srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
					srvDesc.Buffer.StructureByteStride = 0;
				} else {
					srvDesc.Format = DXGI_FORMAT_UNKNOWN;
					srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
					srvDesc.Buffer.StructureByteStride = elementSize;
				}
				UINT descriptorIndex = AllocateDescriptor(&buffer->handleForCPU);
				device->CreateShaderResourceView(buffer->resource.Get(), &srvDesc, buffer->handleForCPU);
				buffer->handleForGPU = CD3DX12_GPU_DESCRIPTOR_HANDLE(descriptorHeap->GetGPUDescriptorHandleForHeapStart(), descriptorIndex, descriptorSize);

#ifdef _DEBUG
				Logger::PrintLog(L"Created Shader Resource View for buffer successfully.\n");
#endif // _DEBUG

				return descriptorIndex;
			}

			// Allocate a descriptor and return its index. 
			// If the passed descriptorIndexToUse is valid, it will be used instead of allocating a new one.
			UINT DirectXRaytracing::AllocateDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE* cpuDescriptor, UINT descriptorIndex)
			{
				auto descriptorHeapCpuHeapStart = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
				if (descriptorIndex >= descriptorHeap->GetDesc().NumDescriptors)
				{
					descriptorIndex = numDescriptorsAllocated++;
				}
				*cpuDescriptor = CD3DX12_CPU_DESCRIPTOR_HANDLE(descriptorHeapCpuHeapStart, descriptorIndex, descriptorSize);

#ifdef _DEBUG
				Logger::PrintLog(L"Allocated descriptor successfully.\n");
#endif // _DEBUG

				return descriptorIndex;
			}

			VOID DirectXRaytracing::BuildRaytracingAccelerationStructuresForGeometry(Fox::Graphics::DirectX::Direct3D& direct3D) {
				auto device = direct3D.GetDirect3DDevice();
				auto commandList = direct3D.GetMainCommandList();
				auto commandQueue = direct3D.GetMainCommandQueue();
				auto commandAllocator = direct3D.GetCurrentCommandAllocator();

				commandList->Reset(commandAllocator, nullptr);

				D3D12_RAYTRACING_GEOMETRY_DESC geometryDesc = {};
				geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
				geometryDesc.Triangles.IndexBuffer = indexBuffer.resource->GetGPUVirtualAddress();
				geometryDesc.Triangles.IndexCount = static_cast<UINT>(indexBuffer.resource->GetDesc().Width) / sizeof(Index);
				geometryDesc.Triangles.IndexFormat = DXGI_FORMAT_R16_UINT;
				geometryDesc.Triangles.Transform3x4 = 0;
				geometryDesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
				geometryDesc.Triangles.VertexCount = static_cast<UINT>(vertexBuffer.resource->GetDesc().Width) / sizeof(Vertex);
				geometryDesc.Triangles.VertexBuffer.StartAddress = vertexBuffer.resource->GetGPUVirtualAddress();
				geometryDesc.Triangles.VertexBuffer.StrideInBytes = sizeof(Vertex);

				geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;

				// Get required sizes for an acceleration structure.
				D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

				D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC bottomLevelBuildDesc = {};
				D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& bottomLevelInputs = bottomLevelBuildDesc.Inputs;
				bottomLevelInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
				bottomLevelInputs.Flags = buildFlags;
				bottomLevelInputs.NumDescs = 1;
				bottomLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
				bottomLevelInputs.pGeometryDescs = &geometryDesc;

				D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC topLevelBuildDesc = {};
				D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& topLevelInputs = topLevelBuildDesc.Inputs;
				topLevelInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
				topLevelInputs.Flags = buildFlags;
				topLevelInputs.NumDescs = 1;
				topLevelInputs.pGeometryDescs = nullptr;
				topLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;

				D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO topLevelPrebuildInfo = {};
				dxrDevice->GetRaytracingAccelerationStructurePrebuildInfo(&topLevelInputs, &topLevelPrebuildInfo);
				ThrowIfFalse(topLevelPrebuildInfo.ResultDataMaxSizeInBytes > 0);

				D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO bottomLevelPrebuildInfo = {};
				dxrDevice->GetRaytracingAccelerationStructurePrebuildInfo(&bottomLevelInputs, &bottomLevelPrebuildInfo);
				ThrowIfFalse(bottomLevelPrebuildInfo.ResultDataMaxSizeInBytes > 0);

				Microsoft::WRL::ComPtr<ID3D12Resource> scratchResource;
				Fox::Graphics::DirectX::AllocateUnorderedAccessViewBuffer(device, max(topLevelPrebuildInfo.ScratchDataSizeInBytes, 
					bottomLevelPrebuildInfo.ScratchDataSizeInBytes), &scratchResource, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, L"ScratchResource");

				// Allocate resources for acceleration structures.
				// Acceleration structures can only be placed in resources that are created in the default heap (or custom heap equivalent). 
				// Default heap is OK since the application doesn’t need CPU read/write access to them. 
				// The resources that will contain acceleration structures must be created in the state D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, 
				// and must have resource flag D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS. The ALLOW_UNORDERED_ACCESS requirement simply acknowledges both: 
				//  - the system will be doing this type of access in its implementation of acceleration structure builds behind the scenes.
				//  - from the app point of view, synchronization of writes/reads to acceleration structures is accomplished using UAV barriers.
				{
					D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;

					Fox::Graphics::DirectX::AllocateUnorderedAccessViewBuffer(device, bottomLevelPrebuildInfo.ResultDataMaxSizeInBytes, 
						&bottomLevelAccelerationStructure, initialResourceState, L"BottomLevelAccelerationStructure");
					Fox::Graphics::DirectX::AllocateUnorderedAccessViewBuffer(device, topLevelPrebuildInfo.ResultDataMaxSizeInBytes, 
						&topLevelAccelerationStructure, initialResourceState, L"TopLevelAccelerationStructure");
				}

				// Create an instance desc for the bottom-level acceleration structure.
				Microsoft::WRL::ComPtr<ID3D12Resource> instanceDescs;
				D3D12_RAYTRACING_INSTANCE_DESC instanceDesc = {};
				instanceDesc.Transform[0][0] = instanceDesc.Transform[1][1] = instanceDesc.Transform[2][2] = 1;
				instanceDesc.InstanceMask = 1;
				instanceDesc.AccelerationStructure = bottomLevelAccelerationStructure->GetGPUVirtualAddress();
				AllocateUploadBuffer(device, &instanceDesc, sizeof(instanceDesc), &instanceDescs, L"InstanceDescs");

				// Bottom Level Acceleration Structure desc
				{
					bottomLevelBuildDesc.ScratchAccelerationStructureData = scratchResource->GetGPUVirtualAddress();
					bottomLevelBuildDesc.DestAccelerationStructureData = bottomLevelAccelerationStructure->GetGPUVirtualAddress();
				}

				// Top Level Acceleration Structure desc
				{
					topLevelBuildDesc.DestAccelerationStructureData = topLevelAccelerationStructure->GetGPUVirtualAddress();
					topLevelBuildDesc.ScratchAccelerationStructureData = scratchResource->GetGPUVirtualAddress();
					topLevelBuildDesc.Inputs.InstanceDescs = instanceDescs->GetGPUVirtualAddress();
				}

				auto BuildAccelerationStructure = [&](auto* raytracingCommandList)
				{
					CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::UAV(bottomLevelAccelerationStructure.Get());
					raytracingCommandList->BuildRaytracingAccelerationStructure(&bottomLevelBuildDesc, 0, nullptr);
					commandList->ResourceBarrier(1, &barrier);
					raytracingCommandList->BuildRaytracingAccelerationStructure(&topLevelBuildDesc, 0, nullptr);
				};

				// Build acceleration structure.
				BuildAccelerationStructure(dxrCommandList.Get());

				// Kick off acceleration structure construction.
				direct3D.ExecuteMainCommandList();

				// Wait for GPU to finish as the locally created temporary GPU resources will get released once we go out of scope.
				direct3D.WaitForGpu();

#ifdef _DEBUG
				Logger::PrintLog(L"Created acceleration strcture for geometry successfully.\n");
#endif
			}

			VOID DirectXRaytracing::CreateConstantBuffers(Fox::Graphics::DirectX::Direct3D& direct3D) { 
				auto device = direct3D.GetDirect3DDevice();
				UINT frameCount = direct3D.GetBackBufferCount();

				// Create the constant buffer memory and map the CPU and GPU addresses
				const D3D12_HEAP_PROPERTIES uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

				// Allocate one constant buffer per frame, since it gets updated every frame.
				size_t cbSize = frameCount * sizeof(PerFrameConstantBuffer);
				const D3D12_RESOURCE_DESC constantBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(cbSize);

				ThrowIfFailed(device->CreateCommittedResource(
					&uploadHeapProperties,
					D3D12_HEAP_FLAG_NONE,
					&constantBufferDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&perFrameConstants)));

				// Map the constant buffer and cache its heap pointers.
				// We don't unmap this until the app closes. Keeping buffer mapped for the lifetime of the resource is okay.
		        // We do not intend to read from this resource on the CPU.
				ThrowIfFailed(perFrameConstants->Map(0, nullptr, reinterpret_cast<void**>(&perFrameConstantData)));

#ifdef _DEBUG
				Logger::PrintLog(L"Created constant buffers successfully.\n");
#endif
			}

			// build shader tables that define root parameters
			VOID DirectXRaytracing::BuildShaderTables(Fox::Graphics::DirectX::Direct3D& direct3D) {
				auto device = direct3D.GetDirect3DDevice();

				void* rayGenerationShaderIdentifier;
				void* missShaderIdentifier;
				void* hitGroupShaderIdentifier;

				auto GetShaderIdentifiers = [&](auto* stateObjectProperties)
				{
					rayGenerationShaderIdentifier = stateObjectProperties->GetShaderIdentifier(L"MyRayGenerationShader");
					missShaderIdentifier = stateObjectProperties->GetShaderIdentifier(L"MyMissShader");
					hitGroupShaderIdentifier = stateObjectProperties->GetShaderIdentifier(L"MyHitGroup");
				};

				// Get shader identifiers.
				UINT shaderIdentifierSize;
				{
					Microsoft::WRL::ComPtr<ID3D12StateObjectProperties> stateObjectProperties;
					ThrowIfFailed(dxrPipelineState.As(&stateObjectProperties));
					GetShaderIdentifiers(stateObjectProperties.Get());
					shaderIdentifierSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
				}

				// Ray gen shader table
				{
					UINT numShaderRecords = 1;
					UINT shaderRecordSize = shaderIdentifierSize;
					Fox::Graphics::DirectX::ShaderTable rayGenerationShaderTable(device, numShaderRecords, shaderRecordSize, L"RayGenerationShaderTable");
					rayGenerationShaderTable.AddRecord(Fox::Graphics::DirectX::ShaderRecord(rayGenerationShaderIdentifier, shaderIdentifierSize));
					this->rayGenerationShaderTable = rayGenerationShaderTable.GetResource();
				}

				// Miss shader table
				{
					UINT numShaderRecords = 1;
					UINT shaderRecordSize = shaderIdentifierSize;
					Fox::Graphics::DirectX::ShaderTable missShaderTable(device, numShaderRecords, shaderRecordSize, L"MissShaderTable");
					missShaderTable.AddRecord(Fox::Graphics::DirectX::ShaderRecord(missShaderIdentifier, shaderIdentifierSize));
					this->missShaderTable = missShaderTable.GetResource();
				}

				// Hit group shader table
				{
					struct RootArguments {
						CubeData material;
					} rootArguments;
					rootArguments.material = cubeConstantBuffer;

					UINT numShaderRecords = 1;
					UINT shaderRecordSize = shaderIdentifierSize + sizeof(rootArguments);
					Fox::Graphics::DirectX::ShaderTable hitGroupShaderTable(device, numShaderRecords, shaderRecordSize, L"HitGroupShaderTable");
					hitGroupShaderTable.AddRecord(Fox::Graphics::DirectX::ShaderRecord(hitGroupShaderIdentifier, shaderIdentifierSize, &rootArguments, sizeof(rootArguments)));
					this->hitGroupShaderTable = hitGroupShaderTable.GetResource();
				}

#ifdef _DEBUG
				Logger::PrintLog(L"Built shader tables successfully.\n");
#endif
			}



		}
	}
}