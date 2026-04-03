////////////////////////////////////////////////////////////////////////////////
// Filename: EntityStorage.cpp
////////////////////////////////////////////////////////////////////////////////
#include "entityStorage.h"
#include <algorithm>


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
	componentEntityCaches.clear();
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
	entity->localTime = timer::currentTime;
	entity->SetOwnerStorage(this);

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


Entity* EntityStorage::GetEntityByName(string Name)
{
	for (Entity* entity : entities) {
		if (entity->name == Name) {
			return entity;
		}
	}
}


vector<Entity*> EntityStorage::GetEntitiesByName(string Name)
{
	vector<Entity*> array;
	for (Entity* entity : entities) {
		if (entity->name == Name) {
			array.push_back(entity);
		}
	}

	return array;
}


Entity* EntityStorage::GetEntityById(int id)
{
	for (Entity* entity : entities) {
		if (entity->GetId() == id) {
			return entity;
		}
	}
}


const vector<Entity*>& EntityStorage::GetEntitiesWithComponent(const type_index& componentType)
{
	auto cacheIt = componentEntityCaches.find(componentType);
	if (cacheIt == componentEntityCaches.end()) {
		cacheIt = componentEntityCaches.emplace(componentType, ComponentEntityCache{}).first;
	}

	ComponentEntityCache& cache = cacheIt->second;
	if (!cache.isBuilt) {
		cache.entities.clear();

		for (Entity* entity : entities) {
			if (entity != nullptr && !entity->IsDeleting() && entity->HasComponent(componentType)) {
				cache.entities.push_back(entity);
			}
		}

		cache.isBuilt = true;
	}

	return cache.entities;
}

void EntityStorage::CleanMem()
{
	int i = 0;
	while (i < entities.size())
	{
		Entity* entity = entities[i];
		if (entity->IsDeleting())
		{
			OnEntityDestroyed(entity);
			delete entity;
			entities.erase(entities.begin() + i);
		}
		else
		{
			i++;
		}
	}
}

void EntityStorage::OnEntityComponentAdded(Entity* entity, const type_index& componentType)
{
	auto cacheIt = componentEntityCaches.find(componentType);
	if (cacheIt == componentEntityCaches.end() || !cacheIt->second.isBuilt) {
		return;
	}

	cacheIt->second.isBuilt = false;
}

void EntityStorage::OnEntityComponentRemoved(Entity* entity, const type_index& componentType)
{
	auto cacheIt = componentEntityCaches.find(componentType);
	if (cacheIt == componentEntityCaches.end() || !cacheIt->second.isBuilt) {
		return;
	}

	cacheIt->second.isBuilt = false;
}

void EntityStorage::OnEntityDestroyed(Entity* entity)
{
	for (auto& cachePair : componentEntityCaches) {
		if (cachePair.second.isBuilt) {
			cachePair.second.isBuilt = false;
		}
	}
}
