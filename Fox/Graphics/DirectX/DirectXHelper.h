#pragma once 

namespace Fox {

	namespace Graphics {
	
		namespace DirectX {
	
            class FOX_API HRException /* : public std::runtime_error */
            {
                inline std::string HRToString(HRESULT hr)
                {
                    char s_str[64] = {};
                    sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
                    return std::string(s_str);
                }
            public:
                HRException(HRESULT HR) : /*std::runtime_error(HRToString(hr)),*/ hr(HR) {}
                HRESULT Error() const { return hr; }
            private:
                const HRESULT hr;
            };

            VOID FOX_API ThrowIfFailed(HRESULT hr);
            VOID FOX_API ThrowIfFailed(HRESULT hr, const wchar_t* message);
		
		}
	}
}