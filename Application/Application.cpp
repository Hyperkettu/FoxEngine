#include "pch.h"
#include "Application.h"

#include "Fox/Platform/Win32/WinEntry.h"

namespace Fox {

	ENTRYAPP(Application)

	Application::Application()
	{
	}

	Application::~Application()
	{
	}

	VOID Application::SetupGameSettings()
	{
		GameSettings::SetGameName(IDS_GAME_NAME);
		GameSettings::SetShortName(IDS_SHORT_NAME);
		GameSettings::SetMainIcon(IDI_MAINICON);
	}

	VOID Application::Init()
	{

		ShaderConfig shaderConfig("shader.json");
		INT success = shaderConfig.resolveFile();

		initBind = updateSignal.Connect(*this, &Application::initTest);
		updateBind = updateSignal.Connect(*this, &Application::updateTest);

		updateSignal(0);
	}

	VOID Application::Update()
	{
		static int val = 1;
	///	MessageBox(0, L"Loop", 0, 0);
	//	updateSignal(val);
		val++;
	}

}