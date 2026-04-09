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
#include "../Lib/timer.h"
#include "../Lib/isingleton.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: EntityStorage
////////////////////////////////////////////////////////////////////////////////
class EntityStorage : public ISingleton
{
public:
	std::vector<Entity*> entities;

private:
	struct ComponentEntityCache
	{
		std::vector<Entity*> entities;
		bool isBuilt = false;
	};

public:
	EntityStorage();
	EntityStorage(const EntityStorage&);
	~EntityStorage();

	Entity* CreateEntity(std::string = "Unnamed", Entity* = nullptr);

	//void RemoveEntityByObject(Entity*);
	Entity* GetEntityByName(std::string);
	std::vector<Entity*> GetEntitiesByName(std::string);
	Entity* GetEntityById(int);
	const std::vector<Entity*>& GetEntitiesWithComponent(const std::type_index& componentType);

	template <typename T>
	const std::vector<Entity*>& GetEntitiesWithComponent()
	{
		return GetEntitiesWithComponent(typeid(T));
	}

	void SaveEntityToFile(Entity* entity, const std::string& filename);
	Entity* LoadEntityFromFile(const std::string& filename);

	void Initialize();
	void Shutdown();
	void CleanMem();

	void OnEntityComponentAdded(Entity* entity, const std::type_index& componentType);
	void OnEntityComponentRemoved(Entity* entity, const std::type_index& componentType);
	void OnEntityDestroyed(Entity* entity);

private:
	int entityCount = 0;
	std::unordered_map<std::type_index, ComponentEntityCache> componentEntityCaches;

	const std::string SAVE_DIRECTORY = "..\\dx11minimal\\Resourses\\EntitySaves\\";
	const std::string EXTENSION = ".json";
};

#endif
