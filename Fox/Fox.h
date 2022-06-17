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
#define SAFE_DELETE(x) if(x) { delete x; x = nullptr; }

#include "Fox/Common/Logger.h"
#include "Fox/Common/Time.h"
#include "Fox/Core/CallableWrapper.h"
#include "Fox/Core/Delegate.h"
#include "Fox/Core/GameSettings.h"
#include "Fox/Core/JSONFile.h"
#include "Fox/Core/ResourceSystem.h"
#include "Fox/Core/Signal.h"
#include "Fox/Core/SignalBind.h"
#include "Fox/Graphics/ShaderConfig.h"