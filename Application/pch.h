#pragma once

#include <Windows.h>

#include "Application/resource.h"

#include "Fox/Fox.h"

#define MAX_STRING_NAME 256
#define GetHInstance() GetModuleHandle(nullptr) 

#define Log(...)				\
{								\
char buf[512];					\
sprintf_s(buf, __VA_ARGS__);	\
OutputDebugStringA(buf);		\
}	