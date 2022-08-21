#pragma once

#include <string>

#include "Fox/Core/JSONFile.h"


namespace Fox {

	namespace Core {

		class FOX_API ResourceSystem {
		public:
			static std::string ReadFile(const std::string& fileName);
			static std::vector<uint8_t> ReadBinaryFile(const std::string filename);
			static void WriteFile(const std::string& fileName, const std::string& data);
			static void WriteToJsonFile(const std::string& fileName, const JSONFile& json);

			static uintmax_t GetFileSize(std::string fileName);
		};
	}
}