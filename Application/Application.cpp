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

	VOID Application::Init()
	{
		MessageBox(0, L"Loaded up", 0, 0);
	}

	VOID Application::Update()
	{
		MessageBox(0, L"Loop", 0, 0);
	}

}