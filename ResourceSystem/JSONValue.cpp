#include "JSONValue.h"

#include "TypedValue.cpp"
#include "JSONObject.h"
#include "JSONValueArray.h"

namespace Fox {

	size_t JSONValue::objectDepth = 0u;
	
	std::ostream& operator<<(std::ostream& outputStream, const JSONValue& jsonValue) {

		const StringValue* stringValue = dynamic_cast<const StringValue*>(&jsonValue);
		const BoolValue* booleanValue = dynamic_cast<const BoolValue*>(&jsonValue);
		const IntValue* intValue = dynamic_cast<const IntValue*>(&jsonValue);
		const FloatValue* floatValue = dynamic_cast<const FloatValue*>(&jsonValue);
		const JSONObject* objectValue = dynamic_cast<const JSONObject*>(&jsonValue);
		const JSONValueArray* arrayValue = dynamic_cast<const JSONValueArray*>(&jsonValue);

		if (stringValue) {
			outputStream << *stringValue;
		} else if (booleanValue) {
			outputStream << *booleanValue;
		} else if (intValue) {
			outputStream << *intValue;
		}
		else if (floatValue) {
			outputStream << *floatValue;
		} else if (arrayValue) {
			JSONValue::objectDepth++;
			outputStream << *arrayValue;
			JSONValue::objectDepth--;
		} else if (objectValue) {
			JSONValue::objectDepth++;
			outputStream << *objectValue;
			JSONValue::objectDepth--;
		}

		return outputStream;
	}
}