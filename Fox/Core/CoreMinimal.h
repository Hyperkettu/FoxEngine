#pragma once

#include "Fox/Core/CoreDefinitions.h"

#include "Fox/Engine/FoxEngine.h"

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

#ifdef WIN32
	#include "Fox/Platform/Win32/Win32Utils.h"
	#include "Fox/Platform/Win32/SubObject.h"
	#include "Fox/Platform/Win32/Window.h"
	#include "Fox/Platform/Win32/IApplication.h"
#endif
