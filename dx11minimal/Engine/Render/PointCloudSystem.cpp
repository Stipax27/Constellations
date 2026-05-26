#include "PointCloudSystem.h"

using namespace std;

PointCloudSystem::PointCloudSystem()
{
	frustum = Singleton::GetInstance<FrustumClass>();
	boneBuffer = nullptr;
}

PointCloudSystem::PointCloudSystem(FrustumClass* Frustum, ID3D11Buffer* boneBuf)
{
	frustum = Frustum ? Frustum : Singleton::GetInstance<FrustumClass>();
	boneBuffer = boneBuf;
}

void PointCloudSystem::Initialize()
{
}

void PointCloudSystem::Shutdown()
{
	if (frustum)
	{
		frustum = 0;
	}
}

void PointCloudSystem::Update(EntityStorage& entityStorage, float deltaTime)
{
	Blend::Blending(Blend::blendmode::on, Blend::blendop::add);
	Rasterizer::Cull(Rasterizer::cullmode::off);
	Depth::Depth(Depth::depthmode::readonly);

	const vector<Entity*>& entities = entityStorage.GetEntitiesWithComponent<PointCloud>();
	size_t size = entities.size();
	const int sceneRT = Textures::currentRT;
	bool hasProceduralClouds = false;

	for (int i = 0; i < size; i++)
	{
		Entity* entity = entities[i];
		if (!IsEntityValid(entity))
		{
			continue;
		}

		PointCloud* pointCloud = entity->GetComponent<PointCloud>();
		if (pointCloud && pointCloud->active && pointCloud->procedural)
		{
			hasProceduralClouds = true;
			break;
		}
	}

	if (hasProceduralClouds)
	{
		ClearProceduralTargets();
	}

	for (int i = 0; i < size; i++)
	{
		Entity* entity = entities[i];
		if (!IsEntityValid(entity))
		{
			continue;
		}

		PointCloud* pointCloud = entity->GetComponent<PointCloud>();
		if (pointCloud == nullptr || !pointCloud->active)
		{
			continue;
		}

		Transform cloudTransform = GetPointCloudTransform(entity, pointCloud);
		if (!frustum->CheckSphere(cloudTransform.position, pointCloud->frustumRadius))
		{
			continue;
		}

		if (pointCloud->procedural)
		{
			RenderProceduralPointCloud(pointCloud, cloudTransform);
		}
		else
		{
			RenderMeshPointCloud(entity, pointCloud, cloudTransform);
		}
	}

	if (hasProceduralClouds)
	{
		CompositeProceduralTargets(sceneRT);
	}
}

Transform PointCloudSystem::GetPointCloudTransform(Entity* entity, PointCloud* pointCloud)
{
	Transform cloudTransform;

	if (entity->HasComponent<Transform>())
	{
		cloudTransform = GetWorldTransform(entity);
	}

	cloudTransform.position += (cloudTransform.GetRightVector() * pointCloud->position.x +
		cloudTransform.GetUpVector() * pointCloud->position.y +
		cloudTransform.GetLookVector() * pointCloud->position.z) * cloudTransform.scale;
	cloudTransform.scale *= pointCloud->scale;
	cloudTransform.mRotation = pointCloud->mRotation * cloudTransform.mRotation;

	return cloudTransform;
}

void PointCloudSystem::RenderMeshPointCloud(Entity* entity, PointCloud* pointCloud, const Transform& cloudTransform)
{
	UpdateWorldMatrix(cloudTransform);

	ConstBuf::drawerV[0] = pointCloud->pointSize;
	ConstBuf::global[0] = XMFLOAT4(pointCloud->color.x, pointCloud->color.y, pointCloud->color.z,
		pointCloud->brightness);
	ConstBuf::Update(0, ConstBuf::drawerV);
	ConstBuf::Update(5, ConstBuf::global);
	ConstBuf::ConstToGeometry(0);
	ConstBuf::ConstToPixel(5);

	const int lastRT = Textures::currentRT;

	ConstBuf::drawerInt[0] = pow(2, (int)pointCloud->compress);
	ConstBuf::Update(7, ConstBuf::drawerInt);
	ConstBuf::ConstToPixel(7);

	if (pointCloud->compress != RenderCompress::none)
	{
		auto [uavIndex, rtIndex, csIndex] = Textures::GetCompressRes(pointCloud->compress);

		Textures::RenderTarget(rtIndex, 0);
		Draw::Clear({ 0.0f, 0.0f, 0.0f, 0.0f });
		Draw::ClearDepth();

		ConstBuf::ConstToCompute(7);

		Compute::Dispatch(csIndex, lastRT, uavIndex);
		Textures::TextureToShader(uavIndex, 0);

		Sampler::SamplerComp(0);

		BindBones(entity);

		Shaders::vShader(pointCloud->vShader);
		Shaders::gShader(pointCloud->gShader);
		Shaders::pShader(pointCloud->pShader);

		InputAssembler::IA(InputAssembler::topology::pointList);
		InputAssembler::vBuffer(pointCloud->index);
		context->DrawIndexedInstanced(Models::Model[pointCloud->index].indexes, pointCloud->instances, 0, 0, 0);

		Textures::CreateMipMap();

		Textures::RenderTarget(lastRT, 0);

		Textures::TextureToShader(rtIndex, 0, targetshader::pixel);

		Shaders::vShader(10);
		Shaders::gShader(0);
		Shaders::pShader(100);

		InputAssembler::IA(InputAssembler::topology::triList);
		context->Draw(6, 0);
	}
	else
	{
		BindBones(entity);

		Shaders::vShader(pointCloud->vShader);
		Shaders::gShader(pointCloud->gShader);
		Shaders::pShader(pointCloud->pShader);

		InputAssembler::IA(InputAssembler::topology::pointList);
		InputAssembler::vBuffer(pointCloud->index);
		context->DrawIndexedInstanced(Models::Model[pointCloud->index].indexes, pointCloud->instances, 0, 0, 0);
	}
}

void PointCloudSystem::RenderProceduralPointCloud(PointCloud* pointCloud, const Transform& cloudTransform)
{
	int gX = sqrt(pointCloud->count / pointCloud->skipper);
	int gY = sqrt(pointCloud->count / pointCloud->skipper);

	if (gX <= 0 || gY <= 0)
	{
		return;
	}

	ConstBuf::nebulaInfo.model = GetWorldMatrix(cloudTransform);
	ConstBuf::nebulaInfo.gX = gX;
	ConstBuf::nebulaInfo.gY = gY;
	ConstBuf::nebulaInfo.mode = (int)pointCloud->mode;
	ConstBuf::nebulaInfo.skipper = pointCloud->skipper;
	ConstBuf::nebulaInfo.base_color = XMFLOAT4(pointCloud->color.x, pointCloud->color.y, pointCloud->color.z,
		pointCloud->brightness);
	ConstBuf::nebulaInfo.scale = pointCloud->scale.x;

	ConstBuf::pointCloudInfo.model = ConstBuf::nebulaInfo.model;
	ConstBuf::pointCloudInfo.gX = gX;
	ConstBuf::pointCloudInfo.gY = gY;
	ConstBuf::pointCloudInfo.mode = (int)pointCloud->mode;
	ConstBuf::pointCloudInfo.skipper = pointCloud->skipper;
	ConstBuf::pointCloudInfo.base_color = ConstBuf::nebulaInfo.base_color;

	ConstBuf::UpdateNebulaInfo();
	ConstBuf::UpdatePointCloudInfo();
	ConstBuf::ConstToVertex(11);

	ConstBuf::drawerInt[0] = 1;
	ConstBuf::Update(7, ConstBuf::drawerInt);
	ConstBuf::ConstToPixel(7);

	Textures::RenderTarget(GetProceduralTarget(pointCloud->mode), 0);
	RenderProceduralPass(pointCloud, gX, gY, false);
}

void PointCloudSystem::RenderProceduralPass(PointCloud* pointCloud, int gX, int gY, bool isBackground)
{
	Shaders::vShader(pointCloud->vShader);
	Shaders::gShader(pointCloud->gShader);
	PSModeSet(pointCloud->mode, isBackground);

	context->VSSetConstantBuffers(0, 1, &ConstBuf::buffer[12]);

	InputAssembler::IA(pointCloud->topology);
	context->DrawInstanced(6, (int)gX * (int)gY, 0, 0);
}

void PointCloudSystem::ClearProceduralTargets()
{
	const int lastRT = Textures::currentRT;
	const int targets[] = {
		Textures::TextureName["ZodiacPBuf"],
		Textures::TextureName["ZodiacPBufMid"],
		Textures::TextureName["ZodiacPBufLow"],
	};

	for (int target : targets)
	{
		Textures::RenderTarget(target, 0);
		Draw::Clear({ 0.0f, 0.0f, 0.0f, 0.0f });
	}

	Textures::RenderTarget(lastRT, 0);
}

void PointCloudSystem::CompositeProceduralTargets(int sceneRT)
{
	Textures::RenderTarget(sceneRT, 0);
	Blend::Blending(Blend::blendmode::on, Blend::blendop::add);
	Depth::Depth(Depth::depthmode::off);
	Rasterizer::Cull(Rasterizer::cullmode::off);

	Textures::TextureToShader("ZodiacPBuf", 0, targetshader::pixel);
	Textures::TextureToShader("ZodiacPBufMid", 1, targetshader::pixel);
	Textures::TextureToShader("ZodiacPBufLow", 2, targetshader::pixel);
	Sampler::Sampler(targetshader::pixel, 0, Sampler::filter::linear, Sampler::addr::wrap, Sampler::addr::wrap);

	Shaders::vShader(10);
	Shaders::gShader(0);
	Shaders::pShader(35);

	InputAssembler::IA(InputAssembler::topology::triList);
	context->Draw(6, 0);
}

int PointCloudSystem::GetProceduralTarget(pMode mode)
{
	if (mode == pMode::glow)
	{
		return Textures::TextureName["ZodiacPBufMid"];
	}

	return Textures::TextureName["ZodiacPBuf"];
}

void PointCloudSystem::UpdateWorldMatrix(Transform worldTransform)
{
	ConstBuf::camera.world = GetWorldMatrix(worldTransform);
	ConstBuf::UpdateCamera();
	ConstBuf::ConstToVertex(3);
	ConstBuf::ConstToPixel(3);
}

void PointCloudSystem::BindBones(Entity* entity)
{
	static vector<XMMATRIX> identityPalette(128, XMMatrixIdentity());
	SkeletalAnimationComponent* animComp = entity->GetComponent<SkeletalAnimationComponent>();
	if (boneBuffer)
	{
		if (animComp && !animComp->bonePalette.empty())
		{
			context->UpdateSubresource(boneBuffer, 0, nullptr, animComp->bonePalette.data(), 0, 0);
		}
		else
		{
			context->UpdateSubresource(boneBuffer, 0, nullptr, identityPalette.data(), 0, 0);
		}

		context->VSSetConstantBuffers(1, 1, &boneBuffer);
	}
}

void PointCloudSystem::PSModeSet(pMode mode, bool isBackground)
{
	switch (mode)
	{
	case pMode::point:
	{
		Shaders::pShader(33);
		break;
	}
	case pMode::glow:
	{
		Shaders::pShader(34);
		break;
	}
	}
}
