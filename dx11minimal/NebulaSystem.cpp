#include "nebulaSystem.h"


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


void NebulaSystem::Update(vector<Entity*>& entities, float deltaTime)
{
	// Clear the buffers to begin the scene.
	/*Draw::Clear({ 0.0f, 0.0588f, 0.1176f, 1.0f });
	Draw::ClearDepth();*/

	Blend::Blending(Blend::blendmode::on, Blend::blendop::add);
	Rasterizer::Cull(Rasterizer::cullmode::off);
	Depth::Depth(Depth::depthmode::readonly);

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
			if (transform == nullptr || frustum->CheckSphere(transform->position, nebula->frustumRadius))
			{
				int gX = sqrt(nebula->count / nebula->skipper);
				int gY = sqrt(nebula->count / nebula->skipper);

				ConstBuf::locationInfo.model = XMMatrixTranspose(XMMatrixTranslation(0, 0, 0));
				ConstBuf::locationInfo.gX = gX;
				ConstBuf::locationInfo.gY = gY;
				ConstBuf::locationInfo.mode = (int)nebula->mode;
				ConstBuf::locationInfo.skipper = nebula->skipper;
				ConstBuf::locationInfo.base_color = XMFLOAT4(nebula->color.x, nebula->color.y, nebula->color.z, 1);

				ConstBuf::UpdateLocationInfo();
				ConstBuf::ConstToVertex(11);

				ConstBuf::drawerV[0] = (float)entity->localTime * 0.01f;
				ConstBuf::Update(0, ConstBuf::drawerV);
				ConstBuf::ConstToVertex(0);
				ConstBuf::ConstToPixel(0);

				if (transform != nullptr)
				{
					ConstBuf::global[0] = XMFLOAT4(transform->position.x, transform->position.y,
						transform->position.z, transform->scale.x);
					ConstBuf::Update(5, ConstBuf::global);
					ConstBuf::ConstToVertex(5);
					ConstBuf::ConstToPixel(5);
					ConstBuf::ConstToGeometry(5);
				}

				int lastRT = Textures::currentRT;

				ConstBuf::drawerInt[0] = pow(2, (int)nebula->compress);
				ConstBuf::Update(7, ConstBuf::drawerInt);
				ConstBuf::ConstToPixel(7);

				if (nebula->compress != RenderCompress::none)
				{
					int uavIndex = (int)nebula->compress * 2 + 1;
					int rtIndex = (int)nebula->compress * 2 + 2;

					Textures::RenderTarget(rtIndex, 0);
					Draw::Clear({ 0.0f, 0.0f, 0.0f, 0.0f });
					Draw::ClearDepth();

					ConstBuf::ConstToCompute(7);

					Compute::Dispatch(0, lastRT, uavIndex);
					Textures::TextureToShader(uavIndex, 0);

					Sampler::SamplerComp(0);

					Shaders::vShader(nebula->vShader);
					Shaders::gShader(nebula->gShader);
					PSModeSet(nebula->mode);

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
					PSModeSet(nebula->mode);

					InputAssembler::IA(nebula->topology);
					Draw::NullDrawer(1, (int)gX * (int)gY);
				}
			}
		}

	}
}


void NebulaSystem::PSModeSet(pMode mode)
{
	switch (mode)
	{
	case pMode::point:
	{
		Shaders::pShader(23);
		break;
	}
	case pMode::glow:
	{
		Shaders::pShader(24);
		break;
	}
	}
}