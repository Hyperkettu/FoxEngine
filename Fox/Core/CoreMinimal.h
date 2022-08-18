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
#include "Fox/Platform/WindowHandle.h"
#include "Fox/Graphics/IRenderer.h"


#ifdef WIN32

#include <stdexcept>
#include <wrl.h>>

	// DirectX 12 headers
	#include <dxgi1_6.h>
	#include <d3d12.h>
	#include "Fox/Graphics/DirectX/d3dx12.h"
	#include <DirectXMath.h>

	#ifdef _DEBUG
		#include <dxgidebug.h>
	#endif

	#include "Fox/Graphics/DirectX/DirectXHelper.h"
	#include "Fox/Graphics/DirectX/DirectXRaytracingHelper.h"
	#include "Fox/Graphics/DirectX/Direct3D.h"
	#include "Fox/Graphics/DirectX/DirectX12Renderer.h"
	#include "Fox/Platform/Win32/Win32Utils.h"
	#include "Fox/Platform/Win32/SubObject.h"
	#include "Fox/Platform/Win32/WindowCaption.h"
	#include "Fox/Platform/Win32/Window.h"
	#include "Fox/Platform/Win32/IApplication.h"
#endif
