#pragma once

#include "JSONValue.h"

#include <vector>

namespace Fox {

	class JSONValueArray : public JSONValue {
	public:
		JSONValueArray() = default;

		void fromArray(JSONValue* array[], size_t arrayLength) {
			for (size_t i = 0u; i < arrayLength; i++) {
				values.push_back(array[i]);
			}
		}

		void set(size_t index, JSONValue& value) {
			values[index] = &value;
		}

		JSONValue& operator[](size_t index);

		friend std::ostream& operator<<(std::ostream& outputStream, const JSONValueArray& object);

	private:
		std::vector<JSONValue*> values;
	};
}