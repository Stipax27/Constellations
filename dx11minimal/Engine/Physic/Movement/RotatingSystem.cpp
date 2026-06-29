#include "RotatingSystem.h"

#include "../../Utils/utils.h"

using namespace std;



RotatingSystem::RotatingSystem()
{
}


void RotatingSystem::Initialize()
{
}


void RotatingSystem::Shutdown()
{
}


void RotatingSystem::Update(EntityStorage& entityStorage, float deltaTime)
{
	const std::vector<Entity*>& entities = entityStorage.GetEntitiesWithComponent<RotatingBody>();

	for (Entity* entity : entities)
	{
		if (!IsEntityValid(entity) || entity->GetTimeScale() == 0.0f)
			continue;

		Transform* transform = entity->GetComponent<Transform>();
		RotatingBody* rotatingBody = entity->GetComponent<RotatingBody>();
		if (transform == nullptr || rotatingBody == nullptr || !rotatingBody->active)
			continue;

		float angle = degreesToRadians(rotatingBody->rotateSpeed) * timer::deltaTimeS * entity->GetTimeScale();
		XMVECTOR quaternion = XMQuaternionRotationAxis(XMVectorSet(rotatingBody->axis.x, rotatingBody->axis.y, rotatingBody->axis.z, 0.0f), angle);
		XMMATRIX rotMatrix = XMMatrixRotationQuaternion(quaternion);

		transform->mRotation = rotMatrix * transform->mRotation;
	}
}