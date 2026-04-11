#include "nebulaSystem.h"

using namespace std;


NebulaSystem::NebulaSystem()
{
	frustum = Singleton::GetInstance<FrustumClass>();
}


void NebulaSystem::Initialize()
{
}


void NebulaSystem::Shutdown()
{
	if (frustum)
	{
		frustum = 0;
	}
}


void NebulaSystem::Update(EntityStorage& entityStorage, float deltaTime)
{
	Blend::Blending(Blend::blendmode::on, Blend::blendop::add);
	Rasterizer::Cull(Rasterizer::cullmode::off);
	Depth::Depth(Depth::depthmode::readonly);

	const std::vector<Entity*>& entities = entityStorage.GetEntitiesWithComponent<Nebula>();
	size_t size = entities.size();
	for (int i = 0; i < size; i++)
	{
		Entity* entity = entities[i];
		if (!IsEntityValid(entity))
		{
			continue;
		}

		Shaders::gShader(0);
		InputAssembler::IA(InputAssembler::topology::triList);

		Nebula* nebula = entity->GetComponent<Nebula>();
		if (nebula != nullptr && nebula->active)
		{
			Transform* transform = entity->GetComponent<Transform>();
			Transform worldTransform = GetWorldTransform(entity);

			if (transform == nullptr || frustum->CheckSphere(worldTransform.position, nebula->frustumRadius))
			{
				int gX = sqrt(nebula->count / nebula->skipper);
				int gY = sqrt(nebula->count / nebula->skipper);

				if (transform != nullptr)
				{
					ConstBuf::nebulaInfo.model = GetWorldMatrix(worldTransform);
				}
				else {
					ConstBuf::nebulaInfo.model = XMMatrixTranspose(XMMatrixTranslation(0, 0, 0));
				}

				ConstBuf::nebulaInfo.gX = gX;
				ConstBuf::nebulaInfo.gY = gY;
				ConstBuf::nebulaInfo.mode = (int)nebula->mode;
				ConstBuf::nebulaInfo.skipper = nebula->skipper;
				ConstBuf::nebulaInfo.base_color = XMFLOAT4(nebula->color.x, nebula->color.y, nebula->color.z, 1);
				ConstBuf::nebulaInfo.scale = nebula->scale;

				ConstBuf::UpdateNebulaInfo();
				ConstBuf::ConstToVertex(11);

				ConstBuf::drawerV[0] = (float)entity->localTime * 0.01f;
				ConstBuf::Update(0, ConstBuf::drawerV);
				ConstBuf::ConstToVertex(0);
				ConstBuf::ConstToPixel(0);

				int lastRT = Textures::currentRT;

				ConstBuf::drawerInt[0] = pow(2, (int)nebula->compress);
				ConstBuf::Update(7, ConstBuf::drawerInt);
				ConstBuf::ConstToPixel(7);

				if (nebula->compress != RenderCompress::none || nebula->isOnBackground)
				{
					int uavIndex;
					int rtIndex;
					int csIndex;
					if (nebula->isOnBackground) {
						uavIndex = 11;
						rtIndex = 12;
						csIndex = 1;
					}
					else {
						uavIndex = (int)nebula->compress * 2 + 1;
						rtIndex = (int)nebula->compress * 2 + 2;
						csIndex = 0;
					}

					Textures::RenderTarget(rtIndex, 0);
					Draw::Clear({ 0.0f, 0.0f, 0.0f, 0.0f });
					Draw::ClearDepth();

					ConstBuf::ConstToCompute(7);

					Compute::Dispatch(csIndex, lastRT, uavIndex);
					Textures::TextureToShader(uavIndex, 0);

					Sampler::SamplerComp(0);

					Shaders::vShader(nebula->vShader);
					Shaders::gShader(nebula->gShader);
					PSModeSet(nebula->mode, nebula->isOnBackground);

					InputAssembler::IA(nebula->topology);
					Draw::NullDrawer(1, (int)gX * (int)gY);

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
					Shaders::vShader(nebula->vShader);
					Shaders::gShader(nebula->gShader);
					PSModeSet(nebula->mode, nebula->isOnBackground);

					InputAssembler::IA(nebula->topology);
					Draw::NullDrawer(1, (int)gX * (int)gY);
				}
			}
		}

	}
}


void NebulaSystem::PSModeSet(pMode mode, bool isBackground)
{
	switch (mode)
	{
	case pMode::point:
	{
		Shaders::pShader(isBackground ? 26 : 23);
		break;
	}
	case pMode::glow:
	{
		Shaders::pShader(isBackground ? 27 : 24);
		break;
	}
	}
}
