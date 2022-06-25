#pragma once 

#include "Fox/Platform/Win32/IApplication.h"

namespace Fox {

	namespace Engine {
	
		class FOX_API Simulation : public Fox::Platform::Win32::IApplication {

		public:
			Simulation();
			~Simulation();

			virtual VOID PreInitialize() override;

		};
	}
}