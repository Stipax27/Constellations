#include "GravityDrawSystem.h"



GravityDrawSystem::GravityDrawSystem()
{
}


void GravityDrawSystem::Initialize()
{
}


void GravityDrawSystem::Shutdown()
{
}


void GravityDrawSystem::Update(vector<Entity*>& entities, float deltaTime)
{
	Blend::Blending(Blend::blendmode::alpha, Blend::blendop::add);
	Rasterizer::Cull(Rasterizer::cullmode::front);
	Depth::Depth(Depth::depthmode::readonly);

	for (Entity* entity : entities)
	{
		if (!IsEntityValid(entity))
			continue;

		Transform* transform = entity->GetComponent<Transform>();
		if (transform == nullptr)
			continue;
			
		GravityPoint* gravityPoint = entity->GetComponent<GravityPoint>();
		if (gravityPoint == nullptr)
			continue;

		Transform worldTransform = GetWorldTransform(entity);
		ConstBuf::global[0] = XMFLOAT4(worldTransform.position.x, worldTransform.position.y, worldTransform.position.z, 0);
		ConstBuf::global[0].w = gravityPoint->radius;

		if (gravityPoint->active) {
			ConstBuf::global[1] = XMFLOAT4(0, 1, 0, 0.25f);
		}
		else {
			ConstBuf::global[1] = XMFLOAT4(0, 0.25f, 0, 0.25f);
		}

		ConstBuf::Update(5, ConstBuf::global);
		ConstBuf::ConstToVertex(5);

		Shaders::vShader(18);
		Shaders::pShader(18);
		Shaders::gShader(0);

		int n = 11 * sqrt(gravityPoint->radius);
		ConstBuf::drawerV[0] = n;
		Draw::Drawer(n * n);
	}
}