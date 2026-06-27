#ifndef _COMPONENT_UTILS_H_
#define _COMPONENT_UTILS_H_

#include "../dx11.h"
#include "../Lib/singleton.h"

#include "../ECS_Base/entityStorage.h"
#include "../ECS_Base/entity.h"

#include "../BasicComponents/Transform.h"
#include "../Physic/Collision/SphereCollider.h"


inline XMFLOAT3 Point3DToXMFLOAT3(const point3d& p);
inline point3d XMFLOAT3ToPoint3D(const XMFLOAT3& v);
inline XMVECTOR LoadPoint3D(const point3d& p);
inline point3d StorePoint3D(FXMVECTOR v);

XMMATRIX GetWorldMatrix(const Transform&);

Transform GetRelativeTransform(const Transform& parentWorldTransform, const Transform& childWorldTransform);

static bool IsInFilters(Entity* entity, std::vector<Entity*> filters) {
	for (Entity* e : filters) {
		if (e == entity) {
			return true;
		}
	}
	return false;
}

template <typename T>
CollisionInfo GetCollisionWithComponent(SphereCollider* sphereCollider, std::vector<Entity*> filters = std::vector<Entity*>()) {
	EntityStorage* entityStorage = Singleton::GetInstance<EntityStorage>();

	for (int i = 0; i < sphereCollider->collisions.size(); i++) {
		CollisionInfo& info = sphereCollider->collisions[i];

		Entity* entity = entityStorage->GetEntityById(info.entityId);
		if (!IsEntityValid(entity)) {
			continue;
		}

		if (IsInFilters(entity, filters)) {
			continue;
		}

		if (entity->HasComponent<T>()) {
			return sphereCollider->collisions[i];
		}
	}

	return CollisionInfo();
}

CollisionInfo GetProjectileCollisionInfo(EntityStorage* entityStorage, Entity* projectile);

void DrawDebugString(std::wstring text, point3d screenPos = point3d(0.75f, 0.75f, 0));


#endif