#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h>

#include <lua.hpp>
#pragma comment(lib,"lua51")
#pragma comment(lib,"luajit")

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
	lua_State* lua=luaL_newstate();
	luaL_openlibs(lua);
	luaL_dostring(lua,"print(1+2)");
}