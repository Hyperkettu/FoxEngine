#pragma once

#ifdef WIN32
#include <Windows.h>
#endif

#ifdef BUILD_DLL
#define FOX_API __declspec(dllexport)
#else
#define FOX_API __declspec(dllimport)
#endif