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

XMMATRIX GetWorldMatrix(Transform);

Transform GetRelativeTransform(const Transform& parentWorldTransform, const Transform& childWorldTransform);

template <typename T>
CollisionInfo GetCollisionWithComponent(SphereCollider* sphereCollider) {
	EntityStorage* entityStorage = Singleton::GetInstance<EntityStorage>();

	for (CollisionInfo info : sphereCollider->collisions) {
		Entity* entity = entityStorage->GetEntityById(info.entityId);
		if (!IsEntityValid(entity)) {
			continue;
		}

		if (entity->HasComponent<T>()) {
			return info;
		}
	}

	return CollisionInfo();
}

const CollisionInfo GetProjectileCollisionInfo(EntityStorage* entityStorage, Entity* projectile);


#endif