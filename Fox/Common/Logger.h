#pragma once

#include <string>

namespace Fox {

	class FOX_API Logger {
	public:
		Logger();
		~Logger();

		static Logger* Instance() { return instance; }
		static VOID PrintLog(const WCHAR* format, ...);
		static VOID PrintDebugSeparator();
		static std::wstring LogDirectory();
		static std::wstring LogFile();
		static void stringToWString(std::wstring& ws, const std::string& s) {
			std::wstring wsTmp(s.begin(), s.end());
			ws = wsTmp;
		}
		
	private:
		static Logger* instance;
	};

}