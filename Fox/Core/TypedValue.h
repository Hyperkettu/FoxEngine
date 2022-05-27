#pragma once

#include "Fox/Core/JSONValue.h"

namespace Fox {

	template<class ValueType>
	class FOX_API TypedValue: public JSONValue {
	public:
		TypedValue() = default;
		TypedValue(const char* string) : value(std::string(string)) {}
		TypedValue(ValueType value) : value(value) {}
		virtual ~TypedValue() = default;
		ValueType& getValue() {
			return value;
		}
		template<class U>
		friend std::ostream& operator<<(std::ostream& outputStream, const TypedValue<U>& jsonValue);
	public:
		ValueType value;
	};

	typedef TypedValue<std::string> StringValue;
	typedef TypedValue<bool> BoolValue;
	typedef TypedValue<int> IntValue;
	typedef TypedValue<float> FloatValue;
}