#pragma once

#include <string>

#include "JSONFile.h"


namespace Fox {

	class ResourceSystem {
	public: 
		static std::string readFile(const std::string& fileName);
		static void writeFile(const std::string& fileName, const std::string& data);
		static void writeToJsonFile(const std::string& fileName, const JSONFile& json);
	};

}