#include "nebulaSystem.h"


NebulaSystem::NebulaSystem(FrustumClass* Frustum)
{
	frustum = Frustum;
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

		Transform* transform = entity->GetComponent<Transform>();

		Nebula* nebula = entity->GetComponent<Nebula>();
		if (nebula != nullptr && nebula->active)
		{
			if (transform == nullptr || frustum->CheckSphere(transform->position, nebula->frustumRadius))
			{
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

				ConstBuf::drawerInt[0] = pow(2, (int)spriteCluster->compress);
				ConstBuf::Update(7, ConstBuf::drawerInt);
				ConstBuf::ConstToPixel(7);

				if (spriteCluster->compress != RenderCompress::none)
				{
					int uavIndex = (int)spriteCluster->compress * 2 + 1;
					int rtIndex = (int)spriteCluster->compress * 2 + 2;

					Textures::RenderTarget(rtIndex, 0);
					Draw::Clear({ 0.0f, 0.0f, 0.0f, 0.0f });
					Draw::ClearDepth();

					//Depth::Depth(Depth::depthmode::on);

					ConstBuf::ConstToCompute(7);

					Compute::Dispatch(0, lastRT, uavIndex);
					Textures::TextureToShader(uavIndex, 0);

					//Depth::Depth(Depth::depthmode::off);


					//Textures::DepthTarget(lastRT, 0);

					/*Shaders::vShader(10);
					Shaders::pShader(101);
					context->PSSetShaderResources(0, 1, &Textures::Texture[lastRT].DepthResView);
					context->Draw(6, 0);*/

					//Depth::Depth(Depth::depthmode::readonly);

					Sampler::SamplerComp(0);

					Shaders::vShader(spriteCluster->vShader);
					Shaders::gShader(spriteCluster->gShader);
					Shaders::pShader(spriteCluster->pShader);

					InputAssembler::IA(spriteCluster->topology);
					context->DrawInstanced(spriteCluster->vertexNum, spriteCluster->pointsNum, 0, 0);

					//Sampler::SamplerComp(0);
					Textures::CreateMipMap();

					Textures::RenderTarget(lastRT, 0);

					Textures::TextureToShader(rtIndex, 0, targetshader::pixel);

					Shaders::vShader(10);
					Shaders::gShader(0);
					Shaders::pShader(100);

					InputAssembler::IA(InputAssembler::topology::triList);
					context->Draw(6, 0);

					//Depth::Depth(Depth::depthmode::readonly);
				}
				else
				{
					Shaders::vShader(spriteCluster->vShader);
					Shaders::gShader(spriteCluster->gShader);
					Shaders::pShader(spriteCluster->pShader);

					InputAssembler::IA(spriteCluster->topology);
					context->DrawInstanced(spriteCluster->vertexNum, spriteCluster->pointsNum, 0, 0);
				}
			}
		}

		//Explosion* explosion = entity->GetComponent<Explosion>();

		//if (explosion != nullptr) {

		//	Shaders::vShader(1);
		//	Shaders::pShader(1);

		//	transform->position;
		//	explosion->radius = min(explosion->max_radius, explosion->radius + explosion->speed * deltaTime);

		//	ConstBuf::global[0] = XMFLOAT4(transform->position.x, transform->position.y, transform->position.z, explosion->radius);
		//	ConstBuf::Update(5, ConstBuf::global);
		//	ConstBuf::ConstToVertex(5);
		//	ConstBuf::ConstToPixel(5);

		//	Draw::Drawer(1);
		//	if (timer::currentTime - explosion->lifeStartTime >= explosion->lifeTime)
		//	{
		//		//entity->RemoveComponent<Explosion>();
		//		entity->SetActive(false);
		//	}
		//}
	}
}


XMMATRIX NebulaSystem::GetWorldMatrix(Transform worldTransform)
{
	XMMATRIX rotateMatrix = worldTransform.mRotation;
	XMMATRIX scaleMatrix = XMMatrixScaling(worldTransform.scale.x, worldTransform.scale.y, worldTransform.scale.z);
	XMMATRIX translateMatrix = XMMatrixTranslation(worldTransform.position.x, worldTransform.position.y,
		worldTransform.position.z);

	// Multiply the scale, rotation, and translation matrices together to create the final world transformation matrix.
	XMMATRIX srMatrix = scaleMatrix * rotateMatrix;
	XMMATRIX worldMatrix = srMatrix * translateMatrix;

	return XMMatrixTranspose(worldMatrix);
}

void NebulaSystem::UpdateWorldMatrix(Transform worldTransform)
{
	ConstBuf::camera.world = GetWorldMatrix(worldTransform);
	ConstBuf::UpdateCamera();
	ConstBuf::ConstToVertex(3);
	ConstBuf::ConstToPixel(3);
}