#include "pch.h"
#include "ResourceSystem.h"

#include <fstream>
#include <sstream>
#include <iostream>

namespace Fox {

	std::string ResourceSystem::readFile(const std::string& fileName) {
		std::ifstream file(fileName);
		std::string line;
		std::stringstream stringStream;

		if (file.is_open()) {
			while (std::getline(file, line)) {
				stringStream << line << "\n";
			}
		}
		else {
			std::cout << "Could not open file " << fileName << std::endl;
		}

		file.close();

		return stringStream.str();
	}

	void ResourceSystem::writeFile(const std::string& fileName, const std::string& data) {
		std::ofstream file;
		file.open(fileName);

		if (file.is_open()) {
			file << data;
		}
		else {
			std::cout << "Could not write to file " << fileName << std::endl;
		}
		file.close();
	}

	void ResourceSystem::writeToJsonFile(const std::string& fileName, const JSONFile& json) {
		std::stringstream stringStream;
		stringStream << json;
		writeFile(fileName, stringStream.str());
	}


}