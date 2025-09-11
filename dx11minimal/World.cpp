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


bool World::Initialize()
{
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	m_Camera->SetPosition(0.0f, 0.0f, -12.0f);
	m_Camera->SetRotation(0.0f, 0.0f, 0.0f);

	return true;
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


bool World::UpdatePhysic()
{
	bool result;
	size_t size = physicSystems.size();
	for (int i = 0; i < size; i++)
	{
		result = physicSystems[i]->Update(entities, 0.01f);
		if (!result)
		{
			return false;
		}
	}
}


bool World::UpdateRender()
{
	bool result;

	// Updating time in const buffer
	ConstBuf::frame.time = XMFLOAT4{ (float)(timer::frameBeginTime * 0.01f), 0, 0, 0 };
	ConstBuf::UpdateFrame();

	size_t size = renderSystems.size();
	for (int i = 0; i < size; i++)
	{
		result = renderSystems[i]->Update(entities, 0.01f);
		if (!result)
		{
			return false;
		}
	}
}

void World::CleanMem()
{
	size_t size = entities.size();
	int i = 0;
	while (i < size)
	{
		Entity* entity = entities[i];
		if (entity == nullptr)
		{
			entities.erase(entities.begin() + i);
		}
		else
		{
			i++;
		}
	}
}