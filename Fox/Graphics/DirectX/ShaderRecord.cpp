#include "Fox.h"

namespace Fox {

    namespace Graphics {

        namespace DirectX {

            void ShaderRecord::Copy(void* destination) const
            {
                uint8_t* destinationBytes = static_cast<uint8_t*>(destination);
                memcpy(destinationBytes, shaderIdentifier.ptr, shaderIdentifier.size);
                if (localRootArguments.ptr)
                {
                    memcpy(destinationBytes + shaderIdentifier.size, localRootArguments.ptr, localRootArguments.size);
                }
            }

        }
    }
}