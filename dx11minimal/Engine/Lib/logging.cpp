#include "logging.h"
#include <windows.h>
#include <debugapi.h>

void Log(const char* message)
{
	OutputDebugStringA(message);
}