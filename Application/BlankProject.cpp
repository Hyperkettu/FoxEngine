#include "BlankProject.h"

#include "Fox/Engine/Simulation.h"
#include "Fox/Platform/Win32/WinEntry.h"

namespace Fox {

	class BlankProjectApplication : public Fox::Engine::Simulation {
	public:
		BlankProjectApplication();
		~BlankProjectApplication();

		VOID SetupGameSettings();
		VOID Init();
		VOID Update();

		VOID initTest(int v) {
			Logger::PrintLog(L"init value %d\n", v);
		}

		VOID updateTest(int v) {
			Logger::PrintLog(L"update value %d\n", v);
		}

		Fox::Signal<void(int)> updateSignal;
		Fox::SignalBind updateBind;
		Fox::SignalBind initBind;
	};

	ENTRYAPP(BlankProjectApplication)

	BlankProjectApplication::BlankProjectApplication() {}

	BlankProjectApplication::~BlankProjectApplication() {}

	VOID BlankProjectApplication::SetupGameSettings() {
		GameSettings::SetGameName(IDS_GAME_NAME);
		GameSettings::SetShortName(IDS_SHORT_NAME);
		GameSettings::SetMainIcon(IDI_MAINICON);
	}

	VOID BlankProjectApplication::Init()
	{

		ShaderConfig shaderConfig("shader.json");
		INT success = shaderConfig.resolveFile();

		initBind = updateSignal.Connect(*this, &BlankProjectApplication::initTest);
		updateBind = updateSignal.Connect(*this, &BlankProjectApplication::updateTest);

		updateSignal(0);
	}

	VOID BlankProjectApplication::Update() {}
}