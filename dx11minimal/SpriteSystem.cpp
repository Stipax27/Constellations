#include "spriteSystem.h"


SpriteSystem::SpriteSystem(FrustumClass* Frustum)
{
	frustum = Frustum;
}


void SpriteSystem::Initialize()
{
}


void SpriteSystem::Shutdown()
{
	if (frustum) {
		frustum = 0;
	}
}


void SpriteSystem::Update(vector<Entity*>& entities, float deltaTime)
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
		if (IsEntityValid(entity))
		{
			Shaders::gShader(0);
			InputAssembler::IA(InputAssembler::topology::triList);

			Transform* transform = entity->GetComponent<Transform>();

			if (transform != nullptr)
			{
				Transform worldTransform = GetWorldTransform(entity);

				Constellation* constellation = entity->GetComponent<Constellation>();
				if (constellation != nullptr && constellation->active) {
					point3d transformPos = transform->position;
					vector<point3d> transformedStars;
					int count;

					ConstBuf::global[constCount - 1] = XMFLOAT4(1, 1, 1, 1);

					for (int a = 0; a < constellation->stars.size(); a++) {
						point3d star = constellation->stars[a];
						star = transformPos + transform->GetLookVector() * star.z + transform->GetRightVector() * star.x + transform->GetUpVector() * star.y;

						transformedStars.push_back(star);
					}

					count = 0;
					for (int a = 0; a < constellation->links.size() && count < constCount / 2 - 1; a++) {
						pair<int, int> link = constellation->links[a];
						point3d point1 = transformedStars[link.first];
						point3d point2 = transformedStars[link.second];

						if (frustum->CheckSphere(point1.lerp(point2, 0.5f), max((point1 - point2).magnitude(), constellation->linkSize))) {
							ConstBuf::global[count * 2] = XMFLOAT4(point1.x, point1.y, point1.z, constellation->linkSize);
							ConstBuf::global[count * 2 + 1] = XMFLOAT4(point2.x, point2.y, point2.z, constellation->linkSize);
							count++;
						}
					}

					if (count > 0) {
						Shaders::vShader(4);
						Shaders::pShader(4);

						ConstBuf::Update(5, ConstBuf::global);
						ConstBuf::ConstToVertex(5);

						context->DrawInstanced(6, min(count, constCount / 2 - 1), 0, 0);
					}

					int n = 12;
					ConstBuf::drawerV[0] = n;

					count = 0;
					for (int a = 0; a < transformedStars.size() && count < constCount - 2; a++) {
						point3d star = transformedStars[a];
						if (frustum->CheckSphere(star, transform->scale.x)) {
							ConstBuf::global[count] = XMFLOAT4(star.x, star.y, star.z, constellation->crownRadius);
							count++;
						}
					}

					ConstBuf::global[constCount - 1] = XMFLOAT4(constellation->crownColor.x, constellation->crownColor.y, constellation->crownColor.z, 1);

					if (count > 0) {
						Shaders::vShader(20);
						Shaders::pShader(20);

						ConstBuf::Update(0, ConstBuf::drawerV);
						ConstBuf::Update(5, ConstBuf::global);
						ConstBuf::ConstToVertex(0);
						ConstBuf::ConstToVertex(5);

						context->DrawInstanced(n * 3, min(count, constCount - 2), 0, 0);
					}
				}

				Star* star = entity->GetComponent<Star>();
				if (star != nullptr && star->active) {
					if (frustum->CheckSphere(worldTransform.position, star->radius)) {

						ConstBuf::global[0] = XMFLOAT4(worldTransform.position.x, worldTransform.position.y, worldTransform.position.z, star->crownRadius);
						ConstBuf::global[constCount - 1] = XMFLOAT4(star->crownColor.x, star->crownColor.y, star->crownColor.z, 1);
						ConstBuf::Update(5, ConstBuf::global);
						ConstBuf::ConstToVertex(5);

						Shaders::vShader(20);
						Shaders::pShader(20);

						int n = 12;

						ConstBuf::drawerV[0] = n;
						ConstBuf::Update(0, ConstBuf::drawerV);
						ConstBuf::ConstToVertex(0);

						context->Draw(n * 3, 0);
					}
				}

				PointCloud* pointCloud = entity->GetComponent<PointCloud>();
				if (pointCloud != nullptr && pointCloud->active) {
					Transform cloudTransform = worldTransform;

					cloudTransform.position += (cloudTransform.GetRightVector() * pointCloud->position.x + cloudTransform.GetUpVector() * pointCloud->position.y + cloudTransform.GetLookVector() * pointCloud->position.z) * cloudTransform.scale;
					cloudTransform.scale *= pointCloud->scale;
					cloudTransform.mRotation = pointCloud->mRotation * cloudTransform.mRotation;


					//if (frustum->CheckSphere(worldTransform.position, worldTransform.scale.magnitude())) {
						//ConstBuf::CreateVertexBuffer(15);

					UpdateWorldMatrix(cloudTransform);

					ConstBuf::drawerV[0] = pointCloud->pointSize;
					ConstBuf::global[0] = XMFLOAT4(pointCloud->color.x, pointCloud->color.y, pointCloud->color.z, pointCloud->brightness);
					ConstBuf::Update(0, ConstBuf::drawerV);
					ConstBuf::Update(5, ConstBuf::global);
					ConstBuf::ConstToGeometry(0);
					ConstBuf::ConstToPixel(5);

					//Rasterizer::Cull(Rasterizer::cullmode::front);

					//Shaders::vShader(17);
					//Shaders::pShader(17);
					////Shaders::gShader(17);

					//InputAssembler::IA(InputAssembler::topology::triList);
					////context->DrawIndexed(16777216, 0, 0);
					//context->DrawInstanced(6, 2097152, 0, 0);


					Shaders::vShader(17);
					Shaders::pShader(17);
					Shaders::gShader(17);

					InputAssembler::IA(InputAssembler::topology::pointList);
					InputAssembler::vBuffer(pointCloud->index);

					context->DrawIndexedInstanced(Models::Model[pointCloud->index].indexes, pointCloud->instances, 0, 0, 0);
					//context->DrawInstanced(1, 2097152 / 6, 0, 0);
				//}

				}

				ParticleEmitter* particleEmitter = entity->GetComponent<ParticleEmitter>();
				if (particleEmitter != nullptr) {

					if (particleEmitter->active) {
						double emitDelta = 1000 / particleEmitter->rate;
						double elapsedTime = timer::currentTime - particleEmitter->lastEmitTime;
						if (elapsedTime >= emitDelta)
						{
							int count = min((int)(elapsedTime / emitDelta), constCount);
							for (int i = 0; i < count; i++)
							{
								double startTime = particleEmitter->lastEmitTime + emitDelta * (i + 1);
								particleEmitter->particles.push_back(XMFLOAT4(worldTransform.position.x, worldTransform.position.y, worldTransform.position.z, (float)startTime));
							}

							particleEmitter->lastEmitTime += emitDelta * count;
						}
					}

					int i = 0;
					while (i < particleEmitter->particles.size())
					{
						float startTime = particleEmitter->particles[i].w;

						if (timer::currentTime - startTime < particleEmitter->lifetime)
						{
							if (i < constCount) {
								ConstBuf::global[i] = particleEmitter->particles[i];
							}

							i++;
						}
						else
						{
							particleEmitter->particles.erase(particleEmitter->particles.begin() + i);
						}
					}

					ConstBuf::Update(5, ConstBuf::global);
					ConstBuf::ConstToVertex(5);

					size_t size = particleEmitter->particles.size();
					if (size > 0) {
						Shaders::vShader(particleEmitter->vShader);
						Shaders::pShader(particleEmitter->pShader);
						Shaders::gShader(particleEmitter->gShader);

						/*for (int i = 0; i < size; i++) {
							ConstBuf::global[i + 1].w = (float)particleEmitter->particles[i];
						}*/

						ConstBuf::Update(5, ConstBuf::global);
						ConstBuf::ConstToVertex(5);
						ConstBuf::ConstToPixel(5);
						ConstBuf::ConstToGeometry(5);

						ConstBuf::particlesInfo.size = XMFLOAT2(particleEmitter->size.first, particleEmitter->size.second);
						ConstBuf::particlesInfo.opacity = XMFLOAT2(particleEmitter->opacity.first, particleEmitter->opacity.second);
						ConstBuf::particlesInfo.color = XMFLOAT3(particleEmitter->color.x, particleEmitter->color.y, particleEmitter->color.z);
						ConstBuf::particlesInfo.lifetime = particleEmitter->lifetime;

						ConstBuf::UpdateParticlesInfo();
						ConstBuf::ConstToVertex(9);
						ConstBuf::ConstToPixel(9);
						ConstBuf::ConstToGeometry(9);

						if (particleEmitter->gShader == 0) {
							InputAssembler::IA(InputAssembler::topology::triList);
						}
						else {
							InputAssembler::IA(InputAssembler::topology::pointList);
						}

						context->DrawInstanced(6, min(size, constCount), 0, 0);
					}
				}
			}

			SpriteCluster* spriteCluster = entity->GetComponent<SpriteCluster>();
			if (spriteCluster != nullptr && spriteCluster->active) {
				if (transform == nullptr || frustum->CheckSphere(transform->position, spriteCluster->frustumRadius)) {
					ConstBuf::drawerV[0] = entity->timeScale;
					ConstBuf::Update(0, ConstBuf::drawerV);
					ConstBuf::ConstToVertex(0);
					ConstBuf::ConstToPixel(0);

					if (transform != nullptr)
					{
						ConstBuf::global[0] = XMFLOAT4(transform->position.x, transform->position.y, transform->position.z, transform->scale.x);
						ConstBuf::Update(5, ConstBuf::global);
						ConstBuf::ConstToVertex(5);
						ConstBuf::ConstToPixel(5);
						ConstBuf::ConstToGeometry(5);
					}

					int lastRT = Textures::currentRT;

					ConstBuf::drawerInt[0] = pow(2, (int)spriteCluster->compress);
					ConstBuf::Update(7, ConstBuf::drawerInt);
					ConstBuf::ConstToPixel(7);

					if (spriteCluster->compress != RenderCompress::none) {

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
					else {
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
}


XMMATRIX SpriteSystem::GetWorldMatrix(Transform worldTransform) {
	XMMATRIX rotateMatrix = worldTransform.mRotation;
	XMMATRIX scaleMatrix = XMMatrixScaling(worldTransform.scale.x, worldTransform.scale.y, worldTransform.scale.z);
	XMMATRIX translateMatrix = XMMatrixTranslation(worldTransform.position.x, worldTransform.position.y, worldTransform.position.z);

	// Multiply the scale, rotation, and translation matrices together to create the final world transformation matrix.
	XMMATRIX srMatrix = scaleMatrix * rotateMatrix;
	XMMATRIX worldMatrix = srMatrix * translateMatrix;

	return XMMatrixTranspose(worldMatrix);
}

void SpriteSystem::UpdateWorldMatrix(Transform worldTransform) {
	ConstBuf::camera.world = GetWorldMatrix(worldTransform);
	ConstBuf::UpdateCamera();
	ConstBuf::ConstToVertex(3);
	ConstBuf::ConstToPixel(3);
}