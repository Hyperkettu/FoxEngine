#pragma once

#include <Windows.h>

#define Log(...)				\
{								\
char buf[512];					\
sprintf_s(buf, __VA_ARGS__);	\
OutputDebugStringA(buf);		\
}	