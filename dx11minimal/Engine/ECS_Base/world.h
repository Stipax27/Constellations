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
#include "entityStorage.h"
#include "system.h"

#include "../Camera/cameraclass.h"
#include "../Lib/isingleton.h"

/////////////
// GLOBALS //
/////////////
#define PHYSICS_DT 1000.0f / 60.0f // 60 FPS
#define RENDER_DT 1000.0f / (float)FRAMES_PER_SECOND // 144 FPS


////////////////////////////////////////////////////////////////////////////////
// Class name: World
////////////////////////////////////////////////////////////////////////////////
class World : public ISingleton
{
public:
	CameraClass* m_Camera;
	EntityStorage* entityStorage;

public:
	World();
	World(const World&);
	~World();

	template <typename T, typename... Args>
	T* AddComputeSystem(Args&&... args)
	{
		auto system = std::make_unique<T>(std::forward<Args>(args)...);
		T* raw_ptr = system.get();
		system->Initialize();
		computeSystems.push_back(move(system));

		return raw_ptr;
	}

	template <typename T, typename... Args>
	T* AddPhysicSystem(Args&&... args)
	{
		auto system = std::make_unique<T>(std::forward<Args>(args)...);
		T* raw_ptr = system.get();
		system->Initialize();
		physicSystems.push_back(move(system));

		return raw_ptr;
	}

	template <typename T, typename... Args>
	T* AddRenderSystem(Args&&... args)
	{
		auto system = std::make_unique<T>(std::forward<Args>(args)...);
		T* raw_ptr = system.get();
		system->Initialize();
		renderSystems.push_back(move(system));

		return raw_ptr;
	}

	void Initialize();
	void Shutdown();

	void PreCalculations();

	void UpdateCompute();
	void UpdatePhysic();
	void UpdateRender();

private:
	std::vector<std::unique_ptr<System>> computeSystems;
	std::vector<std::unique_ptr<System>> physicSystems;
	std::vector<std::unique_ptr<System>> renderSystems;

	bool firstFrame = true;

private:
	void RenderPerlinNoise();
	void RenderVoronoiNoise();
	void RenderStarNoise();

	void NoisesToShaders();
};

#endif