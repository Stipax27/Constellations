#include "PivotDrawSystem.h"

using namespace std;


PivotDrawSystem::PivotDrawSystem()
{
	camera = Singleton::GetInstance<CameraClass>();
	frustum = Singleton::GetInstance<FrustumClass>();
}


void PivotDrawSystem::Initialize()
{
}


void PivotDrawSystem::Shutdown()
{
	if (frustum)
		frustum = 0;

	if (camera)
		camera = 0;
}


void PivotDrawSystem::Update(EntityStorage& entityStorage, float deltaTime)
{
	Blend::Blending(Blend::blendmode::alpha, Blend::blendop::add);
	Rasterizer::Cull(Rasterizer::cullmode::back);
	Depth::Depth(Depth::depthmode::off);

	Shaders::gShader(0);
	InputAssembler::IA(InputAssembler::topology::triList);

	const std::vector<Entity*>& entities = entityStorage.GetEntitiesWithComponent<Transform>();
	size_t size = entities.size();
	for (int i = 0; i < size; i++)
	{
		Entity* entity = entities[i];
		if (!IsEntityValid(entity))
		{
			continue;
		}

		Transform worldTransform = GetWorldTransform(entity);

		float radius = PIVOT_BASIC_RADIUS;

		float distance = (camera->position - worldTransform.position).magnitude();
		float screenSize = 1.0f / distance;

		if (screenSize < PIVOT_MIN_SCREEN_SIZE) {
			float d = distance / (1 / PIVOT_MIN_SCREEN_SIZE);
			radius *= d;
		}
		else if (screenSize > 1) {
			radius *= distance;
		}

		ConstBuf::global[0] = XMFLOAT4(worldTransform.position.x, worldTransform.position.y, worldTransform.position.z, 0);
		ConstBuf::global[0].w = radius;

		ConstBuf::global[1] = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

		ConstBuf::Update(5, ConstBuf::global);
		ConstBuf::ConstToVertex(5);

		Shaders::vShader(18);
		Shaders::pShader(18);

		int n = 11;
		ConstBuf::drawerV[0] = n;
		Draw::Drawer(n * n);
	}
}