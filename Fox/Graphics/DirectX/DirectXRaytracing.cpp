#include "Fox.h"

namespace Fox {
	
	namespace Graphics {
	
		namespace DirectX {

			// initialize the raytracing pipeline
			VOID DirectXRaytracing::CreateDeviceDependentResources(const Fox::Graphics::DirectX::Direct3D& direct3D) {
				CreateRaytracingInterfaces(direct3D);
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
		}
	}
}