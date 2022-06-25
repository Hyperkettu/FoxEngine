#include "Fox.h"
#include "Simulation.h"

#include "Fox/Engine/SplashScreen.h"

namespace Fox {

	namespace Engine {
	
		Simulation::Simulation()
		{
		}

		Simulation::~Simulation()
		{
		}

		VOID Simulation::PreInitialize() {

			Logger::PrintDebugSeparator();
			Logger::PrintLog(L"Application starting...\n");
			Logger::PrintLog(L"Game Name: %s\n", GameSettings::GameName());
			Logger::PrintLog(L"Boot Time: %s\n", GameSettings::BootTime());
			Logger::PrintLog(L"Engine Mode: %s\n", Fox::Engine::EngineModeToString().c_str());
			Logger::PrintDebugSeparator();

			Fox::Engine::SplashScreen::Open();

		}

	}
}
