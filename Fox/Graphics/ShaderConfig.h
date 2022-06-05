#pragma once

#include <vector>

namespace Fox {

	class FOX_API ShaderConfig {

	public:
		enum DescriptorTableType {
			CONSTANT_BUFFER = 0,
			NUM_DESCRIPTOR_TABLES
		};

		struct FOX_API DescriptorTableConfig {
			std::string name;
			DescriptorTableType type;
		};

		ShaderConfig() = default;
		ShaderConfig(const std::string& configFile);
		~ShaderConfig();

		static DescriptorTableType getType(const std::string& type);

		INT resolveFile();

		std::string name;
		std::vector<DescriptorTableConfig> descriptorTableConfigs;
		UINT numDescriptorTables = 0u;

	private:
		JSONFile* sourceFile;
		std::string fileName;
		std::string shaderSourceFile;
	};

}