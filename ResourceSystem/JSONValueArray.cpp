#include "JSONValueArray.h"

#include "JSONObject.h"

namespace Fox {

	JSONValueArray::~JSONValueArray() {
		for (size_t i = 0u; i < values.size(); i++) {
			if(values[i])
				delete values[i];
		}
	}

	JSONValue& JSONValueArray::operator[](size_t index) {
		return *values[index];
	}


	std::ostream& operator<<(std::ostream& outputStream, const JSONValueArray& object) {

		size_t elementCount = 0u;

		std::string paddingObject = "";

		for (size_t i = 0u; i < JSONValue::objectDepth; i++) {
			paddingObject += "\t";
		}

		outputStream << "\n" << paddingObject << "[\n";

		for (std::vector<JSONValue*>::const_iterator iter = object.values.begin(); iter != object.values.end(); iter++) {
			
			outputStream << paddingObject << **iter;

			if (elementCount < object.values.size() - 1) {
				outputStream << ",";
			}
			outputStream << "\n";
			elementCount++;
		}

		outputStream << paddingObject << "]";

		return outputStream;
	}

}
