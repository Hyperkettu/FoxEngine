#include "Fox.h"

namespace Fox {

	GameSettings* GameSettings::instance;

	GameSettings::GameSettings()
	{
		instance = this;

		wcscpy_s(instance->gameName, L"Undefined");
		wcscpy_s(instance->shortName, L"Undefined");
		wcscpy_s(instance->bootTime, Time::GetDateTimeString(TRUE).c_str());


	}

	GameSettings::~GameSettings()
	{
	}
}
