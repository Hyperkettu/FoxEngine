#pragma once

#ifdef WIN32
#include <Windows.h>
#endif

#ifdef BUILD_DLL
#define FOX_API __declspec(dllexport)
#else
#define FOX_API __declspec(dllimport)
#endif

#define MAX_STRING_NAME 256
#define GetHInstance() GetModuleHandle(nullptr) 

#include "Fox/Common/Logger.h"
#include "Fox/Common/Time.h"
#include "Fox/Core/GameSettings.h"
#include "Fox/Core/JSONFile.h"
#include "Fox/Core/ResourceSystem.h"