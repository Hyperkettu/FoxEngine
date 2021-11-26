#pragma once

#include "Fox/Platform/WIN32/IApplication.h"

namespace Fox {

	class Application: public IApplication {
	public:
		Application();
		~Application();

		VOID Init();
		VOID Update();
	};


}