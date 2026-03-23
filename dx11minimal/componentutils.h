#ifndef _COMPONENT_UTILS_H_
#define _COMPONENT_UTILS_H_

#include "dx11.h"
#include "singleton.h"

#include "entityStorage.h"
#include "entity.h"

#include "Transform.h"
#include "SphereCollider.h"


XMMATRIX GetWorldMatrix(Transform);
Transform GetTransformDelta(Transform, Transform);

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