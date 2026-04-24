#include "componentutils.h"

#include "../Compute/Combat/Health.h"


inline XMFLOAT3 Point3DToXMFLOAT3(const point3d& p) {
    return XMFLOAT3(p.x, p.y, p.z);
}

inline point3d XMFLOAT3ToPoint3D(const XMFLOAT3& v) {
    return point3d(v.x, v.y, v.z);
}

inline XMVECTOR LoadPoint3D(const point3d& p) {
    return XMVectorSet(p.x, p.y, p.z, 0.0f);
}

inline point3d StorePoint3D(FXMVECTOR v) {
    XMFLOAT3 f;
    XMStoreFloat3(&f, v);
    return point3d(f.x, f.y, f.z);
}


XMMATRIX GetWorldMatrix(Transform worldTransform) {
	XMMATRIX rotateMatrix = worldTransform.mRotation;
	XMMATRIX scaleMatrix = XMMatrixScaling(worldTransform.scale.x, worldTransform.scale.y, worldTransform.scale.z);
	XMMATRIX translateMatrix = XMMatrixTranslation(worldTransform.position.x, worldTransform.position.y, worldTransform.position.z);

	XMMATRIX srMatrix = scaleMatrix * rotateMatrix;
	XMMATRIX worldMatrix = srMatrix * translateMatrix;

	return XMMatrixTranspose(worldMatrix);
}


Transform GetRelativeTransform(const Transform& parentWorldTransform, const Transform& childWorldTransform) {
    Transform relativeTransform;

    // Относительная позиция
    point3d relativePosition = point3d(
        childWorldTransform.position.x - parentWorldTransform.position.x,
        childWorldTransform.position.y - parentWorldTransform.position.y,
        childWorldTransform.position.z - parentWorldTransform.position.z
    );

    // Обратная матрица поворота
    DirectX::XMMATRIX invParentRotation = DirectX::XMMatrixTranspose(parentWorldTransform.mRotation);

    // Поворачиваем относительную позицию
    DirectX::XMVECTOR posVec = LoadPoint3D(relativePosition);
    posVec = DirectX::XMVector3Transform(posVec, invParentRotation);
    relativeTransform.position = StorePoint3D(posVec);

    // Относительный масштаб
    relativeTransform.scale = point3d(
        childWorldTransform.scale.x / parentWorldTransform.scale.x,
        childWorldTransform.scale.y / parentWorldTransform.scale.y,
        childWorldTransform.scale.z / parentWorldTransform.scale.z
    );

    // Относительный поворот
    relativeTransform.mRotation = DirectX::XMMatrixMultiply(
        childWorldTransform.mRotation,
        invParentRotation
    );

    return relativeTransform;
}


const CollisionInfo& GetProjectileCollisionInfo(EntityStorage* entityStorage, Entity* projectile)
{
    SphereCollider* sphereCollider = projectile->GetComponent<SphereCollider>();

    if (sphereCollider != nullptr) {
        for (CollisionInfo info : sphereCollider->collisions) {
            Entity* entity = entityStorage->GetEntityById(info.entityId);
            if (!IsEntityValid(entity)) {
                continue;
            }

            Health* health = entity->GetComponentInAncestor<Health>();
            if (health != nullptr && health->active) {
                return info;
            }
        }
    }

    return CollisionInfo();
}