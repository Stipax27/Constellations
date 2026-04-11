#include "StarClaySystem.h"

using namespace std;


StarClaySystem::StarClaySystem()
{
	frustum = Singleton::GetInstance<FrustumClass>();
	camera = Singleton::GetInstance<CameraClass>();
}


void StarClaySystem::Initialize()
{
}


void StarClaySystem::Shutdown()
{
	if (frustum)
		frustum = 0;

	if (camera)
		camera = 0;
}


void StarClaySystem::Update(EntityStorage& entityStorage, float deltaTime)
{
	Blend::Blending(Blend::blendmode::alpha, Blend::blendop::add);
	Rasterizer::Cull(Rasterizer::cullmode::back);
	Depth::Depth(Depth::depthmode::on);

	Shaders::gShader(0);

	InputAssembler::IA(InputAssembler::topology::triList);
	InputAssembler::vBufferNull();

	const std::vector<Entity*>& entities = entityStorage.GetEntitiesWithComponent<StarClay>();
	size_t size = entities.size();
	for (int i = 0; i < size; i++)
	{
		Entity* entity = entities[i];
		if (!IsEntityValid(entity))
			continue;

		StarClay* starClay = entity->GetComponent<StarClay>();
		if (starClay != nullptr && starClay->active)
		{
			Transform worldTransform = GetWorldTransform(entity);
			const double localTime = entity->localTime;

			if (!frustum->CheckSphere(worldTransform.position, worldTransform.scale.magnitude()))
				continue;

			ClearOldBlobs(starClay->blobs, starClay->lifetime, localTime);
			ClearOldBlobs(starClay->out_blobs, starClay->lifetime, localTime);

			EmitNewBlobs(entity, starClay);
			RenderBlobs(starClay, worldTransform, localTime);
		}

	}
}

//// Blobs processing ////

void StarClaySystem::ClearOldBlobs(std::vector<Blob>& blobs, const double& lifetime, const double& localTime)
{
	auto it = std::remove_if(blobs.begin(), blobs.end(),
		[lifetime, localTime](const auto& blob) {
			return localTime - blob.startTime > lifetime;
		});

	blobs.erase(it, blobs.end());
}

void StarClaySystem::EmitNewBlobs(Entity* entity, StarClay* starClay)
{
	double emitDelta = 1000.0f / starClay->rate;
	double elapsedTime = abs(entity->localTime - starClay->lastEmitTime);

	if (elapsedTime >= emitDelta)
	{
		int blobsToEmit = min(static_cast<int>(elapsedTime / emitDelta), constCount);

		for (int i = 0; i < blobsToEmit; i++)
		{
			point3d pos = getRandomDirection() * getRandomFloat(0.0f, starClay->coreRadius);
			double startTime = starClay->lastEmitTime + emitDelta * (i + 1);
			float radius = getRandomFloat(starClay->blobsRadius.first, starClay->blobsRadius.second);

			Blob blob = Blob(pos, radius, startTime);
			starClay->blobs.push_back(blob);
		}

		starClay->lastEmitTime += emitDelta * blobsToEmit;
	}
}


void StarClaySystem::RenderBlobs(StarClay* starClay, Transform& worldTransform, const double& localTime)
{
	int n = GetVertexCount(worldTransform.position, 3, 15, 1);
	int blobsSize = starClay->blobs.size();

	if (blobsSize == 0)
		return;

	for (int a = 0; a < blobsSize; a++) {
		Blob& blob = starClay->blobs[a];

		Transform blobTransform = worldTransform;
		blobTransform.position += worldTransform.GetRightVector() * blob.pos.x + worldTransform.GetUpVector() * blob.pos.y + worldTransform.GetLookVector() * blob.pos.z;

		float timeMultiplier = (localTime - blob.startTime) / starClay->lifetime;
		timeMultiplier = min(timeMultiplier, 1.0f - timeMultiplier) * 2;

		blobTransform.scale = point3d(blob.radius * timeMultiplier);

		ConstBuf::drawerMatrix[a] = GetWorldMatrix(blobTransform);
	}

	ConstBuf::Update(8, ConstBuf::drawerMatrix);
	ConstBuf::ConstToVertex(8);

	ConstBuf::global[0] = XMFLOAT4(worldTransform.position.x, worldTransform.position.y, worldTransform.position.z, 1.0f);
	ConstBuf::global[1] = XMFLOAT4(0.04f, 0.0f, 0.19f, 1.0f);
	ConstBuf::Update(5, ConstBuf::global);
	ConstBuf::ConstToVertex(5);

	ConstBuf::drawerV[0] = n;
	ConstBuf::Update(0, ConstBuf::drawerV);
	ConstBuf::ConstToVertex(0);

	Shaders::vShader(starClay->vShader);
	Shaders::pShader(starClay->pShader);

	context->DrawInstanced(n * n * 6, blobsSize, 0, 0);
}

//// Other functions ////

void StarClaySystem::UpdateWorldMatrix(Transform worldTransform) {
	ConstBuf::camera.world = GetWorldMatrix(worldTransform);
	ConstBuf::UpdateCamera();
	ConstBuf::ConstToVertex(3);
	ConstBuf::ConstToPixel(3);
}

int StarClaySystem::GetVertexCount(point3d position, int min, int max, float size) {
	size = max(size, 1);
	float dist = (camera->position - position).magnitude();

	float a = dist - HIGH_RENDER_DISTANCE * size;
	a /= RENDER_DISTANCE_DELTA * size;
	a = clamp(a, 0, 1);

	int n = (int)lerp((float)max, (float)min, a);
	n += 1 - n % 2;

	return n;
}