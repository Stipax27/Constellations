#include "WayDrawSystem.h"

#include "../../GlobalConfigs.h"

using namespace std;



WayDrawSystem::WayDrawSystem()
{
}


void WayDrawSystem::Initialize()
{
}


void WayDrawSystem::Shutdown()
{
}


void WayDrawSystem::Update(EntityStorage& entityStorage, float deltaTime)
{
	/*Blend::Blending(Blend::blendmode::alpha, Blend::blendop::add);
	Rasterizer::Cull(Rasterizer::cullmode::back);
	Depth::Depth(Depth::depthmode::readonly);

	const std::vector<Entity*>& entities = entityStorage.GetEntitiesWithComponent<RotatingBody>();
	for (Entity* entity : entities)
	{
		if (!IsEntityValid(entity))
			continue;

		Transform* transform = entity->GetComponent<Transform>();
		if (transform == nullptr)
			continue;

		RotatingBody* rotatingBody = entity->GetComponent<RotatingBody>();
		if (rotatingBody == nullptr)
			continue;

		Transform worldTransform = GetWorldTransform(entity);
		ConstBuf::global[0] = XMFLOAT4(worldTransform.position.x, worldTransform.position.y, worldTransform.position.z, 0);
		ConstBuf::global[0].w = STAR_LINKING_RADIUS;

		ConstBuf::global[1] = XMFLOAT4(0.471f, 0.922f, 1.0f, 0.25f);

		ConstBuf::Update(5, ConstBuf::global);
		ConstBuf::ConstToVertex(5);

		Shaders::vShader(18);
		Shaders::pShader(18);
		Shaders::gShader(0);

		int n = 11 * sqrt(STAR_LINKING_RADIUS);
		ConstBuf::drawerV[0] = n;
		Draw::Drawer(n * n);
	}*/
}