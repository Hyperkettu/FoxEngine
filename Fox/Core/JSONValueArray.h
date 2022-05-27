#pragma once

#include "Fox/Core/JSONValue.h"

#include <vector>

namespace Fox {

	class FOX_API JSONValueArray : public JSONValue {
	public:
		JSONValueArray() = default;
		~JSONValueArray();

		void fromArray(JSONValue* array[], size_t arrayLength) {
			for (size_t i = 0u; i < arrayLength; i++) {
				values.push_back(array[i]);
			}
		}

		void set(size_t index, JSONValue& value) {
			values[index] = &value;
		}

		template<class U>
		U& get(size_t index) {
			U* value = dynamic_cast<U*>(values[index]);
			if (value) {
				return *value;
			}
			else {
				U dummy;
				return dummy;
			}
		}

		JSONValue& operator[](size_t index);

		friend std::ostream& operator<<(std::ostream& outputStream, const JSONValueArray& object);

	private:
		std::vector<JSONValue*> values;
	};
}