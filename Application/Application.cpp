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
		Logger::PrintDebugSeparator();
		Logger::PrintLog(L"Application starting...\n");
		Logger::PrintLog(L"Game Name: %s\n", GameSettings::GameName());
		Logger::PrintLog(L"Boot Time %s\n", GameSettings::BootTime());
		Logger::PrintDebugSeparator();
	}

	VOID Application::Update()
	{
	//	MessageBox(0, L"Loop", 0, 0);
	}

}