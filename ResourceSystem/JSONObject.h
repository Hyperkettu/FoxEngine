#pragma once

#include <map>
#include <string>

#include "JSONValue.h"

namespace Fox { 

	class JSONObject: public JSONValue {
	public:

		JSONObject() = default;
		JSONObject(JSONObject& object) {
			attributes = object.attributes;
		}
		virtual ~JSONObject() = default;

		void set(const std::string& key, JSONValue& value) {
			attributes[key] = &value;
		}

		JSONValue& operator[](std::string& key);

		friend std::ostream& operator<<(std::ostream& outputStream, const JSONObject& object);

	public:
		std::map<std::string, JSONValue*> attributes;
	};

}

