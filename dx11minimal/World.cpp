////////////////////////////////////////////////////////////////////////////////
// Filename: World.cpp
////////////////////////////////////////////////////////////////////////////////
#include "World.h"


World::World()
{
}


World::World(const World& other)
{
}


World::~World()
{
}


void World::Shutdown()
{
	size_t size = physicSystems.size();
	for (int i = 0; i < size; i++)
	{
		physicSystems[i]->Shutdown();
	}

	size = renderSystems.size();
	for (int i = 0; i < size; i++)
	{
		renderSystems[i]->Shutdown();
	}
}


Entity* World::CreateEntity()
{
	Entity* entity = new Entity;
	entities.push_back(entity);

	return entity;
}


void World::UpdatePhysic()
{
	size_t size = physicSystems.size();
	for (int i = 0; i < size; i++)
	{
		physicSystems[i]->Update(entities, 0.01f);
	}
}


void World::UpdateRender()
{
	size_t size = renderSystems.size();
	for (int i = 0; i < size; i++)
	{
		renderSystems[i]->Update(entities, 0.01f);
	}
}