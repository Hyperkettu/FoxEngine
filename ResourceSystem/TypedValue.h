#pragma once

#include "JSONValue.h"

namespace Fox {

	template<class ValueType>
	class TypedValue: public JSONValue {
	public:
		TypedValue() = default;
		TypedValue(const char* string) : value(std::string(string)) {}
		TypedValue(ValueType value) : value(value) {}
		virtual ~TypedValue() = default;
		template<class U>
		friend std::ostream& operator<<(std::ostream& outputStream, const TypedValue<U>& jsonValue);
	protected:
		ValueType value;
	};

	typedef TypedValue<std::string> StringValue;
	typedef TypedValue<bool> BoolValue;
	typedef TypedValue<int> IntValue;
	typedef TypedValue<float> FloatValue;
}