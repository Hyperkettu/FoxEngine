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

			VOID ThrowIfFailed(HRESULT hr, const wchar_t* msg)
			{
				if (FAILED(hr))
				{
					Logger::PrintLog(msg);
					throw HRException(hr);
				}
			}

			VOID ThrowIfFalse(bool value)
			{
				ThrowIfFailed(value ? S_OK : E_FAIL);
			}

			VOID ThrowIfFalse(bool value, const wchar_t* msg)
			{
				ThrowIfFailed(value ? S_OK : E_FAIL, msg);
			}

			UINT Align(UINT size, UINT alignment) {
				return (size + (alignment - 1)) & ~(alignment - 1);
			}
		}
	}
}