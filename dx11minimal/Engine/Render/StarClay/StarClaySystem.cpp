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

			if (!frustum->CheckSphere(worldTransform.position, worldTransform.scale.magnitude()))
				continue;

			for (Blob& blob : starClay->blobs) {
				Transform blobTransform = worldTransform;
				blobTransform.position += worldTransform.GetRightVector() * blob.pos.x + worldTransform.GetUpVector() * blob.pos.y + worldTransform.GetLookVector() * blob.pos.z;
				blobTransform.scale = point3d(blob.radius);

				ConstBuf::drawerMatrix[0] = GetWorldMatrix(blobTransform);
				ConstBuf::Update(8, ConstBuf::drawerMatrix);
				ConstBuf::ConstToVertex(8);

				ConstBuf::global[1] = XMFLOAT4(0.04f, 0.0f, 0.19f, 1.0f);
				ConstBuf::Update(5, ConstBuf::global);
				ConstBuf::ConstToVertex(5);

				Shaders::vShader(starClay->vShader);
				Shaders::pShader(starClay->pShader);

				int n = GetVertexCount(blobTransform.position, 3, 15, 1);

				ConstBuf::drawerV[0] = n;
				ConstBuf::Update(0, ConstBuf::drawerV);
				ConstBuf::ConstToVertex(0);

				context->DrawInstanced(n * n * 6, 1, 0, 0);
			}
		}

	}
}


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