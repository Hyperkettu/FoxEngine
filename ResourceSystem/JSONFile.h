#pragma once

#include <vector>
#include <cassert>

#include "JSONObject.h"
#include "TypedValue.h"
#include "JSONValueArray.h"

namespace Fox {

	class JSONFile {

	public: 
		JSONFile() {}

		template<class ValueType>
		void setRoot(ValueType& value) {
			root = &value;
		}

		friend std::ostream& operator<<(std::ostream& outputStream, const JSONFile& object);

	private: 
		JSONValue* root;
	};
}