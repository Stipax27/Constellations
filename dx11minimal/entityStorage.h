////////////////////////////////////////////////////////////////////////////////
// Filename: entityStorage.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _ENTITY_STORAGE_H_
#define _ENTITY_STORAGE_H_

//////////////
// INCLUDES //
//////////////
#include <vector>
#include "entity.h"
#include "timer.h"

using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: EntityStorage
////////////////////////////////////////////////////////////////////////////////
class EntityStorage
{
public:
	vector<Entity*> entities;

public:
	EntityStorage();
	EntityStorage(const EntityStorage&);
	~EntityStorage();

	Entity* CreateEntity(string = "Unnamed", Entity* = nullptr);
	//void RemoveEntityByObject(Entity*);
	Entity* GetEntityByName(string);
	vector<Entity*> GetEntitiesByName(string);

	void Initialize();
	void Shutdown();
	void CleanMem();
private:
	int entityCount = 0;
};

#endif