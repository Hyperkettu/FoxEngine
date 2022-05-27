#pragma once

#include <map>
#include <string>

#include "Fox/Core/JSONValue.h"

namespace Fox { 

	class FOX_API JSONObject: public JSONValue {
	public:

		JSONObject() = default;
		JSONObject(JSONObject& object) {
			attributes = object.attributes;
		}
		virtual ~JSONObject();

		void set(const std::string& key, JSONValue* value) {
			attributes[key] = value;
		}

		JSONValue& operator[](std::string& key);

		JSONObject& get() {
			return *this;
		}

		template<class U>
		U& get(const char* key) {
			const std::string str(key);			
			U* typedValuePtr = dynamic_cast<U*>(attributes[str]);
			if (typedValuePtr) {
				return *typedValuePtr;
			}	
			else {
				U value;
				return value;
			}
		
		}

		friend std::ostream& operator<<(std::ostream& outputStream, const JSONObject& object);

	private:
		std::map<std::string, JSONValue*> attributes;
	};

}

