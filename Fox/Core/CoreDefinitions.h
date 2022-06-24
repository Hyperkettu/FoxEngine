#pragma once 

#ifdef BUILD_DLL
	#define FOX_API __declspec(dllexport)
#else
	#define FOX_API __declspec(dllimport)
#endif

#define MAX_STRING_NAME 256
#define GetHInstance() GetModuleHandle(nullptr) 
#define SAFE_DELETE(x) if(x) { delete x; x = nullptr; }