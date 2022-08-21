#include "Fox.h"

namespace Fox {

	ShaderConfig::ShaderConfig(const std::string& configFile) : fileName(configFile) {
	}

	ShaderConfig::~ShaderConfig() {
		SAFE_DELETE(sourceFile);
	}

	INT ShaderConfig::resolveFile() {
		std::string data = Fox::Core::ResourceSystem::ReadFile(fileName);
		sourceFile = new JSONFile;
		sourceFile->parse(data);

		JSONObject& root = sourceFile->get<JSONObject>();
		assert(&root != nullptr, "JSON File error: No root specified.");

		if (!&root)
			return -1;

		name = root.get<StringValue>("shaderName").value;
		assert(name.length() > 0, "JSON File error: No shader name specified.");

		shaderSourceFile = root.get<StringValue>("source").value;
		assert(name.length() > 0, "JSON File error: No shader source file specified.");

		numDescriptorTables = root.get<IntValue>("numDescriptorTables").getValue();
		descriptorTableConfigs = std::vector<DescriptorTableConfig>(numDescriptorTables);

		JSONValueArray& tables = root.get<JSONValueArray>("descriptorTables");
		assert(&tables != nullptr, "JSON File error: No descriptor tables defined.");

		if (!&tables)
			return -1;

		for (UINT i = 0u; i < numDescriptorTables; i++) {
			JSONObject& table = tables.get<JSONObject>(i);
			assert(&table != nullptr, "JSON File error: No table found.");
			std::string& tableName = table.get<StringValue>("name").value;
			assert(tableName.length() > 0, "JSON File error: No descriptor table name.");
			std::string& tableType = table.get<StringValue>("type").value;
			UINT bindIndex = table.get<IntValue>("bindIndex").getValue();
			DescriptorTableConfig tableConfig = {
				tableName,
				ShaderConfig::getType(tableType)
			};

			descriptorTableConfigs[bindIndex] = tableConfig;
		}

		return 1;
	}

	ShaderConfig::DescriptorTableType ShaderConfig::getType(const std::string& type) {
		if (type == "constant-buffer") {
			return DescriptorTableType::CONSTANT_BUFFER;
		}
		return DescriptorTableType::NUM_DESCRIPTOR_TABLES;
	}



}