////////////////////////////////////////////////////////////////////////////////
// Filename: world.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _WORLD_H_
#define _WORLD_H_

//////////////
// INCLUDES //
//////////////
#include <vector>
#include <memory>
#include "component.h"
#include "entity.h"
#include "system.h"

/////////////
// GLOBALS //
/////////////
const float PHYSICS_DT = 1.0f / 60.0f; // 60 FPS
const float RENDER_DT = 1.0f / 144.0f; // 144 FPS


////////////////////////////////////////////////////////////////////////////////
// Class name: World
////////////////////////////////////////////////////////////////////////////////
class World
{
public:
	World();
	World(const World&);
	~World();

	template <typename T>
	T* AddPhysicSystem()
	{
		auto system = make_unique<T>();
		T* raw_ptr = system.get();
		system->Initialize();
		physicSystems.push_back(move(system));

		return raw_ptr;
	}

	template <typename T>
	T* AddRenderSystem()
	{
		auto system = make_unique<T>();
		T* raw_ptr = system.get();
		system->Initialize();
		renderSystems.push_back(move(system));

		return raw_ptr;
	}

	Entity* CreateEntity();

	void Shutdown();
	void UpdatePhysic();
	void UpdateRender();
private:
	vector<Entity*> entities;
	vector<unique_ptr<System>> physicSystems;
	vector<unique_ptr<System>> renderSystems;
};

#endif