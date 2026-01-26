////////////////////////////////////////////////////////////////////////////////
// Filename: EntityStorage.cpp
////////////////////////////////////////////////////////////////////////////////
#include "entityStorage.h"


EntityStorage::EntityStorage()
{
}


EntityStorage::EntityStorage(const EntityStorage& other)
{
}


EntityStorage::~EntityStorage()
{
}


void EntityStorage::Initialize()
{
}


void EntityStorage::Shutdown()
{
	if (entities.size() > 0) {
		for (int i = 0; i < entities.size(); i++)
		{
			Entity* entity = entities[i];
			delete entity;
		}
		entities.clear();
	}
}


Entity* EntityStorage::CreateEntity(string Name, Entity* Parent)
{
	Entity* entity = new Entity;
	entity->name = Name;
	entity->SetId(entityCount++);

	if (Parent != nullptr) {
		Parent->AddChild(entity);
	}

	entities.push_back(entity);

	return entity;
}


//void EntityStorage::RemoveEntityByObject(Entity* object)
//{
//	for (int i = 0; i < entities.size(); i++) {
//		Entity* entity = entities[i];
//		if (entity == object) {
//			entity->Destroy();
//			entities.erase(entities.begin() + i);
//			break;
//		}
//	}
//}


void EntityStorage::CleanMem()
{
	int i = 0;
	while (i < entities.size())
	{
		Entity* entity = entities[i];
		if (entity->IsDeleting())
		{
			delete entity;
			entities.erase(entities.begin() + i);
		}
		else
		{
			i++;
		}
	}
}