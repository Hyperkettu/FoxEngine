#include "Fox.h"

namespace Fox {

	template<class U>
	std::ostream& operator<<(std::ostream& outputStream, const TypedValue<U>& jsonValue) {
		outputStream << jsonValue.value;
		return outputStream;
	}

	template<>
	inline std::ostream& operator<<(std::ostream& outputStream, const TypedValue<std::string>& jsonValue) {
		outputStream << "\"" << jsonValue.value << "\"";
		return outputStream;
	}

	template<>
	inline std::ostream& operator<<(std::ostream& outputStream, const TypedValue<bool>& jsonValue) {
		outputStream << (jsonValue.value ? "true" : "false");
		return outputStream;
	}
}