#include "SkinnedMeshSystem.h"

using namespace std;

SkinnedMeshSystem::SkinnedMeshSystem(
	FrustumClass* f,
	CameraClass* c)
{
	frustum = f;
	camera = c;
}

void SkinnedMeshSystem::Initialize() {}
void SkinnedMeshSystem::Shutdown() {}

void SkinnedMeshSystem::Update(EntityStorage& entityStorage, float deltaTime)
{
	const std::vector<Entity*>& entities = entityStorage.GetEntitiesWithComponent<SkinnedMesh>();
	Blend::Blending(Blend::blendmode::off, Blend::blendop::add);
	Depth::Depth(Depth::depthmode::on);
	Rasterizer::Cull(Rasterizer::cullmode::off);

	for (Entity* entity : entities)
	{
		if (!IsEntityValid(entity))
		{
			continue;
		}

		Transform* transform = entity->GetComponent<Transform>();
		SkinnedMesh* skinned = entity->GetComponent<SkinnedMesh>();

		if (!transform || !skinned || !skinned->active)
		{
			continue;
		}

		if (skinned->gpuModelIndex < 0 ||
			skinned->gpuModelIndex >= Models::modelsCount)
		{
			continue;
		}

		Transform worldTransform = GetWorldTransform(entity);

		if (!frustum->CheckSphere(worldTransform.position,
			worldTransform.scale.magnitude()))
		{
			continue;
		}

		UpdateWorldMatrix(worldTransform);

		// Use skinned vertex shader + mesh pixel shader.
		Shaders::vShader(27);
		Shaders::pShader(15);

		static std::vector<XMMATRIX> identityPalette(128, XMMatrixIdentity());
		SkeletalAnimationComponent* animComp = entity->GetComponent<SkeletalAnimationComponent>();
		if (animComp && !animComp->bonePalette.empty())
		{
			StructBuf::UpdateBoneMatrices(animComp->bonePalette.data(), static_cast<unsigned int>(animComp->bonePalette.size()));
		}
		else
		{
			StructBuf::UpdateBoneMatrices(identityPalette.data(), StructBuf::boneMatrixCapacity);
		}

		InputAssembler::IA(InputAssembler::topology::triList);
		InputAssembler::vBuffer(skinned->gpuModelIndex);

		context->DrawIndexed(
			Models::Model[skinned->gpuModelIndex].indexes,
			0, 0);
	}
}


void SkinnedMeshSystem::UpdateWorldMatrix(Transform worldTransform)
{
	StructBuf::modelMatrixData[0] = GetWorldMatrix(worldTransform);
	StructBuf::UpdateModelMatrices(1);
	ConstBuf::ConstToVertex(3);
	ConstBuf::ConstToPixel(3);
}
