#ifndef NDEBUG

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <thread>
#include <mutex>

#pragma push_macro("byte")
#undef byte
#include <windows.h>
#pragma pop_macro("byte")

#pragma comment(lib,"Ws2_32.lib")//network

#include <lua.hpp>
#pragma comment(lib,"lua51")
#pragma comment(lib,"luajit")
//#include <LuaBridge/LuaBridge.h>

#include "lua.h"
#include "singleton.h"
#include "entityStorage.h"
#include "Transform.h"
#include "Star.h"

namespace 
{
	EntityStorage* storage=Singleton::GetInstance<EntityStorage>();
	lua_State* LUA=0;

	std::deque<std::vector<char>> scripts;
	std::mutex busy;

	void create_console()
	{
		AllocConsole();
		freopen("CONOUT$","w",stdout);
		freopen("CONOUT$","w",stderr);
		freopen("CONIN$","r",stdin);
	}

	void inter()
	{
		lua_Writer writer=
		[](lua_State*,const void* from,size_t size,void* dest)
		{
			std::vector<char>* d=(std::vector<char>*)dest;
			d->insert(d->end(),(const char*)from,(const char*)from+size);
			return 0;
		};

		std::string script;
		while(true)
		{
			std::getline(std::cin,script);
			busy.lock();
				if(luaL_loadstring(LUA,script.c_str()))
				{
					std::cout<<lua_tostring(LUA,-1)<<std::endl;
					lua_pop(LUA,1);
				}
				else
				{
					scripts.push_back(std::vector<char>());
					lua_dump(LUA,writer,&scripts.back());
					lua_pop(LUA,1);
				}
			busy.unlock();
		}
	}

	void inter_remote(unsigned short port)
	{
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2,2),&wsaData);

		SOCKET l=socket(AF_INET,SOCK_STREAM,0);
		sockaddr_in addr={AF_INET,htons(port),INADDR_ANY};
		bind(l,(sockaddr*)&addr,sizeof(addr));
		listen(l,1);
		
		SOCKET s=accept(l,(struct sockaddr*)0,0);

		char script[1024];

		while(true)
		{
			int size=recv(s,script,sizeof(script),0);

			lua_Writer writer=
			[](lua_State*,const void* from,size_t size,void* dest)
			{
				std::vector<char>* d=(std::vector<char>*)dest;
				d->insert(d->end(),(const char*)from,(const char*)from+size);
				return 0;
			};

			busy.lock();

			if(luaL_loadstring(LUA,std::string(script,size).c_str()))
			{
				const std::string error=lua_tostring(LUA,-1);
				send(s,error.c_str(),error.size()+1,0);
				lua_pop(LUA,1);
			}
			else
			{
				scripts.push_back(std::vector<char>());
				lua_dump(LUA,writer,&scripts.back());
				lua_pop(LUA,1);
			}

			busy.unlock();
		}
	}

	/*
	void register_entity(lua_State* lua)
	{
		luabridge::getGlobalNamespace(lua)
		.beginClass<Transform>("Transform")
			.addConstructor<void(*)()>()
		.endClass();

		luabridge::getGlobalNamespace(lua)
		.beginClass<Entity>("Entity")
			.addConstructor<void(*)()>()
			.addFunction("getTransform",&Entity::GetComponent<Transform>)
		.endClass();
	}
	*/

	int cls(lua_State* lua)
	{
		std::system("cls");
		return 0;
	}

	int create_entity(lua_State* lua)
	{
		Entity* e=storage->CreateEntity();
		lua_pushlightuserdata(lua,e);

		return 1;
	}

	int make_star(lua_State* lua)
	{
		Entity* e=(Entity*)lua_touserdata(lua,1);

		e->AddComponent<Transform>();
		e->AddComponent<Star>();

		return 0;
	}

	int set_pos(lua_State* lua)
	{
		Entity* e=(Entity*)lua_touserdata(lua,1);
		Transform* t=e->GetComponent<Transform>();
		if(!t)return 0;
		t->position.x=lua_tonumber(lua,2);
		t->position.y=lua_tonumber(lua,3);
		t->position.z=lua_tonumber(lua,4);

		return 0;
	}

	int get_pos(lua_State* lua)
	{
		Entity* e=(Entity*)lua_touserdata(lua,1);
		Transform* t=e->GetComponent<Transform>();
		if(!t)return 0;
		
		lua_pushnumber(lua,t->position.x);
		lua_pushnumber(lua,t->position.y);
		lua_pushnumber(lua,t->position.z);

		return 3;
	}
	
	int remote(lua_State* lua)
	{
		std::thread(inter_remote,lua_tolstring(lua,)).detach();
		return 0;
	}

	class initializer
	{
	public:
		initializer()
		{
			create_console();
			LUA=luaL_newstate();
			luaL_openlibs(LUA);
			lua_register(LUA,"cls",cls);
			lua_register(LUA,"create_entity",create_entity);
			lua_register(LUA,"make_star",make_star);
			lua_register(LUA,"set_pos",set_pos);
			lua_register(LUA,"get_pos",get_pos);
			lua_register(LUA,"remote",remote);
			std::thread(inter).detach();
			//std::thread(inter_remote,11111).detach();
		}
		~initializer()
		{
			FreeConsole();
			lua_close(LUA);
		}
	}
	do_initialize;
}

void lua()
{
	if(scripts.empty())return;
	if(!busy.try_lock())return;

	luaL_loadbuffer(LUA,scripts.front().data(),scripts.front().size(),"test");
	lua_call(LUA,0,0);
	scripts.pop_front();

	busy.unlock();
}

#else

void lua(){}

#endif