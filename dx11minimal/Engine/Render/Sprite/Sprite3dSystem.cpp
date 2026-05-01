#include "Sprite3dSystem.h"
#include <utility>

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


struct SpriteDecs {
	Sprite* sprite;
	Transform worldTransform;

	SpriteDecs(Sprite* sprite, Transform worldTransform)
		: sprite(sprite), worldTransform(worldTransform)
	{ }
};

int partition(vector<SpriteDecs>& arr, int low, int high) {
	SpriteDecs pivot = arr[high];
	int i = low - 1;

	for (int j = low; j < high; j++) {
		if (arr[j].worldTransform.position.y < pivot.worldTransform.position.y) {
			i++;
			swap(arr[i], arr[j]);
		}
	}
	swap(arr[i + 1], arr[high]);
	return i + 1;
}

// Быстрая сортировка
void quickSort(vector<SpriteDecs>& arr, int low, int high) {
	if (low < high) {
		int pi = partition(arr, low, high);
		quickSort(arr, low, pi - 1);
		quickSort(arr, pi + 1, high);
	}
}


void Sprite3dSystem::Update(EntityStorage& entityStorage, float deltaTime)
{
	Blend::Blending(Blend::blendmode::alpha, Blend::blendop::add);
	Rasterizer::Cull(Rasterizer::cullmode::off);
	Depth::Depth(Depth::depthmode::readonly);

	Shaders::gShader(0);
	InputAssembler::IA(InputAssembler::topology::triList);

	std::vector<SpriteDecs> spriteDescs;

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
		spriteDescs.push_back({ sprite, worldTransform });
	}

	quickSort(spriteDescs, 0, spriteDescs.size() - 1);

	for (SpriteDecs& spriteDesc : spriteDescs)
	{
		UpdateWorldMatrix(spriteDesc.worldTransform);

		Shaders::vShader(spriteDesc.sprite->vShader);
		Shaders::pShader(spriteDesc.sprite->pShader);

		ConstBuf::global[3] = XMFLOAT4(spriteDesc.sprite->color.x, spriteDesc.sprite->color.y, spriteDesc.sprite->color.z, spriteDesc.sprite->opacity);
		ConstBuf::Update(5, ConstBuf::global);
		ConstBuf::ConstToVertex(5);
		ConstBuf::ConstToPixel(5);

		Textures::TextureToShader(spriteDesc.sprite->textureName, 5, targetshader::pixel);

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