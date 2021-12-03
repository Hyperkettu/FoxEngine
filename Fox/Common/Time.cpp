#include "Fox.h"

#include <ctime>
#include <sstream>
#include <iomanip>

namespace Fox {

	namespace Time {

		std::wstring GetTime(BOOL stripped) {
			time_t now = time(nullptr);
			tm ltm;
			localtime_s(&ltm, &now);

			std::wstringstream wss;
			wss << std::put_time(&ltm, L"%T");

			std::wstring timeString = wss.str();

			if (stripped) {
				std::wstring chars = L":";
				for (WCHAR ch : chars) {
					timeString.erase(std::remove(timeString.begin(), timeString.end(), ch), timeString.end());
				}
			}
			return timeString;
		}

		std::wstring GetDate(BOOL stripped) {
			time_t now = time(nullptr);
			tm ltm;
			localtime_s(&ltm, &now);

			std::wstringstream wss;
			wss << std::put_time(&ltm, L"%d/%m/%y");

			std::wstring timeString = wss.str();

			if (stripped) {
				std::wstring chars = L"/";
				for (WCHAR ch : chars) {
					timeString.erase(std::remove(timeString.begin(), timeString.end(), ch), timeString.end());
				}
			}
			return timeString;
		}

		std::wstring GetDateTimeString(BOOL stripped) {

			std::wstring timeString = GetDate(stripped) + L" " + GetTime(stripped);

			if (stripped) {
				std::wstring chars = L" ";
				for (WCHAR ch : chars) {
					timeString.erase(std::remove(timeString.begin(), timeString.end(), ch), timeString.end());
				}
			}
			return timeString;
		}
	}
}