#pragma once

#include <iostream>
#include <string>
#include <memory>

namespace Fox {

	class FOX_API JSONValue {
	public:
		JSONValue() = default;
		virtual ~JSONValue() = default;
		friend std::ostream& operator<<(std::ostream& outputStream, const JSONValue& jsonValue);

		static size_t objectDepth;
	};
} 