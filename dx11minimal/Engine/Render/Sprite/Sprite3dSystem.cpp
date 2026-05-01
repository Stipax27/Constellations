#include "Sprite3dSystem.h"

using namespace std;


Sprite3dSystem::Sprite3dSystem()
{
	frustum = Singleton::GetInstance<FrustumClass>();
}


void Sprite3dSystem::Initialize()
{
}


void Sprite3dSystem::Shutdown()
{
	if (frustum)
	{
		frustum = 0;
	}
}


void Sprite3dSystem::Update(EntityStorage& entityStorage, float deltaTime)
{
	Blend::Blending(Blend::blendmode::alpha, Blend::blendop::add);
	Rasterizer::Cull(Rasterizer::cullmode::off);
	Depth::Depth(Depth::depthmode::readonly);

	Shaders::gShader(0);
	InputAssembler::IA(InputAssembler::topology::triList);

	const std::vector<Entity*>& entities = entityStorage.GetEntitiesWithComponent<Sprite>();
	size_t size = entities.size();
	for (int i = 0; i < size; i++)
	{
		Entity* entity = entities[i];
		if (!IsEntityValid(entity))
			continue;

		Sprite* sprite = entity->GetComponent<Sprite>();
		if (sprite == nullptr || !sprite->active)
			continue;

		Transform worldTransform = GetWorldTransform(entity);
		UpdateWorldMatrix(worldTransform);

		Shaders::vShader(sprite->vShader);
		Shaders::pShader(sprite->pShader);

		ConstBuf::global[3] = XMFLOAT4(sprite->color.x, sprite->color.y, sprite->color.z, sprite->opacity);
		ConstBuf::Update(5, ConstBuf::global);
		ConstBuf::ConstToVertex(5);
		ConstBuf::ConstToPixel(5);

		Textures::TextureToShader(sprite->textureName, 5, targetshader::pixel);

		Draw::Drawer(1);
	}
}


void Sprite3dSystem::UpdateWorldMatrix(Transform worldTransform)
{
	ConstBuf::camera.world = GetWorldMatrix(worldTransform);
	ConstBuf::UpdateCamera();
	ConstBuf::ConstToVertex(3);
	ConstBuf::ConstToPixel(3);
}