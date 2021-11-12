#include "JSONFile.h"

namespace Fox {

	std::ostream& operator<<(std::ostream& outputStream, const JSONFile& file) {
		outputStream << *file.root;
		return outputStream;
	}

}