#include "MapBuild.h"

#include "../Engine/Lib/singleton.h"
#include "../Engine/ECS_Base/entityStorage.h"

#include "../Engine/Render/Star.h"
#include "../Engine/Physic/Collision/SphereCollider.h"
#include "../Engine/Physic/Movement/RotatingSystem.h"


void MapBuild::BuildMaze() {
	Transform t = Transform();
	t.position = point3d(0, 10, 0);

	CreateRotatingStar(t, nullptr);
}


void MapBuild::CreateRotatingStar(const Transform& onTransform, Entity* parent) {
	EntityStorage* entityStorage = Singleton::GetInstance<EntityStorage>();

	Entity* entity = entityStorage->CreateEntity("RotatingStar", parent);

	Transform* transform = entity->AddComponent<Transform>();
	*transform = onTransform;

	Star* star = entity->AddComponent<Star>();

	SphereCollider* sphereCollider = entity->AddComponent<SphereCollider>();

	RotatingBody* rotatingBody = entity->AddComponent<RotatingBody>();
}