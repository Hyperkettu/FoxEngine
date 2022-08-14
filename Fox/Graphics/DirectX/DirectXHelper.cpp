#include "Fox.h"

namespace Fox {

	namespace Graphics {
	
		namespace DirectX {

			VOID ThrowIfFailed(HRESULT hr)
			{
				if (FAILED(hr))
				{
					throw HRException(hr);
				}
			}

			inline void ThrowIfFailed(HRESULT hr, const wchar_t* msg)
			{
				if (FAILED(hr))
				{
					Logger::PrintLog(msg);
					throw HRException(hr);
				}
			}
		}
	}
}