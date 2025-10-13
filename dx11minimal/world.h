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

#include "cameraclass.h"

using namespace std;

/////////////
// GLOBALS //
/////////////
const float PHYSICS_DT = 1.0f / 60.0f; // 60 FPS
const float RENDER_DT = 1.0f / 144.0f; // 144 FPS
const float SCREEN_DEPTH = 10000.0f;
const float SCREEN_NEAR = 0.1f;


////////////////////////////////////////////////////////////////////////////////
// Class name: World
////////////////////////////////////////////////////////////////////////////////
class World
{
public:
	CameraClass* m_Camera;

public:
	World();
	World(const World&);
	~World();

	template <typename T, typename... Args>
	T* AddPhysicSystem(Args&&... args)
	{
		auto system = make_unique<T>(forward<Args>(args)...);
		T* raw_ptr = system.get();
		system->Initialize();
		physicSystems.push_back(move(system));

		return raw_ptr;
	}

	template <typename T, typename... Args>
	T* AddRenderSystem(Args&&... args)
	{
		auto system = make_unique<T>(forward<Args>(args)...);
		T* raw_ptr = system.get();
		system->Initialize();
		renderSystems.push_back(move(system));

		return raw_ptr;
	}

	Entity* CreateEntity();

	bool Initialize(float);
	void Shutdown();
	void PreCalculations();
	bool UpdatePhysic();
	bool UpdateRender();
	void CleanMem();
private:
	vector<Entity*> entities;
	vector<unique_ptr<System>> physicSystems;
	vector<unique_ptr<System>> renderSystems;
};

#endif