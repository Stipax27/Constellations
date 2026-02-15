#include "spriteSystem.h"
#include "Components/Orientation.h"


SpriteSystem::SpriteSystem(FrustumClass* Frustum)
{
	frustum = Frustum;
}


void SpriteSystem::Initialize()
{
}


void SpriteSystem::Shutdown()
{
	if (frustum)
	{
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
		if (!IsEntityValid(entity))
		{
			continue;
		}

		Shaders::gShader(0);
		InputAssembler::IA(InputAssembler::topology::triList);

		Transform* transform = entity->GetComponent<Transform>();

		if (transform != nullptr)
		{
			Transform worldTransform = GetWorldTransform(entity);

			Constellation* constellation = entity->GetComponent<Constellation>();
			if (constellation != nullptr && constellation->active)
			{
				point3d transformPos = transform->position;
				vector<point3d> transformedStars;
				int count;

				for (int a = 0; a < constellation->stars.size(); a++)
				{
					point3d star = constellation->stars[a];
					star = transformPos + transform->GetLookVector() * star.z + transform->GetRightVector() * star.x
						+ transform->GetUpVector() * star.y;

					transformedStars.push_back(star);
				}

				count = 0;
				for (int a = 0; a < constellation->links.size() && count < constCount; a++)
				{
					pair<int, int> link = constellation->links[a];
					point3d point1 = transformedStars[link.first];
					point3d point2 = transformedStars[link.second];

					if (frustum->CheckSphere(point1.lerp(point2, 0.5f),
					                         max((point1 - point2).magnitude(), constellation->linkSize)))
					{
						/*ConstBuf::global[count * 2] = XMFLOAT4(point1.x, point1.y, point1.z, constellation->linkSize);
						ConstBuf::global[count * 2 + 1] = XMFLOAT4(point2.x, point2.y, point2.z, constellation->linkSize);*/

						ConstBuf::drawerFloat4x4[count] = XMFLOAT4X4(
							point1.x, point1.y, point1.z, constellation->linkSize,
							point2.x, point2.y, point2.z, constellation->linkSize,
							1, 1, 1, 1,
							1, 1, 1, 1
						);
						count++;
					}
				}

				if (count > 0)
				{
					Shaders::vShader(4);
					Shaders::pShader(4);

					/*ConstBuf::Update(5, ConstBuf::global);
					ConstBuf::ConstToVertex(5);*/

					ConstBuf::Update(10, ConstBuf::drawerFloat4x4);
					ConstBuf::ConstToVertex(10);
					ConstBuf::ConstToPixel(10);

					context->DrawInstanced(6, min(count, constCount - 1), 0, 0);
				}

				int n = 12;
				ConstBuf::drawerV[0] = n;

				count = 0;
				for (int a = 0; a < transformedStars.size() && count < constCount - 2; a++)
				{
					point3d star = transformedStars[a];
					if (frustum->CheckSphere(star, transform->scale.x))
					{
						ConstBuf::global[count + 1] = XMFLOAT4(star.x, star.y, star.z, constellation->crownRadius);
						count++;
					}
				}

				ConstBuf::global[0] = XMFLOAT4(constellation->crownColor.x, constellation->crownColor.y,
				                               constellation->crownColor.z, 1);

				if (count > 0)
				{
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
			if (star != nullptr && star->active)
			{
				if (frustum->CheckSphere(worldTransform.position, star->radius))
				{
					ConstBuf::global[0] = XMFLOAT4(star->crownColor.x, star->crownColor.y, star->crownColor.z, 1);
					ConstBuf::global[1] = XMFLOAT4(worldTransform.position.x, worldTransform.position.y,
					                               worldTransform.position.z, star->crownRadius);
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
			if (pointCloud != nullptr && pointCloud->active)
			{
				Transform cloudTransform = worldTransform;

				cloudTransform.position += (cloudTransform.GetRightVector() * pointCloud->position.x +
					cloudTransform.GetUpVector() * pointCloud->position.y + cloudTransform.GetLookVector() *
					pointCloud->position.z) * cloudTransform.scale;
				cloudTransform.scale *= pointCloud->scale;
				cloudTransform.mRotation = pointCloud->mRotation * cloudTransform.mRotation;


				if (frustum->CheckSphere(cloudTransform.position, pointCloud->frustumRadius)) {
					UpdateWorldMatrix(cloudTransform);

					ConstBuf::drawerV[0] = pointCloud->pointSize;
					ConstBuf::global[0] = XMFLOAT4(pointCloud->color.x, pointCloud->color.y, pointCloud->color.z,
												   pointCloud->brightness);
					ConstBuf::Update(0, ConstBuf::drawerV);
					ConstBuf::Update(5, ConstBuf::global);
					ConstBuf::ConstToGeometry(0);
					ConstBuf::ConstToPixel(5);

					int lastRT = Textures::currentRT;

					ConstBuf::drawerInt[0] = pow(2, (int)pointCloud->compress);
					ConstBuf::Update(7, ConstBuf::drawerInt);
					ConstBuf::ConstToPixel(7);

					if (pointCloud->compress != RenderCompress::none)
					{
						int uavIndex = (int)pointCloud->compress * 2 + 1;
						int rtIndex = (int)pointCloud->compress * 2 + 2;

						Textures::RenderTarget(rtIndex, 0);
						Draw::Clear({ 0.0f, 0.0f, 0.0f, 0.0f });
						Draw::ClearDepth();

						ConstBuf::ConstToCompute(7);

						Compute::Dispatch(0, lastRT, uavIndex);
						Textures::TextureToShader(uavIndex, 0);

						Sampler::SamplerComp(0);

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
						Shaders::vShader(pointCloud->vShader);
						Shaders::gShader(pointCloud->gShader);
						Shaders::pShader(pointCloud->pShader);

						InputAssembler::IA(InputAssembler::topology::pointList);
						InputAssembler::vBuffer(pointCloud->index);
						context->DrawIndexedInstanced(Models::Model[pointCloud->index].indexes, pointCloud->instances, 0, 0, 0);
					}
				}
			}

			ProcessParticle(entity, worldTransform);

			Beam* beam = entity->GetComponent<Beam>();
			if (beam != nullptr && beam->active)
			{
				Transform wTransform1 = Transform();
				wTransform1.position = beam->point1;
				wTransform1 = worldTransform + wTransform1;

				Transform wTransform2 = Transform();
				wTransform2.position = beam->point2;
				wTransform2 = worldTransform + wTransform2;

				if (frustum->CheckSphere(wTransform1.position.lerp(wTransform2.position, 0.5f),
				                         max((wTransform1.position - wTransform2.position).magnitude(),
				                             max(beam->size1, beam->size2))))
				{
					ConstBuf::drawerFloat4x4[0] = XMFLOAT4X4(
						wTransform1.position.x, wTransform1.position.y, wTransform1.position.z, beam->size1,
						wTransform2.position.x, wTransform2.position.y, wTransform2.position.z, beam->size2,
						beam->color1.x, beam->color1.y, beam->color1.z, beam->opacity1,
						beam->color2.x, beam->color2.y, beam->color2.z, beam->opacity2
					);

					ConstBuf::Update(10, ConstBuf::drawerFloat4x4);
					ConstBuf::ConstToVertex(10);
					ConstBuf::ConstToPixel(10);

					Shaders::vShader(4);
					Shaders::gShader(0);
					Shaders::pShader(beam->pShader);

					InputAssembler::IA(InputAssembler::topology::triList);
					context->Draw(6, 0);
				}
			}
		}

		SpriteCluster* spriteCluster = entity->GetComponent<SpriteCluster>();
		if (spriteCluster != nullptr && spriteCluster->active)
		{
			if (transform == nullptr || frustum->CheckSphere(transform->position, spriteCluster->frustumRadius))
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
					Draw::Clear({0.0f, 0.0f, 0.0f, 0.0f});
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


XMMATRIX SpriteSystem::GetWorldMatrix(Transform worldTransform)
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

void SpriteSystem::UpdateWorldMatrix(Transform worldTransform)
{
	ConstBuf::camera.world = GetWorldMatrix(worldTransform);
	ConstBuf::UpdateCamera();
	ConstBuf::ConstToVertex(3);
	ConstBuf::ConstToPixel(3);
}

// Start process particle.
void SpriteSystem::ProcessParticle(Entity* entity, Transform& worldTransform)
{
	ParticleEmitter* particleEmitter = entity->GetComponent<ParticleEmitter>();
	if (!particleEmitter)
	{
		return;
	}

	AddEmitterOrientation(entity, worldTransform);

	if (particleEmitter->active && particleEmitter->rate > 0.0f)
	{
		EmitNewParticles(entity, worldTransform, particleEmitter);
	}

	UpdateExistingParticles(entity, particleEmitter);

	if (!particleEmitter->particles.empty())
	{
		RenderParticles(entity, particleEmitter);
	}
}

void SpriteSystem::AddEmitterOrientation(Entity* entity, Transform& transform)
{
	point3d forward = transform.GetLookVector();
	point3d right = transform.GetRightVector();
	point3d up = transform.GetUpVector();

	Orientation* orientation = entity->AddComponent<Orientation>();
	orientation->SetFromDirections(forward, right, up);
}

void SpriteSystem::EmitNewParticles(Entity* entity, const Transform& worldTransform, ParticleEmitter* emitter)
{
	double emitDelta = CalculateEmitDelta(*emitter);
	double elapsedTime = entity->localTime - emitter->lastEmitTime;

	if (elapsedTime >= emitDelta)
	{
		Orientation* baseOrientation = entity->GetComponent<Orientation>();
		Orientation emitOrientation = CalculateEmitOrientation(*baseOrientation, emitter->emitDirection);

		int particlesToEmit = CalculateEmitCount(*emitter, elapsedTime, emitDelta);

		for (int i = 0; i < particlesToEmit; i++)
		{
			Orientation particleOrientation = ApplySpread(emitOrientation, *emitter);
			double startTime = CalculateParticleStartTime(entity, *emitter, emitDelta, i);

			CreateParticle(worldTransform, emitter, particleOrientation, startTime);
		}

		UpdateEmitTiming(entity, emitter, emitDelta, particlesToEmit);
	}
}

double SpriteSystem::CalculateEmitDelta(const ParticleEmitter& emitter)
{
	double result = emitter.isHeapEmit ? emitter.heapEmitInterval : 1000 / emitter.rate;
	return result;
}

int SpriteSystem::CalculateEmitCount(const ParticleEmitter& emitter, double elapsedTime,
                                     double emitDelta)
{
	if (emitter.isHeapEmit)
	{
		return min(static_cast<int>(emitter.rate), constCount);
	}
	else
	{
		return min(static_cast<int>(elapsedTime / emitDelta), constCount);
	}
}

Orientation SpriteSystem::ApplySpread(Orientation orientation,
                                               const ParticleEmitter& emitter)
{
	if (emitter.spread.first > 0.0f)
	{
		float angle = GenerateRandomAngle(emitter.spread.first);
		orientation.forward = rotateInPlane(orientation.forward, orientation.up, angle);
	}

	if (emitter.spread.second > 0.0f)
	{
		float angle = GenerateRandomAngle(emitter.spread.second);
		orientation.forward = rotateInPlane(orientation.forward, orientation.right, angle);
	}

	return orientation;
}

float SpriteSystem::GenerateRandomAngle(float spread)
{
	float s = spread * 10000;
	float angle = static_cast<float>(getRandom(s)) / 10000;
	float result = getRandom(2) == 0 ? -angle : angle;
	return result;
}

double SpriteSystem::CalculateParticleStartTime(Entity* entity, const ParticleEmitter& emitter, double emitDelta,
                                                int particleIndex)
{
	double result = emitter.isHeapEmit ? entity->localTime : emitter.lastEmitTime + emitDelta * (particleIndex + 1);
	return result;
}

void SpriteSystem::CreateParticle(const Transform& worldTransform, ParticleEmitter* emitter,
                                  const Orientation& direction, double startTime)
{
	if (emitter->isReverse)
	{
		CreateReversedParticle(worldTransform, emitter, direction, startTime);
	}
	else
	{
		CreateNormalParticle(worldTransform, emitter, direction, startTime);
	}
}

void SpriteSystem::CreateReversedParticle(const Transform& worldTransform, ParticleEmitter* emitter,
                                          const Orientation& direction, double startTime)
{
	float lifetime = emitter->lifetime * 0.001f;
	float acceleration = (emitter->speed.second - emitter->speed.first) / lifetime;
	float distance = emitter->speed.first * lifetime + acceleration * pow(lifetime, 2) / 2;

	point3d position = worldTransform.position + direction.forward * distance;

	emitter->particles.push_back(XMFLOAT4X4(
		position.x, position.y, position.z, static_cast<float>(startTime),
		-direction.forward.x, -direction.forward.y, -direction.forward.z, 0,
		0, 0, 0, 0,
		0, 0, 0, 0
	));
}

void SpriteSystem::CreateNormalParticle(const Transform& worldTransform, ParticleEmitter* emitter,
                                        const Orientation& direction, double startTime)
{
	emitter->particles.push_back(XMFLOAT4X4(
		worldTransform.position.x, worldTransform.position.y, worldTransform.position.z,
		static_cast<float>(startTime),
		direction.forward.x, direction.forward.y, direction.forward.z, 0,
		0, 0, 0, 0,
		0, 0, 0, 0
	));
}

void SpriteSystem::UpdateEmitTiming(Entity* entity, ParticleEmitter* emitter, double emitDelta, int emittedCount)
{
	if (emitter->isHeapEmit)
	{
		emitter->lastEmitTime = entity->localTime;
		emitter->heapCount++;

		if (emitter->heapEmitRepeats > 0 && emitter->heapCount >= emitter->heapEmitRepeats)
		{
			emitter->heapCount = 0;
			emitter->active = false;
		}
	}
	else
	{
		emitter->lastEmitTime += emitDelta * emittedCount;
	}
}

void SpriteSystem::UpdateExistingParticles(Entity* entity, ParticleEmitter* emitter)
{
	int i = 0;
	while (i < emitter->particles.size())
	{
		float startTime = emitter->particles[i]._14;

		if (entity->localTime - startTime < emitter->lifetime)
		{
			if (i < constCount)
			{
				ConstBuf::drawerFloat4x4[i] = emitter->particles[i];
			}
			i++;
		}
		else
		{
			emitter->particles.erase(emitter->particles.begin() + i);
		}
	}
}

void SpriteSystem::RenderParticles(Entity* entity, const ParticleEmitter* emitter)
{
	SetupShaders(emitter);
	SetupConstantBuffers(entity);
	SetupParticleInfo(emitter);
	SetupInputAssembler(emitter);

	size_t particleCount = emitter->particles.size();
	context->DrawInstanced(6, min(particleCount, constCount - 1), 0, 0);
}

void SpriteSystem::SetupShaders(const ParticleEmitter* emitter)
{
	Shaders::vShader(emitter->vShader);
	Shaders::pShader(emitter->pShader);
	Shaders::gShader(emitter->gShader);
}

void SpriteSystem::SetupConstantBuffers(Entity* entity)
{
	ConstBuf::Update(10, ConstBuf::drawerFloat4x4);
	ConstBuf::ConstToVertex(10);
	ConstBuf::ConstToPixel(10);
	ConstBuf::ConstToGeometry(10);

	ConstBuf::drawerV[0] = (float)entity->localTime;
	ConstBuf::Update(0, ConstBuf::drawerV);
	ConstBuf::ConstToVertex(0);
	ConstBuf::ConstToPixel(0);
}

void SpriteSystem::SetupParticleInfo(const ParticleEmitter* emitter)
{
	ConstBuf::ParticlesDesc& info = ConstBuf::particlesInfo;

	if (emitter->isReverse)
	{
		info.size = XMFLOAT2(emitter->size.second, emitter->size.first);
		info.opacity = XMFLOAT2(emitter->opacity.second, emitter->opacity.first);
		info.speed = XMFLOAT2(emitter->speed.second, emitter->speed.first);
	}
	else
	{
		info.size = XMFLOAT2(emitter->size.first, emitter->size.second);
		info.opacity = XMFLOAT2(emitter->opacity.first, emitter->opacity.second);
		info.speed = XMFLOAT2(emitter->speed.first, emitter->speed.second);
	}

	info.color = XMFLOAT3(emitter->color.x, emitter->color.y, emitter->color.z);
	info.lifetime = emitter->lifetime;

	ConstBuf::UpdateParticlesInfo();
	ConstBuf::ConstToVertex(9);
	ConstBuf::ConstToPixel(9);
	ConstBuf::ConstToGeometry(9);
}

void SpriteSystem::SetupInputAssembler(const ParticleEmitter* emitter)
{
	if (emitter->gShader == 0)
	{
		InputAssembler::IA(InputAssembler::topology::triList);
	}
	else
	{
		InputAssembler::IA(InputAssembler::topology::pointList);
	}
}

Orientation SpriteSystem::CalculateEmitOrientation(
	const Orientation& baseOrientation,
	EmitDirection emitDir)
{
	Orientation result;

	switch (emitDir)
	{
	case Front:
		result.SetFromDirections(
			baseOrientation.forward,
			baseOrientation.right,
			baseOrientation.up
		);
		break;

	case Back:
		result.SetFromDirections(
			-baseOrientation.forward,
			-baseOrientation.right,
			baseOrientation.up
		);
		break;

	case Right:
		result.SetFromDirections(
			baseOrientation.right,
			-baseOrientation.forward,
			baseOrientation.up
		);
		break;

	case Left:
		result.SetFromDirections(
			-baseOrientation.right,
			baseOrientation.forward,
			baseOrientation.up
		);
		break;

	case Up:
		result.SetFromDirections(
			baseOrientation.up,
			baseOrientation.right,
			-baseOrientation.forward
		);
		break;

	case Bottom:
		result.SetFromDirections(
			-baseOrientation.up,
			baseOrientation.right,
			baseOrientation.forward
		);
		break;

	default:
		result = baseOrientation;
		break;
	}

	return result;
}

// End process particle