#include "Fox.h"

namespace Fox {

	namespace Engine {

		FoxEngine foxEngine;

		VOID SetMode(EngineMode mode) {
			foxEngine.SetEngineMode(mode);
		}

		EngineMode GetMode() {
			return foxEngine.GetEngineMode();
		}

		std::wstring EngineModeToString() {
		
			switch (Fox::Engine::GetMode())
			{
			case EngineMode::DEBUG: 
				return L"Debug";
			case EngineMode::RELEASE:
				return L"Release";
			case EngineMode::SERVER:
				return L"Server";
			case EngineMode::EDITOR:
				return L"Editor";
			default:
				return L"None";
			}
		}

		FoxEngine::FoxEngine() {
#ifdef _DEBUG
			engineMode = EngineMode::DEBUG;
#else
			engineMode = EngineMode::RELEASE;
#endif
		}

		FoxEngine::~FoxEngine() {
		}

	}
}