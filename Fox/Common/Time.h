#pragma once 

namespace Fox {

	namespace Time {

		std::wstring FOX_API GetTime(BOOL stripped = FALSE);
		std::wstring FOX_API GetDate(BOOL stripped = FALSE);
		std::wstring FOX_API GetDateTimeString(BOOL stripped = FALSE);
	}

}