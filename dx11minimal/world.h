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
#define PHYSICS_DT 1000.0f / 60.0f // 60 FPS
#define RENDER_DT 1000.0f / (float)FRAMES_PER_SECOND // 144 FPS
#define SCREEN_DEPTH 1000.0f
#define SCREEN_NEAR 0.1f


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

	Entity* CreateEntity(string = "Unnamed", Entity* = nullptr);
	void RemoveEntityByObject(Entity*);

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

	bool firstFrame = true;
};

#endif