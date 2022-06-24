#pragma once 

namespace Fox {

	namespace Engine {

		class FOX_API FoxEngine;
	
		enum EngineMode : INT {
			NONE,
			DEBUG,
			RELEASE,
			EDITOR,
			SERVER
		};
		
		extern FoxEngine foxEngine;

		VOID FOX_API SetMode(EngineMode mode);
		EngineMode FOX_API GetMode();
		std::wstring FOX_API EngineModeToString();

		class FOX_API FoxEngine {

		public:
			FoxEngine();
			~FoxEngine();

			VOID SetEngineMode(EngineMode mode) {
				engineMode = mode;
			}

			EngineMode GetEngineMode() CONST {
				return engineMode;
			}

		private: 
			EngineMode engineMode;
		};
	}
}