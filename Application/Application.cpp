#include "pch.h"
#include "Application.h"

#include "Fox/Platform/WIN32/WinEntry.h"
#include "Fox/Core/TypedValue.h"

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

		std::string data = Fox::ResourceSystem::readFile("oma.json");
		Fox::JSONFile* file = new Fox::JSONFile;
		file->parse(data);
		Fox::StringValue& s = file->get<Fox::JSONObject>().get<Fox::JSONObject>("child").get<Fox::JSONObject>("child2").get<Fox::JSONValueArray>("myArray").get<Fox::JSONObject>(1).get<Fox::StringValue>("value");
		std::string& str = s.value;
	}

	VOID Application::Update()
	{
	///	MessageBox(0, L"Loop", 0, 0);
	}

}