#pragma once

#include <vector>
#include <cassert>

#include "Fox/Core/JSONObject.h"
#include "Fox/Core/JSONValueArray.h"
#include "Fox/Core/TypedValue.h"

namespace Fox {

	class FOX_API JSONFile {

	public: 
		JSONFile() {}
		~JSONFile();

		template<class ValueType, typename std::enable_if<std::is_base_of<JSONValue, ValueType>::value>::type* = nullptr>
		void setRoot(ValueType& value) {
			root = &value;
		}

		template<class U>
		U& get() {
			U* ptr = dynamic_cast<U*>(root);
			return *ptr;
		}

		void parse(std::string& file);

		friend std::ostream& operator<<(std::ostream& outputStream, const JSONFile& object);

	private: 
		void parseValue(JSONObject* parent, std::string& file);
		bool parseBoolValue(JSONObject* object, std::string& attributeName, size_t comma, size_t endOfObject, std::string& file);
		bool parseNumberValue(JSONObject* object, const std::string& attributeName, size_t comma, size_t endOfObject, std::string& file);
		bool parseStringValue(JSONObject* object, const std::string& attributeName, size_t comma, size_t endOfObject, std::string& file);
		bool parseArray(JSONValueArray* array, std::string& file);
		bool parseIndexData(std::vector<JSONValue*>& array, std::string& indexData);

		JSONValue* root;
	};
}