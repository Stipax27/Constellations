////////////////////////////////////////////////////////////////////////////////
// Filename: entityStorage.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _ENTITY_STORAGE_H_
#define _ENTITY_STORAGE_H_

//////////////
// INCLUDES //
//////////////
#include <typeindex>
#include <unordered_map>
#include <vector>
#include "entity.h"
#include "Engine/Lib/timer.h"
#include "Engine/Lib/isingleton.h"

using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: EntityStorage
////////////////////////////////////////////////////////////////////////////////
class EntityStorage : public ISingleton
{
public:
	vector<Entity*> entities;

private:
	struct ComponentEntityCache
	{
		vector<Entity*> entities;
		bool isBuilt = false;
	};

public:
	EntityStorage();
	EntityStorage(const EntityStorage&);
	~EntityStorage();

	Entity* CreateEntity(string = "Unnamed", Entity* = nullptr);

	//void RemoveEntityByObject(Entity*);
	Entity* GetEntityByName(string);
	vector<Entity*> GetEntitiesByName(string);
	Entity* GetEntityById(int);
	const vector<Entity*>& GetEntitiesWithComponent(const type_index& componentType);

	template <typename T>
	const vector<Entity*>& GetEntitiesWithComponent()
	{
		return GetEntitiesWithComponent(typeid(T));
	}

	void Initialize();
	void Shutdown();
	void CleanMem();
	void OnEntityComponentAdded(Entity* entity, const type_index& componentType);
	void OnEntityComponentRemoved(Entity* entity, const type_index& componentType);
	void OnEntityDestroyed(Entity* entity);

private:
	int entityCount = 0;
	unordered_map<type_index, ComponentEntityCache> componentEntityCaches;
};

#endif
