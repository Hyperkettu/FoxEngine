#pragma once

#include "Fox/Engine/Simulation.h"

namespace Fox {

	class Application: public Fox::Engine::Simulation {
	public:
		Application();
		~Application();

		VOID SetupGameSettings();
		VOID Init();
		VOID Update();

		VOID initTest(int v) {
			Logger::PrintLog(L"init value %d\n", v);
		}

		VOID updateTest(int v) {
			Logger::PrintLog(L"update value %d\n", v);
		}

		Signal<void(int)> updateSignal;
		SignalBind updateBind;
		SignalBind initBind;
	};


}