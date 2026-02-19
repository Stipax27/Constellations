#include "SkinnedMeshSystem.h"
#include "../Components/SkeletalAnimationComponent.h"

SkinnedMeshSystem::SkinnedMeshSystem(
	FrustumClass* f,
	CameraClass* c,
	ID3D11Buffer* boneBuf)
{
	frustum = f;
	camera = c;
	boneBuffer = boneBuf;
}

void SkinnedMeshSystem::Initialize() {}
void SkinnedMeshSystem::Shutdown() {}

void SkinnedMeshSystem::Update(std::vector<Entity*>& entities, float deltaTime)
{
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

		if (!transform || !skinned)
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
		Shaders::vShader(23);
		Shaders::pShader(15);

		static std::vector<XMMATRIX> identityPalette(128, XMMatrixIdentity());
		SkeletalAnimationComponent* animComp = entity->GetComponent<SkeletalAnimationComponent>();
		if (animComp && !animComp->bonePalette.empty())
		{
			context->UpdateSubresource(boneBuffer, 0, nullptr, animComp->bonePalette.data(), 0, 0);
		}
		else
		{
			context->UpdateSubresource(boneBuffer, 0, nullptr, identityPalette.data(), 0, 0);
		}

		context->VSSetConstantBuffers(1, 1, &boneBuffer);

		InputAssembler::IA(InputAssembler::topology::triList);
		InputAssembler::vBuffer(skinned->gpuModelIndex);

		context->DrawIndexed(
			Models::Model[skinned->gpuModelIndex].indexes,
			0, 0);
	}
}

XMMATRIX SkinnedMeshSystem::GetWorldMatrix(Transform worldTransform)
{
	XMMATRIX rotateMatrix = worldTransform.mRotation;
	XMMATRIX scaleMatrix = XMMatrixScaling(worldTransform.scale.x, worldTransform.scale.y, worldTransform.scale.z);
	XMMATRIX translateMatrix = XMMatrixTranslation(worldTransform.position.x, worldTransform.position.y, worldTransform.position.z);

	// Multiply the scale, rotation, and translation matrices together to create the final world transformation matrix.
	XMMATRIX srMatrix = scaleMatrix * rotateMatrix;
	XMMATRIX worldMatrix = srMatrix * translateMatrix;

	return XMMatrixTranspose(worldMatrix);
}

void SkinnedMeshSystem::UpdateWorldMatrix(Transform worldTransform)
{
	ConstBuf::camera.world = GetWorldMatrix(worldTransform);
	ConstBuf::UpdateCamera();
	ConstBuf::ConstToVertex(3);
	ConstBuf::ConstToPixel(3);
}
