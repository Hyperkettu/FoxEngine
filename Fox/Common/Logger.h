#pragma once

#include <string>

namespace Fox {

	class FOX_API Logger {
	public:
		Logger();
		~Logger();

		static Logger* Instance() { return instance; }
		static VOID PrintLog(const WCHAR* format, ...);
		static std::wstring LogDirectory();
		static std::wstring LogFile();
	private:
		static Logger* instance;
	};

}