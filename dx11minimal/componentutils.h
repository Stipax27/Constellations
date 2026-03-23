#ifndef _COMPONENT_UTILS_H_
#define _COMPONENT_UTILS_H_

#include "dx11.h"
#include "singleton.h"

#include "entityStorage.h"
#include "entity.h"

#include "Transform.h"
#include "SphereCollider.h"


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


#endif