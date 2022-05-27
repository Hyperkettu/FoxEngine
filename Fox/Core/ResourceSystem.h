#pragma once

#include <string>

#include "Fox/Core/JSONFile.h"


namespace Fox {

	class FOX_API ResourceSystem {
	public: 
		static std::string readFile(const std::string& fileName);
		static void writeFile(const std::string& fileName, const std::string& data);
		static void writeToJsonFile(const std::string& fileName, const JSONFile& json);
	};

}