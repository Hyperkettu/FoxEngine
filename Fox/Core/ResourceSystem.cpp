#include "Fox.h"

#include <fstream>
#include <sstream>
#include <iostream>

namespace Fox {

	namespace Core {

		std::string ResourceSystem::ReadFile(const std::string& fileName) {
			std::ifstream file(fileName);
			std::string line;
			std::stringstream stringStream;

			if (file.is_open()) {
				while (std::getline(file, line)) {
					stringStream << line << "\n";
				}
			} else {
				std::cout << "Could not open file " << fileName << std::endl;
			}

			file.close();

			return stringStream.str();
		}

		void ResourceSystem::WriteFile(const std::string& fileName, const std::string& data) {
			std::ofstream file;
			file.open(fileName);

			if (file.is_open()) {
				file << data;
			} else {
				std::cout << "Could not write to file " << fileName << std::endl;
			}
			file.close();
		}

		void ResourceSystem::WriteToJsonFile(const std::string& fileName, const JSONFile& json) {
			std::stringstream stringStream;
			stringStream << json;
			WriteFile(fileName, stringStream.str());
		}

		std::vector<uint8_t> ResourceSystem::ReadBinaryFile(const std::string fileName)
		{
			std::vector<uint8_t> data;

			std::ifstream fs;
			fs.open(fileName, std::ifstream::in | std::ifstream::binary);
			if (fs.good())
			{
				auto size = ResourceSystem::GetFileSize(fileName);
				// TODO: check here if size is more than size_t
				data.resize(static_cast<size_t>(size));
				fs.seekg(0, std::ios::beg);
				fs.read(reinterpret_cast<char*>(&data[0]), size);
				fs.close();
			}
			return data;
		}

		uintmax_t ResourceSystem::GetFileSize(std::string fileName)
		{
			std::ifstream in(fileName, std::ifstream::ate | std::ifstream::binary);
			return in.tellg();
		}
	}
}