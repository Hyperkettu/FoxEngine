#include "pch.h"
#include "Application.h"

#include "Fox/Platform/WIN32/WinEntry.h"

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
		//MessageBox(0, L"Loaded up", 0, 0);
		Logger::PrintLog(L"Loaded up %s, thanks", L"project");
	}

	VOID Application::Update()
	{
	//	MessageBox(0, L"Loop", 0, 0);
	}

}