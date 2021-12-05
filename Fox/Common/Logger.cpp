#include "Fox.h"

#include <fstream>
#include <ShlObj.h>

namespace Fox {

	Logger* Logger::instance;

	Logger::Logger() {
		instance = this;
	}

	Logger::~Logger() {
	}

	VOID Logger::PrintLog(const WCHAR* format, ...) {
		WCHAR buffer[4096];
		va_list args;

		va_start(args, format);
		vswprintf_s(buffer, format, args);
		va_end(args);

		OutputDebugString(buffer);

		std::wfstream file;
		file.open(std::wstring(LogDirectory() + L"//" + LogFile()), std::ios_base::app);
		
		if (file.is_open()) {
			std::wstring s = buffer;
			file << L"[" << Time::GetDateTimeString() + L"] " << s;
			file.close();
			OutputDebugString(s.c_str());
		}
		else {
			MessageBox(nullptr, L"Unable to open log file...", L"Log Error", MB_OK);
		}
	}

	VOID Logger::PrintDebugSeparator()
	{
		std::wstring line = L"\n---------------------------------------------------------------------------\n\n";

#ifdef _DEBUG
		std::wfstream file;
		file.open(std::wstring(LogDirectory() + L"//" + LogFile()), std::ios_base::app);

		if (file.is_open()) {
			file << line;
			file.close();
		}
		else {
			MessageBox(nullptr, L"Unable to open log file...", L"Log Error", MB_OK);
		}
#endif // _DEBUG
	}

	std::wstring Logger::LogDirectory()	{
		WCHAR path[1024];
		WCHAR* appDataLocalPath;

		SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &appDataLocalPath);
		wcscpy_s(path, appDataLocalPath);
		wcscat_s(path, L"\\");
		wcscat_s(path, GameSettings::GameName());
		CreateDirectory(path, nullptr);
		wcscat_s(path, L"\\Log");
		CreateDirectory(path, nullptr);
		return path;
	}

	std::wstring Logger::LogFile() {
		WCHAR fileName[1024];
		wcscpy_s(fileName, GameSettings::GameName());
		wcscat_s(fileName, GameSettings::BootTime());
		wcscat_s(fileName, L".log");
		return fileName;
	}
}