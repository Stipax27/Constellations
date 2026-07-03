#include "MapBuild.h"

#include "../Engine/Lib/singleton.h"
#include "../Engine/ECS_Base/entityStorage.h"

#include "../Engine/Render/Star.h"
#include "../Engine/Physic/Collision/SphereCollider.h"
#include "../Engine/Physic/Gravitation/GravityPoint.h"
#include "../Engine/Physic/Movement/RotatingSystem.h"


void MapBuild::BuildMaze() {
	EntityStorage* entityStorage = Singleton::GetInstance<EntityStorage>();

	Transform t = Transform();
	t.position = point3d(0, 10, 0);
	t.scale = point3d(5);

	CreateRotatingStar(t, 25.0f, entityStorage->GetEntityByName("World"));

	t.mRotation = XMMatrixRotationAxis(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), PI / 2);

	CreateRotatingStar(t, 40.0f, entityStorage->GetEntityByName("World"));

	t.position = point3d(10, 10, 0);

	CreateRotatingStar(t, 10.0f, entityStorage->GetEntityByName("World"));

	t.position = point3d(20, 10, 0);

	CreateRotatingStar(t, 50.0f, entityStorage->GetEntityByName("World"));

	t.position = point3d(30, 10, 0);

	CreateRotatingStar(t, 15.0f, entityStorage->GetEntityByName("World"));
}


void MapBuild::CreateRotatingStar(const Transform& onTransform, float rotateSpeed, Entity* parent) {
	EntityStorage* entityStorage = Singleton::GetInstance<EntityStorage>();

	///////////////////////////////////

	Entity* starHolder = entityStorage->CreateEntity("RotatingStarHolder", parent);

	Transform* transform = starHolder->AddComponent<Transform>();
	*transform = onTransform;

	RotatingBody* rotatingBody = starHolder->AddComponent<RotatingBody>();
	rotatingBody->rotateSpeed = rotateSpeed;

	///////////////////////////////////

	Entity* starEntity = entityStorage->CreateEntity("RotatingStar", starHolder);

	transform = starEntity->AddComponent<Transform>();
	transform->position = point3d(0, 0, 10);

	Star* star = starEntity->AddComponent<Star>();

	SphereCollider* sphereCollider = starEntity->AddComponent<SphereCollider>();
	sphereCollider->radius = onTransform.scale.x / 2;

	GravityPoint* gravityPoint = starEntity->AddComponent<GravityPoint>();
	gravityPoint->radius = 25.0f;
	gravityPoint->mass = 50.0f;
}