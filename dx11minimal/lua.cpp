#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h>

#include "lua.h"

void create_console()
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	freopen("CONIN$", "r", stdin);
}

void lua_test()
{
	std::cout<<"test"<<std::endl;
}