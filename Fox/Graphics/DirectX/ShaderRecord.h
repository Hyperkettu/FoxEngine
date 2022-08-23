#pragma once 

namespace Fox {

	namespace Graphics {
	
		namespace DirectX {
		
			class FOX_API ShaderRecord {

            public:

                struct SizedPointer {
                    SizedPointer() : ptr(nullptr), size(0) {}
                    SizedPointer(void* ptr, UINT size) : ptr(ptr), size(size) {};

                    void* ptr;
                    UINT size;
                };

                ShaderRecord(void* pShaderIdentifier, UINT shaderIdentifierSize) :
                    shaderIdentifier(pShaderIdentifier, shaderIdentifierSize) { }

                ShaderRecord(void* pShaderIdentifier, UINT shaderIdentifierSize, void* pLocalRootArguments, UINT localRootArgumentsSize) :
                    shaderIdentifier(pShaderIdentifier, shaderIdentifierSize),
                    localRootArguments(pLocalRootArguments, localRootArgumentsSize) { }

                void Copy(void* destination) const;

                SizedPointer shaderIdentifier;
                SizedPointer localRootArguments;
			};
		}
	}
}